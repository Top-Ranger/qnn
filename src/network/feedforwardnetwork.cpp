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

#include "feedforwardnetwork.h"

#include "commonnetworkfunctions.h"
#include "networktoxml.h"

#include <QDebug>
#include <QtCore/qmath.h>

using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::sigmoid;

using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigNeuron;
using NetworkToXML::writeConfigEnd;

FeedForwardNetwork::FeedForwardNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _hidden_layers(NULL),
    _output(NULL)
{
}

FeedForwardNetwork::FeedForwardNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _hidden_layers(NULL),
    _output(NULL)
{
}

FeedForwardNetwork::~FeedForwardNetwork()
{
    if(_hidden_layers != NULL)
    {
        for(qint32 i = 0; i < _config.num_hidden_layer; ++i)
        {
            delete [] _hidden_layers[i];
        }
        delete [] _hidden_layers;
    }
    delete [] _output;
}

void FeedForwardNetwork::_initialise()
{
    if(Q_UNLIKELY(_gene->segments().length() < num_segments(_len_input, _len_output, _config.num_hidden_layer, _config.len_hidden)))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    else if(Q_UNLIKELY(_config.len_hidden <= 0 || _config.num_hidden_layer < 0))
    {
        qFatal(QString("FATAL ERROR in %1 %2: invalid hidden layer size!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    if(_config.len_hidden > 0)
    {
        _hidden_layers = new double*[_config.num_hidden_layer];
        for(qint32 i = 0; i < _config.num_hidden_layer; ++i)
        {
            _hidden_layers[i] = new double[_config.len_hidden];
        }
    }
    _output = new double[_len_output];
}

void FeedForwardNetwork::_processInput(QList<double> input)
{
    if(Q_UNLIKELY(input.length() != _len_input))
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << __LINE__ << ": input length" << input.length() << "!= _len_input" << _len_input;
        return;
    }

    if(_config.num_hidden_layer == 0)
    {
        qint32 current_segment = 0;
        for(qint32 i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0d;
            for(qint32 i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            _output[i_output] = _config.activision_function(sum);
        }
    }
    else
    {
        qint32 current_segment = 0;

        // Input to hidden
        for(qint32 i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
        {
            double sum = 0.0d;
            for(qint32 i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            _hidden_layers[0][i_hidden] = _config.activision_function(sum);
        }

        // Hidden to hidden
        for(qint32 current_hidden = 1; current_hidden < _config.num_hidden_layer; ++current_hidden)
        {
            for(qint32 i_output = 0; i_output < _config.len_hidden; ++i_output)
            {
                double sum = 0.0d;
                for(qint32 i_input = 0; i_input < _config.len_hidden; ++i_input)
                {
                    sum += _hidden_layers[current_hidden-1][i_input] * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
                }
                sum += 1.0d * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
                _hidden_layers[current_hidden][i_output] = _config.activision_function(sum);
            }
        }

        // Hidden to output
        for(qint32 i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0d;
            for(qint32 i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
            {
                sum += _hidden_layers[_config.num_hidden_layer-1][i_hidden] * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            _output[i_output] = _config.activision_function(sum);
        }
    }
}

double FeedForwardNetwork::_getNeuronOutput(qint32 i)
{
    if(Q_LIKELY(i >= 0 && i < _len_output))
    {
        return _output[i];
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << __LINE__ << ": i out of bound";
        return -1.0;
    }
}

bool FeedForwardNetwork::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    QMap<QString, QVariant> config_network;
    config_network["num_hidden_layer"] = _config.num_hidden_layer;
    config_network["len_hidden"] = _config.len_hidden;
    config_network["activision_function"] = _config.activision_function == &standard_activision_function ? "standard" : "non-standard";
    config_network["weight_scalar"] = _config.weight_scalar;
    config_network["len_input"] = _len_input;
    config_network["len_output"] = _len_output;

    writeConfigStart("FeedForwardNetwork", config_network, stream);

    for(qint32 i = 0; i < _len_input; ++i)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connections_neuron;

        writeConfigNeuron(i, config_neuron, connections_neuron, stream);
    }

    if(_config.num_hidden_layer == 0)
    {
        qint32 current_segment = 0;
        for(qint32 i_output = 0; i_output < _len_output; ++i_output)
        {
            QMap<QString, QVariant> config_neuron;
            QMap<qint32, double> connections_neuron;
            for(qint32 i_input = 0; i_input < _len_input; ++i_input)
            {
                connections_neuron[i_input] = weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            writeConfigNeuron(_len_input+i_output, config_neuron, connections_neuron, stream);
        }
    }
    else
    {
        qint32 current_segment = 0;

        // Input to hidden
        for(qint32 i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
        {
            QMap<QString, QVariant> config_neuron;
            QMap<qint32, double> connections_neuron;
            for(qint32 i_input = 0; i_input < _len_input; ++i_input)
            {
                connections_neuron[i_input] = weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            writeConfigNeuron(_len_input+i_hidden, config_neuron, connections_neuron, stream);
        }

        // Hidden to hidden
        for(qint32 current_hidden = 1; current_hidden < _config.num_hidden_layer; ++current_hidden)
        {
            for(qint32 i_output = 0; i_output < _config.len_hidden; ++i_output)
            {
                QMap<QString, QVariant> config_neuron;
                QMap<qint32, double> connections_neuron;
                for(qint32 i_input = 0; i_input < _config.len_hidden; ++i_input)
                {
                    connections_neuron[_config.len_hidden*(current_hidden-1)+_len_input+i_input] = weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
                }
                writeConfigNeuron(_config.len_hidden*current_hidden+_len_input+i_output, config_neuron, connections_neuron, stream);
            }
        }

        // Hidden to output
        for(qint32 i_output = 0; i_output < _len_output; ++i_output)
        {
            QMap<QString, QVariant> config_neuron;
            QMap<qint32, double> connections_neuron;
            for(qint32 i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
            {
                connections_neuron[_config.len_hidden*(_config.num_hidden_layer-1)+_len_input+i_hidden] = weight(_gene->segments()[current_segment++][0], _config.weight_scalar);
            }
            writeConfigNeuron(_config.len_hidden*_config.num_hidden_layer+_len_input+i_output, config_neuron, connections_neuron, stream);
        }
    }

    writeConfigEnd(stream);
    return true;
}

qint32 FeedForwardNetwork::num_segments(qint32 len_input, qint32 len_output, qint32 hidden_layer, qint32 len_hidden)
{
    if(hidden_layer == 0)
    {
        return len_input * len_output + len_output;
    }
    else
    {
        return len_input*len_hidden + len_hidden*len_hidden*(hidden_layer-1) + len_output*len_hidden + len_output + hidden_layer*len_hidden;
    }
}

double FeedForwardNetwork::standard_activision_function(double input)
{
    return sigmoid(input);
}

GenericGene *FeedForwardNetwork::getRandomGene()
{
    return new GenericGene(num_segments(_len_input, _len_output, _config.num_hidden_layer, _config.len_hidden));
}

AbstractNeuralNetwork *FeedForwardNetwork::createConfigCopy()
{
    return new FeedForwardNetwork(_len_input, _len_output, _config);
}
