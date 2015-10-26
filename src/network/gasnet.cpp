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

#include "gasnet.h"

#include "lengthchanginggene.h"
#include "commonnetworkfunctions.h"
#include "networktoxml.h"

#include <QString>
#include <QtCore/qmath.h>

#include <math.h>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius, basis index, bias
//                0  1   2   3      4    5    6     7     8     9             10     11               12        13         14        15

using CommonNetworkFunctions::floatFromGeneInput;
using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::calculateDistance;
using CommonNetworkFunctions::areNodesConnected;
using CommonNetworkFunctions::cut01;

using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigNeuron;
using NetworkToXML::writeConfigEnd;

GasNet::GasNet(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _network(NULL),
    _gas_emitting(NULL),
    _distances(NULL),
    _weights(NULL),
    _P()
{
    if(Q_UNLIKELY(_config.area_size <= 0))
    {
        QNN_FATAL_MSG("Area needs to be greater then 0");
    }
    if(Q_UNLIKELY(_config.min_size != -1 && _config.min_size < _len_output))
    {
        QNN_FATAL_MSG("min_size_network must not be smaller then output lenght");
    }
    if(Q_UNLIKELY(_config.offset_rate_of_gas <= 0.0))
    {
        QNN_FATAL_MSG("offset_rate_of_gas must be greater then 0");
    }

    _P.append(-4.0);
    _P.append(-2.0);
    _P.append(-1.0);
    _P.append(-0.5);
    _P.append(-0.25);
    _P.append(0.0);
    _P.append(0.25);
    _P.append(0.5);
    _P.append(1.0);
    _P.append(2.0);
    _P.append(4.0);
}

GasNet::GasNet() :
    AbstractNeuralNetwork(),
    _config(),
    _network(NULL),
    _gas_emitting(NULL),
    _distances(NULL),
    _weights(NULL),
    _P()
{
    _P.append(-4.0);
    _P.append(-2.0);
    _P.append(-1.0);
    _P.append(-0.5);
    _P.append(-0.25);
    _P.append(0.0);
    _P.append(0.25);
    _P.append(0.5);
    _P.append(1.0);
    _P.append(2.0);
    _P.append(4.0);
}

GasNet::~GasNet()
{
    delete [] _network;
    delete [] _gas_emitting;
    if(_distances != NULL && _gene != NULL)
    {
        for(qint32 i = 0; i < _gene->segments().size(); ++i)
        {
            delete [] _distances[i];
        }
        delete [] _distances;
    }
    if(_weights != NULL && _gene != NULL)
    {
        for(qint32 i = 0; i < _gene->segments().size(); ++i)
        {
            delete [] _weights[i];
        }
        delete [] _weights;
    }
    if(_config.neuron_save != NULL && _config.neuron_save_opened)
    {
        _config.neuron_save->close();
    }
    if(_config.gas_save != NULL && _config.gas_save_opened)
    {
        _config.gas_save->close();
    }
}

GenericGene *GasNet::getRandomGene()
{
    LengthChangingGene::config config;
    config.min_length = _config.min_size;
    config.max_length = _config.max_size;

    qint32 initial_length;

    if(config.min_length == -1 || config.max_length == -1)
    {
        initial_length = _len_output;
    }
    else
    {
        if(Q_UNLIKELY(config.min_length > config.max_length))
        {
            QNN_WARNING_MSG("min_length is not smaller then max_length");
            initial_length = _len_output;
        }
        else
        {
            qint32 diff = config.max_length - config.min_length;
            if(diff == 0)
            {
                initial_length = config.min_length;
            }
            else
            {
                initial_length = qrand()%diff + config.min_length;
            }
        }
    }

    return new LengthChangingGene(initial_length, 16, config);
}

AbstractNeuralNetwork *GasNet::createConfigCopy()
{
    config new_config = _config;
    new_config.neuron_save = NULL;
    new_config.gas_save = NULL;
    return new GasNet(_len_input, _len_output, new_config);
}

void GasNet::_initialise()
{
    if(Q_UNLIKELY(_gene->segments().size() < _len_output))
    {
        QNN_FATAL_MSG("Gene length must be bigger then len_output");
    }
    if(Q_UNLIKELY(_gene->segments()[0].size() != 16))
    {
        QNN_FATAL_MSG("Wrong gene segment length");
    }
    _network = new double[_gene->segments().size()];
    _gas_emitting = new double[_gene->segments().size()];

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        _network[i] = 0;
        _gas_emitting[i] = 0;
    }


    // Cache distances and connection for faster calculation later
    _distances = new double*[_gene->segments().size()];
    _weights = new double*[_gene->segments().size()];

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        _distances[i] = new double[_gene->segments().size()];
        _weights[i] = new double[_gene->segments().size()];
        for(qint32 j = 0; j < _gene->segments().size(); ++j)
        {
            // distance
            _distances[i][j] = calculateDistance(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                                  floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                                  floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                                  floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size));

            // weight
            _weights[i][j] = 0;
            if(i == j)
            {
                // recurrent connection
                switch(_gene->segments()[i][gene_recurrent]%3)
                {
                case 1:
                    _weights[i][j] = 1.0;
                    break;
                case 2:
                    _weights[i][j] = -1.0;
                    break;
                default:
                    _weights[i][j] = 0.0;
                    break;
                }
            }
            else
            {
                if(areNodesConnected(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio),
                                     floatFromGeneInput(_gene->segments()[i][gene_PositivConeExt], 2*M_PI),
                                     floatFromGeneInput(_gene->segments()[i][gene_PositivConeOrientation], 2*M_PI)))
                {
                    _weights[i][j] += 1.0;
                }
                if(areNodesConnected(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size),
                                     floatFromGeneInput(_gene->segments()[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio),
                                     floatFromGeneInput(_gene->segments()[i][gene_NegativConeExt], 2*M_PI),
                                     floatFromGeneInput(_gene->segments()[i][gene_NegativConeOrientation], 2*M_PI)))
                {
                    _weights[i][j] += -1.0;
                }
            }
        }
    }

    // Prepare output
    if(_config.neuron_save != NULL)
    {
        if(!_config.neuron_save->isOpen())
        {
            QNN_DEBUG_MSG("Opening device");
            if(!_config.neuron_save->open(QIODevice::WriteOnly))
            {
                QNN_CRITICAL_MSG("Can not open device");
                // setting to null because we can not write to it
                _config.neuron_save = NULL;
            }
            else
            {
                _config.neuron_save_opened = true;
            }
        }
        else
        {
            _config.neuron_save_opened = false;
        }
        if(_config.neuron_save != NULL)
        {
            // write header
            QTextStream stream(_config.neuron_save);
            stream << "Neuron 0";
            for(int i = 1; i < _gene->segments().size(); ++i)
            {
                stream << ";";
                stream << "Neuron " << i;
            }
            stream << "\n";
        }
    }
    if(_config.gas_save != NULL)
    {
        if(!_config.gas_save->isOpen())
        {
            QNN_DEBUG_MSG("Opening device");
            if(!_config.gas_save->open(QIODevice::WriteOnly))
            {
                QNN_CRITICAL_MSG("Can not open device");
                // setting to null because we can not write to it
                _config.gas_save = NULL;
            }
            else
            {
                _config.gas_save_opened = true;
            }
        }
        else
        {
            _config.gas_save_opened = false;
        }
        if(_config.gas_save != NULL)
        {
            // write header
            QTextStream stream(_config.gas_save);
            stream << "positive 0;negative 0";
            for(int i = 1; i < _gene->segments().size(); ++i)
            {
                stream << ";";
                stream << "positive " << i << ";negative " << i;
            }
            stream << "\n";
        }
    }
}

