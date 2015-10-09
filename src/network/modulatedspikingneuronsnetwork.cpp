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

#include "modulatedspikingneuronsnetwork.h"

#include "lengthchanginggene.h"
#include "commonnetworkfunctions.h"
#include "networktoxml.h"

#include <QString>
#include <QDebug>
#include <QtCore/qmath.h>

#include <math.h>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius,   a,   b,   c,   d
//                0  1   2   3      4    5    6     7     8     9             10     11               12        13       14   15   16   17

using CommonNetworkFunctions::floatFromGeneInput;
using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::calculateDistance;
using CommonNetworkFunctions::areNodesConnected;
using CommonNetworkFunctions::cut01;

using NetworkToXML::writeConfigStart;
using NetworkToXML::writeConfigNeuron;
using NetworkToXML::writeConfigEnd;

namespace {
double getModulatedValue(bool modulation_applied, double gasPos, double gasNeg, qint32 basis_index, QVector<double> &array)
{
    if(modulation_applied)
    {
        qint32 index = qFloor(basis_index + gasPos * (array.size() - basis_index) + gasNeg * basis_index);
        if(index < 0)
        {
            index = 0;
        }
        else if(index >= array.size())
        {
            index = array.size()-1;
        }
        return array[index];
    }
    else
    {
        return array[basis_index];
    }
}
}


ModulatedSpikingNeuronsNetwork::ModulatedSpikingNeuronsNetwork(qint32 len_input, qint32 len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _network(NULL),
    _gas_emitting(NULL),
    _u(NULL),
    _firecount(NULL),
    _distances(NULL),
    _weights(NULL),
    _Pa(),
    _Pb(),
    _Pc(),
    _Pd(),
    _WhenGas_list(),
    _TypeGas_list(),
    _emitting_possible(false)
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

    initialiseP();
    initialiseTokenArrays();
}

ModulatedSpikingNeuronsNetwork::ModulatedSpikingNeuronsNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _network(NULL),
    _gas_emitting(NULL),
    _u(NULL),
    _firecount(NULL),
    _distances(NULL),
    _weights(NULL),
    _Pa(),
    _Pb(),
    _Pc(),
    _Pd(),
    _WhenGas_list(),
    _TypeGas_list(),
    _emitting_possible(false)
{
}

ModulatedSpikingNeuronsNetwork::~ModulatedSpikingNeuronsNetwork()
{
    delete [] _network;
    delete [] _gas_emitting;
    delete [] _u;
    delete [] _firecount;

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

void ModulatedSpikingNeuronsNetwork::initialiseP()
{
    _Pa.clear();
    _Pb.clear();
    _Pc.clear();
    _Pd.clear();

    _Pa.reserve(13);
    _Pa.append(0.0);
    _Pa.append(0.02);
    _Pa.append(0.04);
    _Pa.append(0.045);
    _Pa.append(0.0475);
    _Pa.append(0.04875);
    _Pa.append(0.05);
    _Pa.append(0.05125);
    _Pa.append(0.0525);
    _Pa.append(0.055);
    _Pa.append(0.06);
    _Pa.append(0.08);
    _Pa.append(0.1);

    _Pb.reserve(13);
    _Pb.append(-0.2);
    _Pb.append(0.0);
    _Pb.append(0.1);
    _Pb.append(0.15);
    _Pb.append(0.175);
    _Pb.append(0.1875);
    _Pb.append(0.2);
    _Pb.append(0.2125);
    _Pb.append(0.225);
    _Pb.append(0.25);
    _Pb.append(0.3);
    _Pb.append(0.4);
    _Pb.append(0.6);

    _Pc.reserve(13);
    _Pc.append(-80.0);
    _Pc.append(-72.0);
    _Pc.append(-68.0);
    _Pc.append(-66.0);
    _Pc.append(-65.5);
    _Pc.append(-65.25);
    _Pc.append(-65.0);
    _Pc.append(-64.75);
    _Pc.append(-64.5);
    _Pc.append(-64.0);
    _Pc.append(-62.0);
    _Pc.append(-58.0);
    _Pc.append(-50.0);

    _Pd.reserve(13);
    _Pd.append(-2.0);
    _Pd.append(0.0);
    _Pd.append(1.0);
    _Pd.append(1.5);
    _Pd.append(1.75);
    _Pd.append(1.875);
    _Pd.append(2.0);
    _Pd.append(2.125);
    _Pd.append(2.25);
    _Pd.append(2.5);
    _Pd.append(3.0);
    _Pd.append(4.0);
    _Pd.append(6.0);
}

void ModulatedSpikingNeuronsNetwork::initialiseTokenArrays()
{
    _WhenGas_list.clear();
    _WhenGas_list.reserve(9);
    _TypeGas_list.clear();
    _TypeGas_list.reserve(9);
    _emitting_possible = false;

    _WhenGas_list.append(ElectricCharge);
    _TypeGas_list.append(NoGas);
    if(_config.a_modulated)
    {
        _emitting_possible = true;
        _WhenGas_list.append(APositiv);
        _WhenGas_list.append(ANegativ);
        _TypeGas_list.append(APositiv);
        _TypeGas_list.append(ANegativ);
    }
    if(_config.b_modulated)
    {
        _emitting_possible = true;
        _WhenGas_list.append(BPositiv);
        _WhenGas_list.append(BNegativ);
        _TypeGas_list.append(BPositiv);
        _TypeGas_list.append(BNegativ);
    }
    if(_config.c_modulated)
    {
        _emitting_possible = true;
        _WhenGas_list.append(CPositiv);
        _WhenGas_list.append(CNegativ);
        _TypeGas_list.append(CPositiv);
        _TypeGas_list.append(CNegativ);
    }
    if(_config.d_modulated)
    {
        _emitting_possible = true;
        _WhenGas_list.append(DPositiv);
        _WhenGas_list.append(DNegativ);
        _TypeGas_list.append(DPositiv);
        _TypeGas_list.append(DNegativ);
    }
}

GenericGene *ModulatedSpikingNeuronsNetwork::getRandomGene()
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

    return new LengthChangingGene(initial_length, 18, config);
}

