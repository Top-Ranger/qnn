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
#include <QDebug>
#include <QtCore/qmath.h>

#include <math.h>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius, basis index, bias
//                0  1   2   3      4    5    6     7     8     9             10     11               12        13         14        15

using CommonNetworkFunctions::floatFromGeneInput;
using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::calculate_distance;
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
        qFatal(QString("FATAL ERROR in %1 %2: Area needs to be greater then 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(Q_UNLIKELY(_config.min_size != -1 && _config.min_size < _len_output))
    {
        qFatal(QString("FATAL ERROR in %1 %2: min_size_network must not be smaller then output lenght!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(Q_UNLIKELY(_config.offset_rate_of_gas <= 0.0d))
    {
        qFatal(QString("FATAL ERROR in %1 %2: offset_rate_of_gas must be greater than 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    _P.append(-4.0d);
    _P.append(-2.0d);
    _P.append(-1.0d);
    _P.append(-0.5d);
    _P.append(-0.25d);
    _P.append(0.0d);
    _P.append(0.25d);
    _P.append(0.5d);
    _P.append(1.0d);
    _P.append(2.0d);
    _P.append(4.0d);
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
    _P.append(-4.0d);
    _P.append(-2.0d);
    _P.append(-1.0d);
    _P.append(-0.5d);
    _P.append(-0.25d);
    _P.append(0.0d);
    _P.append(0.25d);
    _P.append(0.5d);
    _P.append(1.0d);
    _P.append(2.0d);
    _P.append(4.0d);
}

GasNet::~GasNet()
{
    delete [] _network;
    delete [] _gas_emitting;
    if(_distances != NULL && _gene != NULL)
    {
        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            delete [] _distances[i];
        }
        delete [] _distances;
    }
    if(_weights != NULL && _gene != NULL)
    {
        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            delete [] _weights[i];
        }
        delete [] _weights;
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
            qCritical() << "CRITICAL ERROR in " << __FILE__ << __LINE__ << ": min_length is not smaller then max_length";
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
    return new GasNet(_len_input, _len_output, _config);
}

void GasNet::_initialise()
{
    QList< QList<qint32> > segments = _gene->segments();

    if(Q_UNLIKELY(segments.length() < _len_output))
    {
        qFatal(QString("FATAL ERROR in %1 %2: gene length must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(Q_UNLIKELY(segments[0].length() != 16))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene segment length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[segments.length()];
    _gas_emitting = new double[segments.length()];

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        _network[i] = 0;
        _gas_emitting[i] = 0;
    }


    // Cache distances and connection for faster calculation later
    _distances = new double*[segments.length()];
    _weights = new double*[segments.length()];

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        _distances[i] = new double[segments.length()];
        _weights[i] = new double[segments.length()];
        for(qint32 j = 0; j < segments.length(); ++j)
        {
            // distance
            _distances[i][j] = calculate_distance(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                                  floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                                  floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                                  floatFromGeneInput(segments[j][gene_y], _config.area_size));

            // weight
            _weights[i][j] = 0;
            if(i == j)
            {
                // recurrent connection
                switch(segments[i][gene_recurrent]%3)
                {
                case 1:
                    _weights[i][j] = 1.0d;
                    break;
                case 2:
                    _weights[i][j] = -1.0d;
                    break;
                default:
                    _weights[i][j] = 0.0d;
                    break;
                }
            }
            else
            {
                if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                     floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                     floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                     floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                     floatFromGeneInput(segments[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio),
                                     floatFromGeneInput(segments[i][gene_PositivConeExt], 2*M_PI),
                                     floatFromGeneInput(segments[i][gene_PositivConeOrientation], 2*M_PI)))
                {
                    _weights[i][j] += 1.0d;
                }
                if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                     floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                     floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                     floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                     floatFromGeneInput(segments[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio),
                                     floatFromGeneInput(segments[i][gene_NegativConeExt], 2*M_PI),
                                     floatFromGeneInput(segments[i][gene_NegativConeOrientation], 2*M_PI)))
                {
                    _weights[i][j] += -1.0d;
                }
            }
        }
    }
}

void GasNet::_processInput(QList<double> input)
{
    QList< QList<qint32> > segments = _gene->segments();

    double gas1[segments.length()];
    double gas2[segments.length()];
    double k[segments.length()];

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate gas concentration
        double gas_radius = _config.offset_gas_radius + floatFromGeneInput( segments[i][gene_Gas_radius], _config.range_gas_radius);
        if(_gas_emitting[i] > 0.0d && segments[i][gene_TypeGas]%3 != 0)
        {
            for(qint32 j = 0; j < segments.length(); ++j)
            {
                if(_distances[i][j] > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                switch (segments[i][gene_TypeGas]%3)
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
                    qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << segments[i][gene_TypeGas]%3 << "- ignoring";
                    break;
                }
            }
        }
    }

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate k
        qint32 basis_index = segments[i][gene_basis_index]%_P.length();
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

    double *newNetwork = new double[segments.length()];

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate new input
        double newValue = 0;

        // Connections
        for(qint32 j = 0; j < segments.length(); ++j)
        {
            newValue += _network[j] * _weights[j][i];
        }

        // Input
        if(segments[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[segments[i][gene_input]%(_len_input+1)-1];
        }

        // K
        newValue *= k[i];

        // Bias
        newValue += weight(segments[i][gene_bias], _config.bias_scalar);

        // tanh
        newNetwork[i] = tanh(newValue);
    }

    delete [] _network;
    _network = newNetwork;

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate emition of gas
        bool emittingGas = false;
        switch (segments[i][gene_WhenGas]%3)
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
            qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas circumstances" << segments[i][gene_WhenGas]%3 << "- ignoring";
            break;
        }

        if(emittingGas)
        {
            _gas_emitting[i] = cut01(_gas_emitting[i] + 1.0d / (_config.offset_rate_of_gas + floatFromGeneInput(segments[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
        }
        else
        {
            _gas_emitting[i] = cut01(_gas_emitting[i] - 1.0d / (_config.offset_rate_of_gas + floatFromGeneInput(segments[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
        }
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
        qCritical() << "CRITICAL ERROR in " __FILE__ << __LINE__ << ": i out of bound";
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

    QList< QList<qint32> > segments = _gene->segments();

    double gas1[segments.length()];
    double gas2[segments.length()];
    double k[segments.length()];

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate gas concentration
        double gas_radius = _config.offset_gas_radius + floatFromGeneInput( segments[i][gene_Gas_radius], _config.range_gas_radius);
        if(_gas_emitting[i] > 0.0d && segments[i][gene_TypeGas]%3 != 0)
        {
            for(qint32 j = 0; j < segments.length(); ++j)
            {
                if(_distances[i][j] > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                switch (segments[i][gene_TypeGas]%3)
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
                    qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << segments[i][gene_TypeGas]%3 << "- ignoring";
                    break;
                }
            }
        }
    }

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        // Calculate k
        qint32 basis_index = segments[i][gene_basis_index]%_P.length();
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

    for(qint32 i = 0; i < segments.length(); ++i)
    {
        QMap<QString, QVariant> config_neuron;
        QMap<qint32, double> connections_neuron;

        config_neuron["pos_x"] = floatFromGeneInput(segments[i][gene_x], _config.area_size);
        config_neuron["pos_y"] = floatFromGeneInput(segments[i][gene_y], _config.area_size);
        config_neuron["positiv_cone_radius"] = floatFromGeneInput(segments[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio);
        config_neuron["positiv_cone_extension"] = floatFromGeneInput(segments[i][gene_PositivConeExt], 2*M_PI);
        config_neuron["positiv_cone_orientation"] = floatFromGeneInput(segments[i][gene_PositivConeOrientation], 2*M_PI);
        config_neuron["negativ_cone_radius"] = floatFromGeneInput(segments[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio);
        config_neuron["negativ_cone_extension"] = floatFromGeneInput(segments[i][gene_NegativConeExt], 2*M_PI);
        config_neuron["negativ_cone_orientation"] = floatFromGeneInput(segments[i][gene_NegativConeOrientation], 2*M_PI);
        config_neuron["bias"] = weight(segments[i][gene_bias], _config.bias_scalar);
        config_neuron["rate_of_gas"] = (_config.offset_rate_of_gas + floatFromGeneInput(segments[i][gene_Rate_of_gas], _config.range_rate_of_gas));

        if(segments[i][gene_input]%(_len_input+1) != 0)
        {
            config_neuron["input"] = segments[i][gene_input]%(_len_input+1)-1;
        }

        config_neuron["gas_radius"] = _config.offset_gas_radius + floatFromGeneInput( segments[i][gene_Gas_radius], _config.range_gas_radius);

        switch (segments[i][gene_TypeGas]%3)
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
            qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << segments[i][gene_TypeGas]%3 << "- ignoring";
            break;
        }

        config_neuron["gas1_concentration"] = gas1[i];
        config_neuron["gas2_concentration"] = gas2[i];
        config_neuron["k_basis"] = _P[segments[i][gene_basis_index]%_P.length()];
        config_neuron["k_modulated"] = k[i];

        switch (segments[i][gene_WhenGas]%3)
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
            qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas circumstances" << segments[i][gene_WhenGas]%3 << "- ignoring";
            break;
        }

        for(qint32 j = 0; j < segments.length(); ++j)
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