void GasNet::_processInput(QList<double> input)
{
    double gas1[_gene->segments().size()];
    double gas2[_gene->segments().size()];
    double k[_gene->segments().size()];

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate gas concentration
        if(_gas_emitting[i] > 0.0 && _gene->segments()[i][gene_TypeGas]%3 != 0)
        {
            double gas_radius = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
            for(qint32 j = 0; j < _gene->segments().size(); ++j)
            {
                if(_distances[i][j] > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                switch (_gene->segments()[i][gene_TypeGas]%3)
                {
                case 0:
                    // No Gas is emitted
                    break;

                case 1:
                    gas1[j] += gas_concentration;
                    break;

                case 2:
                    gas2[j] += gas_concentration;
                    break;

                default:
                    QNN_WARNING_MSG("Unknown gas");
                    break;
                }
            }
        }
    }

    // write gas
    if(_config.gas_save != NULL)
    {
        QTextStream stream(_config.gas_save);
        stream << _network[0];
        for(int i = 1; i < _gene->segments().size(); ++i)
        {
            stream << ";";
            stream << gas1[i] << ";" << gas2[i];
        }
        stream << "\n";
    }

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate k
        qint32 basis_index = _gene->segments()[i][gene_basis_index]%_P.length();
        qint32 index = qFloor(basis_index + gas1[i] * (_P.length() - basis_index) + gas2[i] * basis_index);
        if(index < 0)
        {
            index = 0;
        }
        else if(index >= _P.length())
        {
            index = _P.length()-1;
        }
        k[i] = _P[index];
    }

    double *newNetwork = new double[_gene->segments().size()];

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate new input
        double newValue = 0;

        // Connections
        for(qint32 j = 0; j < _gene->segments().size(); ++j)
        {
            newValue += _network[j] * _weights[j][i];
        }

        // Input
        if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[_gene->segments()[i][gene_input]%(_len_input+1)-1];
        }

        // K
        newValue *= k[i];

        // Bias
        newValue += weight(_gene->segments()[i][gene_bias], _config.bias_scalar);

        // tanh
        newNetwork[i] = tanh(newValue);
    }

    delete [] _network;
    _network = newNetwork;

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate emition of gas
        bool emittingGas = false;
        switch (_gene->segments()[i][gene_WhenGas]%3)
        {
        case 0: // Electric charge
            if(_network[i] > _config.electric_threshhold)
            {
                emittingGas = true;
            }
            break;

        case 1: // Gas1
            if(gas1[i] > _config.gas_threshhold)
            {
                emittingGas = true;
            }
            break;

        case 2: // Gas2
            if(gas2[i] > _config.gas_threshhold)
            {
                emittingGas = true;
            }
            break;

        default:
            QNN_WARNING_MSG("Unknown gas circumstances");
            break;
        }

        if(emittingGas)
        {
            _gas_emitting[i] = cut01(_gas_emitting[i] + 1.0 / (_config.offset_rate_of_gas + floatFromGeneInput(_gene->segments()[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
        }
        else
        {
            _gas_emitting[i] = cut01(_gas_emitting[i] - 1.0 / (_config.offset_rate_of_gas + floatFromGeneInput(_gene->segments()[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
        }
    }

    // write output
    if(_config.neuron_save != NULL)
    {
        QTextStream stream(_config.neuron_save);
        stream << _network[0];
        for(int i = 1; i < _gene->segments().size(); ++i)
        {
            stream << ";";
            stream << _network[i];
        }
        stream << "\n";
    }
}

double GasNet::_getNeuronOutput(qint32 i)
{
    if(Q_LIKELY(i >= 0 && i < _len_output))
    {
        return _network[i];
    }
    else
    {
        QNN_CRITICAL_MSG("i out of bounds");
        return -1.0;
    }
}

bool GasNet::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    QMap<QString, QVariant> config_network;
    config_network["area_size"] = _config.area_size;
    config_network["bias_scalar"] = _config.bias_scalar;
    config_network["gas_threshhold"] = _config.gas_threshhold;
    config_network["electric_threshhold"] = _config.electric_threshhold;
    config_network["cone_ratio"] = _config.cone_ratio;
    config_network["offset_gas_radius"] = _config.offset_gas_radius;
    config_network["range_gas_radius"] = _config.range_gas_radius;
    config_network["offset_rate_of_gas"] = _config.offset_rate_of_gas;
    config_network["range_rate_of_gas"] = _config.range_rate_of_gas;
    config_network["min_size"] = _config.min_size;
    config_network["max_size"] = _config.max_size;
    config_network["len_input"] = _len_input;
    config_network["len_output"] = _len_output;

    writeConfigStart("GasNet", config_network, stream);

    double gas1[_gene->segments().size()];
    double gas2[_gene->segments().size()];
    double k[_gene->segments().size()];

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate gas concentration
        if(_gas_emitting[i] > 0.0 && _gene->segments()[i][gene_TypeGas]%3 != 0)
        {
            double gas_radius = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
            for(qint32 j = 0; j < _gene->segments().size(); ++j)
            {
                if(_distances[i][j] > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                switch (_gene->segments()[i][gene_TypeGas]%3)
                {
                case 0:
                    // No Gas is emitted
                    break;

                case 1:
                    gas1[j] += gas_concentration;
                    break;

                case 2:
                    gas2[j] += gas_concentration;
                    break;

                default:
                    QNN_WARNING_MSG("Unknown gas");
                    break;
                }
            }
        }
    }

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        // Calculate k
        qint32 basis_index = _gene->segments()[i][gene_basis_index]%_P.length();
        qint32 index = qFloor(basis_index + gas1[i] * (_P.length() - basis_index) + gas2[i] * basis_index);
        if(index < 0)
        {
            index = 0;
        }
        else if(index >= _P.length())
        {
            index = _P.length()-1;
        }
        k[i] = _P[index];
    }

    for(qint32 i = 0; i < _gene->segments().size(); ++i)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connections_neuron;

        config_neuron["pos_x"] = floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size);
        config_neuron["pos_y"] = floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size);
        config_neuron["positiv_cone_radius"] = floatFromGeneInput(_gene->segments()[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio);
        config_neuron["positiv_cone_extension"] = floatFromGeneInput(_gene->segments()[i][gene_PositivConeExt], 2*M_PI);
        config_neuron["positiv_cone_orientation"] = floatFromGeneInput(_gene->segments()[i][gene_PositivConeOrientation], 2*M_PI);
        config_neuron["negativ_cone_radius"] = floatFromGeneInput(_gene->segments()[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio);
        config_neuron["negativ_cone_extension"] = floatFromGeneInput(_gene->segments()[i][gene_NegativConeExt], 2*M_PI);
        config_neuron["negativ_cone_orientation"] = floatFromGeneInput(_gene->segments()[i][gene_NegativConeOrientation], 2*M_PI);
        config_neuron["bias"] = weight(_gene->segments()[i][gene_bias], _config.bias_scalar);
        config_neuron["rate_of_gas"] = (_config.offset_rate_of_gas + floatFromGeneInput(_gene->segments()[i][gene_Rate_of_gas], _config.range_rate_of_gas));

        if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
        {
            config_neuron["input"] = _gene->segments()[i][gene_input]%(_len_input+1)-1;
        }

        config_neuron["gas_radius"] = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);

        switch (_gene->segments()[i][gene_TypeGas]%3)
        {
        case 0:
            config_neuron["gas_type"] = "No gas";
            break;

        case 1:
            config_neuron["gas_type"] = "Gas 1";
            break;

        case 2:
            config_neuron["gas_type"] = "Gas 2";
            break;

        default:
            QNN_WARNING_MSG("Unknown gas");
            break;
        }

        config_neuron["gas1_concentration"] = gas1[i];
        config_neuron["gas2_concentration"] = gas2[i];
        config_neuron["k_basis"] = _P[_gene->segments()[i][gene_basis_index]%_P.length()];
        config_neuron["k_modulated"] = k[i];

        switch (_gene->segments()[i][gene_WhenGas]%3)
        {
        case 0: // Electric charge
            config_neuron["when_gas_emitting"] = "electric charge";
            break;

        case 1: // Gas1
            config_neuron["when_gas_emitting"] = "gas1 concentration";
            break;

        case 2: // Gas2
            config_neuron["when_gas_emitting"] = "gas2 concentration";
            break;

        default:
            QNN_WARNING_MSG("Unknown gas circumstances");
            break;
        }

        for(qint32 j = 0; j < _gene->segments().size(); ++j)
        {
            if(_weights[j][i] != 0)
            {
                connections_neuron[j] = _weights[j][i];
            }
        }

        writeConfigNeuron(i, config_neuron, connections_neuron, stream);
    }
    writeConfigEnd(stream);
    return true;
}