AbstractNeuralNetwork *ModulatedSpikingNeuronsNetwork::createConfigCopy()
{
    return new ModulatedSpikingNeuronsNetwork(_len_input, _len_output, _config);
}

void ModulatedSpikingNeuronsNetwork::_initialise()
{
    if(Q_UNLIKELY(_gene->segments().length() < _len_output))
    {
        qFatal(QString("FATAL ERROR in %1 %2: gene length must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(Q_UNLIKELY(_gene->segments()[0].length() != 18))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene segment length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[_gene->segments().length()];
    _gas_emitting = new double[_gene->segments().length()];
    _u = new double[_gene->segments().length()];
    _firecount = new double[_gene->segments().length()];

    for(qint32 i = 0; i < _gene->segments().length(); ++i)
    {
        _network[i] = 0;
        _gas_emitting[i] = 0;
        _u[i] = 0;
        _firecount[i] = 0;
    }

    // Cache distances and connection for faster calculation later
    _distances = new double*[_gene->segments().length()];
    _weights = new double*[_gene->segments().length()];

    for(qint32 i = 0; i < _gene->segments().length(); ++i)
    {
        _distances[i] = new double[_gene->segments().length()];
        _weights[i] = new double[_gene->segments().length()];
        for(qint32 j = 0; j < _gene->segments().length(); ++j)
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
}

void ModulatedSpikingNeuronsNetwork::_processInput(QList<double> input)
{
    // Clear fire count
    for(qint32 i = 0; i < _gene->segments().length(); ++i)
    {
        _firecount[i] = 0;
    }

    for(qint32 timesteps = 0; timesteps < 1.0 / _config.timestep_size; ++timesteps)
    {
        double *newNetwork = new double[_gene->segments().length()];
        double *newU = new double[_gene->segments().length()];

        double gasAPos[_gene->segments().length()];
        double gasANeg[_gene->segments().length()];
        double gasBPos[_gene->segments().length()];
        double gasBNeg[_gene->segments().length()];
        double gasCPos[_gene->segments().length()];
        double gasCNeg[_gene->segments().length()];
        double gasDPos[_gene->segments().length()];
        double gasDNeg[_gene->segments().length()];

        double a;
        double b;
        double c[_gene->segments().length()]; // This both parameter have to be cached
        double d[_gene->segments().length()];

        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            // Initiation
            gasAPos[i] = 0;
            gasANeg[i] = 0;
            gasBPos[i] = 0;
            gasBNeg[i] = 0;
            gasCPos[i] = 0;
            gasCNeg[i] = 0;
            gasDPos[i] = 0;
            gasDNeg[i] = 0;
        }

        if(_emitting_possible)
        {
            for(qint32 i = 0; i < _gene->segments().length(); ++i)
            {
                // Calculate gas concentration
                if(_gas_emitting[i] > 0.0 && _TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()] != NoGas)
                {
                    double gas_radius = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
                    for(qint32 j = 0; j < _gene->segments().length(); ++j)
                    {
                        if(_distances[i][j] > gas_radius)
                        {
                            continue;
                        }
                        double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                        switch(_TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()])
                        {
                        case NoGas:
                            // No Gas is emitted
                            break;

                        case APositiv:
                            gasAPos[j] += gas_concentration;
                            break;

                        case ANegativ:
                            gasANeg[j] += gas_concentration;
                            break;

                        case BPositiv:
                            gasBPos[j] += gas_concentration;
                            break;

                        case BNegativ:
                            gasBNeg[j] += gas_concentration;
                            break;

                        case CPositiv:
                            gasCPos[j] += gas_concentration;
                            break;

                        case CNegativ:
                            gasCNeg[j] += gas_concentration;
                            break;

                        case DPositiv:
                            gasDPos[j] += gas_concentration;
                            break;

                        case DNegativ:
                            gasDNeg[j] += gas_concentration;
                            break;

                        default:
                            qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << _TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()] << "- ignoring";
                            break;
                        }
                    }
                }
            }
        }

        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            // Calculate a,b,c,d
            a = getModulatedValue(_config.a_modulated, gasAPos[i], gasANeg[i], _gene->segments()[i][gene_a]%_Pa.size(), _Pa);
            b = getModulatedValue(_config.b_modulated, gasBPos[i], gasBNeg[i], _gene->segments()[i][gene_b]%_Pb.size(), _Pb);
            c[i] = getModulatedValue(_config.c_modulated, gasCPos[i], gasCNeg[i], _gene->segments()[i][gene_c]%_Pc.size(), _Pc);
            d[i] = getModulatedValue(_config.d_modulated, gasDPos[i], gasDNeg[i], _gene->segments()[i][gene_d]%_Pd.size(), _Pd);


            // Calculate new input

            double newValue = 0;

            // Connections
            for(qint32 j = 0; j < _gene->segments().length(); ++j)
            {
                newValue += _network[j] * _weights[j][i];
            }

            // Input
            if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
            {
                newValue += input[_gene->segments()[i][gene_input]%(_len_input+1)-1];
            }

            // Calculate potential
            newNetwork[i] = _network[i] + (0.04 * _network[i] * _network[i] + 5.0 * _network[i] + 140.0 - _u[i] + newValue);

            // Calculate U
            newU[i] = _u[i] + (a * (b * _network[i] - _u[i]));
        }

        delete [] _network;
        _network = newNetwork;
        delete [] _u;
        _u = newU;

        if(_emitting_possible)
        {
            for(qint32 i = 0; i < _gene->segments().length(); ++i)
            {
                // Calculate emition of gas
                bool emittingGas = false;
                switch(_WhenGas_list[_gene->segments()[i][gene_WhenGas]%_WhenGas_list.size()])
                {
                case ElectricCharge:
                    if(_network[i] > _config.electric_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case APositiv:
                    if(gasAPos[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case ANegativ:
                    if(gasANeg[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case BPositiv:
                    if(gasBPos[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case BNegativ:
                    if(gasBNeg[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case CPositiv:
                    if(gasCPos[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case CNegativ:
                    if(gasCNeg[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case DPositiv:
                    if(gasDPos[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                case DNegativ:
                    if(gasDNeg[i] > _config.gas_threshhold)
                    {
                        emittingGas = true;
                    }
                    break;

                default:
                    qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas circumstances" << _WhenGas_list[_gene->segments()[i][gene_WhenGas]%_WhenGas_list.size()] << "- ignoring";
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
        }

        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            // Check if fired
            if(_network[i] >= 30.0)
            {
                _network[i] = c[i];
                _u[i] = _u[i] + d[i];
                ++_firecount[i];
            }
        }
    }
}

double ModulatedSpikingNeuronsNetwork::_getNeuronOutput(qint32 i)
{
    if(Q_LIKELY(i >= 0 && i < _len_output))
    {
        return _firecount[i] * _config.timestep_size;
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << __LINE__ << ": i out of bound";
        return -1.0;
    }
}

bool ModulatedSpikingNeuronsNetwork::_saveNetworkConfig(QXmlStreamWriter *stream)
{
    QMap<QString, QVariant> network_config;

    network_config["area_size"] = _config.area_size;
    network_config["bias_scalar"] = _config.bias_scalar;
    network_config["gas_threshhold"] = _config.gas_threshhold;
    network_config["electric_threshhold"] = _config.electric_threshhold;
    network_config["cone_ratio"] = _config.cone_ratio;
    network_config["offset_gas_radius"] = _config.offset_gas_radius;
    network_config["range_gas_radius"] = _config.range_gas_radius;
    network_config["offset_rate_of_gas"] = _config.offset_rate_of_gas;
    network_config["range_rate_of_gas"] = _config.range_rate_of_gas;
    network_config["min_size"] = _config.min_size;
    network_config["max_size"] = _config.max_size;
    network_config["a_modulated"] = _config.a_modulated;
    network_config["b_modulated"] = _config.b_modulated;
    network_config["c_modulated"] = _config.c_modulated;
    network_config["d_modulated"] = _config.d_modulated;
    network_config["timestep_size"] = _config.timestep_size;

    writeConfigStart("ModulatedSpikingNeuronsNetwork", network_config, stream);

    // Gas concentration

    double gasAPos[_gene->segments().length()];
    double gasANeg[_gene->segments().length()];
    double gasBPos[_gene->segments().length()];
    double gasBNeg[_gene->segments().length()];
    double gasCPos[_gene->segments().length()];
    double gasCNeg[_gene->segments().length()];
    double gasDPos[_gene->segments().length()];
    double gasDNeg[_gene->segments().length()];

    for(qint32 i = 0; i < _gene->segments().length(); ++i)
    {
        // Initiation
        gasAPos[i] = 0;
        gasANeg[i] = 0;
        gasBPos[i] = 0;
        gasBNeg[i] = 0;
        gasCPos[i] = 0;
        gasCNeg[i] = 0;
        gasDPos[i] = 0;
        gasDNeg[i] = 0;
    }

    if(_emitting_possible)
    {
        for(qint32 i = 0; i < _gene->segments().length(); ++i)
        {
            // Calculate gas concentration
            if(_gas_emitting[i] > 0.0 && _TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()] != NoGas)
            {
                double gas_radius = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
                for(qint32 j = 0; j < _gene->segments().length(); ++j)
                {
                    if(_distances[i][j] > gas_radius)
                    {
                        continue;
                    }
                    double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                    switch(_TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()])
                    {
                    case NoGas:
                        // No Gas is emitted
                        break;

                    case APositiv:
                        gasAPos[j] += gas_concentration;
                        break;

                    case ANegativ:
                        gasANeg[j] += gas_concentration;
                        break;

                    case BPositiv:
                        gasBPos[j] += gas_concentration;
                        break;

                    case BNegativ:
                        gasBNeg[j] += gas_concentration;
                        break;

                    case CPositiv:
                        gasCPos[j] += gas_concentration;
                        break;

                    case CNegativ:
                        gasCNeg[j] += gas_concentration;
                        break;

                    case DPositiv:
                        gasDPos[j] += gas_concentration;
                        break;

                    case DNegativ:
                        gasDNeg[j] += gas_concentration;
                        break;

                    default:
                        qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << _TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()] << "- ignoring";
                        break;
                    }
                }
            }
        }
    }

    // Write neuron config

    for(qint32 i = 0; i < _gene->segments().length(); ++i)
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
        config_neuron["gasAPos_concentration"] = gasAPos[i];
        config_neuron["gasBPos_concentration"] = gasBPos[i];
        config_neuron["gasCPos_concentration"] = gasCPos[i];
        config_neuron["gasDPos_concentration"] = gasDPos[i];
        config_neuron["gasANeg_concentration"] = gasANeg[i];
        config_neuron["gasBNeg_concentration"] = gasBNeg[i];
        config_neuron["gasCNeg_concentration"] = gasCNeg[i];
        config_neuron["gasDNeg_concentration"] = gasDNeg[i];


        if(_emitting_possible)
        {
            switch(_WhenGas_list[_gene->segments()[i][gene_WhenGas]%_WhenGas_list.size()])
            {
            case ElectricCharge:
                config_neuron["when_gas_emitting"] = "electric charge";
                break;

            case APositiv:
                config_neuron["when_gas_emitting"] = "gasAPositiv concentration";
                break;

            case ANegativ:
                config_neuron["when_gas_emitting"] = "gasANegativ concentration";
                break;

            case BPositiv:
                config_neuron["when_gas_emitting"] = "gasBPositiv concentration";
                break;

            case BNegativ:
                config_neuron["when_gas_emitting"] = "gasBNegativ concentration";
                break;

            case CPositiv:
                config_neuron["when_gas_emitting"] = "gasCPositiv concentration";
                break;

            case CNegativ:
                config_neuron["when_gas_emitting"] = "gasCNegativ concentration";
                break;

            case DPositiv:
                config_neuron["when_gas_emitting"] = "gasDPositiv concentration";
                break;

            case DNegativ:
                config_neuron["when_gas_emitting"] = "gasDNegativ concentration";
                break;

            default:
                qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas circumstances" << _WhenGas_list[_gene->segments()[i][gene_WhenGas]%_WhenGas_list.size()] << "- ignoring";
                break;
            }

            switch(_TypeGas_list[_gene->segments()[i][gene_TypeGas]%_TypeGas_list.size()])
            {
            case NoGas:
                config_neuron["gas_type"] = "No gas";
                break;

            case APositiv:
                config_neuron["gas_type"] = "gasAPositiv";
                break;

            case ANegativ:
                config_neuron["gas_type"] = "gasANegativ";
                break;

            case BPositiv:
                config_neuron["gas_type"] = "gasBPositiv";
                break;

            case BNegativ:
                config_neuron["gas_type"] = "gasBNegativ";
                break;

            case CPositiv:
                config_neuron["gas_type"] = "gasCPositiv";
                break;

            case CNegativ:
                config_neuron["gas_type"] = "gasCNegativ";
                break;

            case DPositiv:
                config_neuron["gas_type"] = "gasDPositiv";
                break;

            case DNegativ:
                config_neuron["gas_type"] = "gasDNegativ";
                break;

            default:
                qWarning() << "WARNING in " __FILE__ << __LINE__ << ": Unknown gas" << _gene->segments()[i][gene_TypeGas]%3 << "- ignoring";
                break;
            }
        }
        else
        {
            config_neuron["gas_type"] = "No gas";
            config_neuron["when_gas_emitting"] = "Not emitting";
        }

        config_neuron["rate_of_gas"] = (_config.offset_rate_of_gas + floatFromGeneInput(_gene->segments()[i][gene_Rate_of_gas], _config.range_rate_of_gas));
        config_neuron["gas_radius"] = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
        config_neuron["a_basis"] = _Pa[_gene->segments()[i][gene_a]%_Pa.size()];
        config_neuron["b_basis"] = _Pb[_gene->segments()[i][gene_b]%_Pb.size()];
        config_neuron["c_basis"] = _Pc[_gene->segments()[i][gene_c]%_Pc.size()];
        config_neuron["d_basis"] = _Pd[_gene->segments()[i][gene_d]%_Pd.size()];
        config_neuron["a_modulated"] = getModulatedValue(_config.a_modulated, gasAPos[i], gasANeg[i], _gene->segments()[i][gene_a]%_Pa.size(), _Pa);
        config_neuron["b_modulated"] = getModulatedValue(_config.b_modulated, gasBPos[i], gasBNeg[i], _gene->segments()[i][gene_b]%_Pb.size(), _Pb);
        config_neuron["c_modulated"] = getModulatedValue(_config.c_modulated, gasCPos[i], gasCNeg[i], _gene->segments()[i][gene_c]%_Pc.size(), _Pc);
        config_neuron["d_modulated"] = getModulatedValue(_config.d_modulated, gasDPos[i], gasDNeg[i], _gene->segments()[i][gene_d]%_Pd.size(), _Pd);
        config_neuron["qint32ernal_charge"] = _network[i];
        config_neuron["fire_output"] = _firecount[i] * _config.timestep_size;

        for(qint32 j = 0; j < _gene->segments().length(); ++j)
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
