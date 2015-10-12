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
    if(Q_UNLIKELY(_config.network_default_size_grow <= 0))
    {
        QNN_FATAL_MSG("max_time_constant must be greater then 0");
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
    if(Q_UNLIKELY(_config.size_network < len_output))
    {
        QNN_FATAL_MSG("size_network must be greater then len_output");
    }
    if(Q_UNLIKELY(_config.max_time_constant < 1))
    {
        QNN_FATAL_MSG("max_time_constant must be greater then 0");
    }
    if(Q_UNLIKELY(_config.size_changing && _config.max_size_network < _config.size_network))
    {
        QNN_FATAL_MSG("max_size_network must not be smaller than size_network");
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
    delete [] _network;
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
    if(Q_UNLIKELY(_gene->segments().size() < _config.size_network || _gene->segments()[0].size() < (3 + _config.size_network)))
    {
        QNN_FATAL_MSG("Gene lenght does not fit");
    }
    if(Q_UNLIKELY(_config.size_changing && _gene->segments()[0].size() < (3 + _config.max_size_network)))
    {
        QNN_FATAL_MSG("Gene lenght does not fit max_size_network");
    }
    _network = new double[_gene->segments().size()];
    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        _network[i] = 0;
    }
}

void ContinuousTimeRecurrenNeuralNetwork::_processInput(QList<double> input)
{
    double *newNetwork = new double[_gene->segments().size()];

    // do calculation
    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        double newValue = -1 * _network[i]; // -y

        if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[_gene->segments()[i][gene_input]%(_len_input+1)-1];; // input
        }

        for(qint32 j = 0; j < _gene->segments().size(); ++j)
        {
            double d = 0.0;
            d += weight(_gene->segments()[j][gene_bias], _config.bias_scalar); // θj
            d += _network[j]; // yj
            d = _config.activision_function(d);
            newValue += d * weight(_gene->segments()[i][gene_W_start+j], _config.weight_scalar); // wij
        }
        newNetwork[i] = newValue / ((_gene->segments()[i][gene_time_constraint]%_config.max_time_constant)+1); // τ
        newNetwork[i] += _network[i];
    }

    delete [] _network;
    _network = newNetwork;
}

double ContinuousTimeRecurrenNeuralNetwork::_getNeuronOutput(qint32 i)
{
    if(Q_LIKELY(_network != NULL && i < _len_output))
    {
        return _config.activision_function(_network[i] + weight(_gene->segments()[i][gene_bias], _config.bias_scalar));
    }
    else
    {
        QNN_CRITICAL_MSG("i out of bounds");
        return -1.0;
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

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connection_neuron;

        config_neuron["qint32ernal_value"] = _network[i];
        config_neuron["bias"] = weight(_gene->segments()[i][gene_bias], _config.bias_scalar);
        config_neuron["time_constant"] = (_gene->segments()[i][gene_time_constraint]%_config.max_time_constant)+1;

        if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
        {
            config_neuron["input"] =_gene->segments()[i][gene_input]%(_len_input+1)-1;
        }

        for(qint32 j = 0; j < _gene->segments().size(); ++j)
        {
            connection_neuron[j] = weight(_gene->segments()[i][gene_W_start+j], _config.weight_scalar);
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
