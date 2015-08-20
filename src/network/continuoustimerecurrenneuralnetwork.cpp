/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn.
 *
 * qnn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with qnn.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "continuoustimerecurrenneuralnetwork.h"

#include "commonnetworkfunctions.h"
#include "lengthchanginggene.h"
#include "networktoxml.h"

#include <QDebug>
#include <math.h>

// GENE ENCODING: θ, input, τ, W
// weight between -5,5

using CommonNetworkFunctions::sigmoid;
using CommonNetworkFunctions::weight;

using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigNeuron;
using NetworkToXML::writeConfigEnd;

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _network(NULL)
{
    if(_config.network_default_size_grow <= 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: max_time_constant must be greater than 0!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.size_network == -1)
    {
        _config.size_network = 0;
        do {
            _config.size_network += _config.network_default_size_grow;
        } while(_config.size_network < len_output);
    }
    if(_config.size_changing && _config.max_size_network == -1)
    {
        _config.max_size_network = _config.size_network * 4;
    }
    if(_config.size_network < len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: size_network must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.max_time_constant < 1)
    {
        qFatal(QString("FATAL ERROR in %1 %2: max_time_constant must be 1 or bigger!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.size_changing && _config.max_size_network < _config.size_network)
    {
        qFatal(QString("FATAL ERROR in %1 %2: max_size_network must not be smaller than size_network!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _network(NULL)
{
}

ContinuousTimeRecurrenNeuralNetwork::~ContinuousTimeRecurrenNeuralNetwork()
{
    if(_network != NULL)
    {
        delete [] _network;
    }
}

GenericGene *ContinuousTimeRecurrenNeuralNetwork::getRandomGene()
{
    if(_config.size_changing)
    {
        LengthChangingGene::config gene_config;
        gene_config.min_length = _config.size_network;
        gene_config.max_length = _config.max_size_network;
        return new LengthChangingGene(_config.size_network, 3 + _config.max_size_network, gene_config);
    }
    else
    {
        return new GenericGene(_config.size_network, 3 + _config.size_network);
    }
}

AbstractNeuralNetwork *ContinuousTimeRecurrenNeuralNetwork::createConfigCopy()
{
    return new ContinuousTimeRecurrenNeuralNetwork(_len_input, _len_output, _config);
}

void ContinuousTimeRecurrenNeuralNetwork::_initialise()
{
    if(_gene->segments().length() < _config.size_network || _gene->segments()[0].length() < (3 + _config.size_network))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Gene lenght does not fit!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.size_changing && _gene->segments()[0].length() < (3 + _config.max_size_network))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Gene lenght does not fit max_size_network!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[_gene->segments().length()];
    for(qint32 i = 0; i < _gene->segments().length(); ++i)
    {
        _network[i] = 0;
    }
}

void ContinuousTimeRecurrenNeuralNetwork::_processInput(QList<double> input)
{
    QList< QList<qint32> > segments = _gene->segments();

    double *newNetwork = new double[segments.length()];

    // do calculation
    for(qint32 i = 0; i < segments.length(); ++i)
    {
        double newValue = -1 * _network[i]; // -y

        if(segments[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[segments[i][gene_input]%(_len_input+1)-1];; // input
        }

        for(qint32 j = 0; j < segments.length(); ++j)
        {
            double d = 0.0d;
            d += weight(segments[j][gene_bias], _config.bias_scalar); // θj
            d += _network[j]; // yj
            d = _config.activision_function(d);
            newValue += d * weight(segments[i][gene_W_start+j], _config.weight_scalar); // wij
        }
        newNetwork[i] = newValue / ((segments[i][gene_time_constraqint32]%_config.max_time_constant)+1); // τ
        newNetwork[i] += _network[i];
    }

    delete [] _network;
    _network = newNetwork;
}

double ContinuousTimeRecurrenNeuralNetwork::_getNeuronOutput(qint32 i)
{
    if(_network != NULL && i < _len_output)
    {
        return _config.activision_function(_network[i] + weight(_gene->segments()[i][gene_bias], _config.bias_scalar));
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0d;
    }
}

bool ContinuousTimeRecurrenNeuralNetwork::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    QMap<QString, QVariant> config_network;
    config_network["size_network"] = _config.size_network;
    config_network["size_changing"] = _config.size_changing;
    config_network["max_size_network"] = _config.max_size_network;
    config_network["max_time_constant"] = _config.max_time_constant;
    config_network["weight_scalar"] = _config.weight_scalar;
    config_network["bias_scalar"] = _config.bias_scalar;
    config_network["network_default_size_grow"] = _config.network_default_size_grow;
    config_network["activision_function"] = _config.activision_function == &standard_activision_function ? "standard" : "non-standard";
    config_network["len_input"] = _len_input;
    config_network["len_output"] = _len_output;

    writeConfigStart("ContinuousTimeRecurrenNeuralNetwork", config_network, stream);

    QList< QList<qint32> > segments = _gene->segments();

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connection_neuron;

        config_neuron["qint32ernal_value"] = _network[i];
        config_neuron["bias"] = weight(segments[i][gene_bias], _config.bias_scalar);
        config_neuron["time_constant"] = (segments[i][gene_time_constraqint32]%_config.max_time_constant)+1;

        if(segments[i][gene_input]%(_len_input+1) != 0)
        {
            config_neuron["input"] =segments[i][gene_input]%(_len_input+1)-1;
        }

        for(qint32 j = 0; j < segments.length(); ++j)
        {
            connection_neuron[j] = weight(segments[i][gene_W_start+j], _config.weight_scalar);
        }

        writeConfigNeuron(i, config_neuron, connection_neuron, stream);
    }

    writeConfigEnd(stream);
    return true;
}

double ContinuousTimeRecurrenNeuralNetwork::standard_activision_function(double input)
{
    return sigmoid(input);
}
