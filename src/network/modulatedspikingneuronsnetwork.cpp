#include "modulatedspikingneuronsnetwork.h"

#include "lengthchanginggene.h"
#include "commonnetworkfunctions.h"

#include <QString>
#include <QDebug>
#include <QtCore/qmath.h>

#include <math.h>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius,   a,   b,   c,   d
//                0  1   2   3      4    5    6     7     8     9             10     11               12        13       14   15   16   17

using CommonNetworkFunctions::floatFromGeneInput;
using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::calculate_distance;
using CommonNetworkFunctions::areNodesConnected;
using CommonNetworkFunctions::cut01;

namespace {
double getModulatedValue(bool modulation_applied, double gasPos, double gasNeg, int basis_index, QList<double> &array)
{
    if(modulation_applied)
    {
        int index = qFloor(basis_index + gasPos * (array.length() - basis_index) + gasNeg * basis_index);
        if(index < 0)
        {
            index = 0;
        }
        else if(index >= array.length())
        {
            index = array.length()-1;
        }
        return array[index];
    }
    else
    {
        return array[basis_index];
    }
}

}


ModulatedSpikingNeuronsNetwork::ModulatedSpikingNeuronsNetwork(int len_input, int len_output, config config) :
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
    _Pd()
{
    if(_config.area_size <= 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Area needs to be greater then 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.min_size != -1 && _config.min_size < _len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: min_size_network must not be smaller then output lenght!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.offset_rate_of_gas <= 0.0d)
    {
        qFatal(QString("FATAL ERROR in %1 %2: offset_rate_of_gas must be greater than 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    initialiseP();
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
    _Pd()
{
}

ModulatedSpikingNeuronsNetwork::~ModulatedSpikingNeuronsNetwork()
{
    if(_network != NULL)
    {
        delete [] _network;
    }
    if(_gas_emitting != NULL)
    {
        delete [] _gas_emitting;
    }
    if(_u != NULL)
    {
        delete [] _u;
    }
    if(_firecount != NULL)
    {
        delete [] _firecount;
    }
    if(_distances != NULL && _gene != NULL)
    {
        for(int i = 0; i < _gene->segments().length(); ++i)
        {
            delete [] _distances[i];
        }
        delete [] _distances;
    }
    if(_weights != NULL && _gene != NULL)
    {
        for(int i = 0; i < _gene->segments().length(); ++i)
        {
            delete [] _weights[i];
        }
        delete [] _distances;
    }
}

void ModulatedSpikingNeuronsNetwork::initialiseP()
{
    _Pa.clear();
    _Pb.clear();
    _Pc.clear();
    _Pd.clear();

    _Pa.append(0.0d);
    _Pa.append(0.02d);
    _Pa.append(0.04d);
    _Pa.append(0.045d);
    _Pa.append(0.0475d);
    _Pa.append(0.04875d);
    _Pa.append(0.05d);
    _Pa.append(0.05125d);
    _Pa.append(0.0525d);
    _Pa.append(0.055d);
    _Pa.append(0.06d);
    _Pa.append(0.08d);
    _Pa.append(0.1d);

    _Pb.append(-0.2d);
    _Pb.append(0.0d);
    _Pb.append(0.1d);
    _Pb.append(0.15d);
    _Pb.append(0.175d);
    _Pb.append(0.1875d);
    _Pb.append(0.2d);
    _Pb.append(0.2125d);
    _Pb.append(0.225d);
    _Pb.append(0.25d);
    _Pb.append(0.3d);
    _Pb.append(0.4d);
    _Pb.append(0.6d);

    _Pc.append(-80.0d);
    _Pc.append(-72.0d);
    _Pc.append(-68.0d);
    _Pc.append(-66.0d);
    _Pc.append(-65.5d);
    _Pc.append(-65.25d);
    _Pc.append(-65.0d);
    _Pc.append(-64.75d);
    _Pc.append(-64.5d);
    _Pc.append(-64.0d);
    _Pc.append(-62.0d);
    _Pc.append(-58.0d);
    _Pc.append(-50.0d);

    _Pd.append(-2.0d);
    _Pd.append(0.0d);
    _Pd.append(1.0d);
    _Pd.append(1.5d);
    _Pd.append(1.75d);
    _Pd.append(1.875d);
    _Pd.append(2.0d);
    _Pd.append(2.125d);
    _Pd.append(2.25d);
    _Pd.append(2.5d);
    _Pd.append(3.0d);
    _Pd.append(4.0d);
    _Pd.append(6.0d);
}

GenericGene *ModulatedSpikingNeuronsNetwork::getRandomGene()
{
    LengthChangingGene::config config;
    config.min_length = _config.min_size;
    config.max_length = _config.max_size;

    int initial_length;

    if(config.min_length == -1 || config.max_length == -1)
    {
        initial_length = _len_output;
    }
    else
    {
        if(config.min_length > config.max_length)
        {
            qCritical() << "CRITICAL ERROR in " << __FILE__ << " " << __LINE__ << ": min_length is not smaller then max_length";
            initial_length = _len_output;
        }
        else
        {
            int diff = config.max_length - config.min_length;
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
    QList< QList<int> > segments = _gene->segments();

    if(segments.length() < _len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: gene length must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(segments[0].length() != 18)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene segment length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[segments.length()];
    _gas_emitting = new double[segments.length()];
    _u = new double[segments.length()];
    _firecount = new double[segments.length()];

    for(int i = 0; i < segments.length(); ++i)
    {
        _network[i] = 0;
        _gas_emitting[i] = 0;
        _u[i] = 0;
        _firecount[i] = 0;
    }

    // Cache distances and connection for faster calculation later
    _distances = new double*[segments.length()];
    _weights = new double*[segments.length()];

    for(int i = 0; i < segments.length(); ++i)
    {
        _distances[i] = new double[segments.length()];
        _weights[i] = new double[segments.length()];
        for(int j = 0; j < segments.length(); ++j)
        {
            // distance
            _distances[i][j] = calculate_distance(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                                  floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                                  floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                                  floatFromGeneInput(segments[j][gene_y], _config.area_size));

            // weight
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
            else if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                      floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                      floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                      floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                      floatFromGeneInput(segments[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio),
                                      floatFromGeneInput(segments[i][gene_PositivConeExt], 2*M_PI),
                                      floatFromGeneInput(segments[i][gene_PositivConeOrientation], 2*M_PI)))
            {
                _weights[i][j] = 1.0d;
            }
            else if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                      floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                      floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                      floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                      floatFromGeneInput(segments[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio),
                                      floatFromGeneInput(segments[i][gene_NegativConeExt], 2*M_PI),
                                      floatFromGeneInput(segments[i][gene_NegativConeOrientation], 2*M_PI)))
            {
                _weights[i][j] = -1.0d;
            }
            else
            {
                _weights[i][j] = 0.0d;
            }
        }
    }
}

void ModulatedSpikingNeuronsNetwork::_processInput(QList<double> input)
{
    QList< QList<int> > segments = _gene->segments();

    // Clear fire count
    for(int i = 0; i < segments.length(); ++i)
    {
        _firecount[i] = 0;
    }

    for(int timesteps = 0; timesteps < 1.0d / _config.timestep_size; ++timesteps)
    {
        double *newNetwork = new double[segments.length()];
        double *newU = new double[segments.length()];

        double gasAPos[segments.length()];
        double gasANeg[segments.length()];
        double gasBPos[segments.length()];
        double gasBNeg[segments.length()];
        double gasCPos[segments.length()];
        double gasCNeg[segments.length()];
        double gasDPos[segments.length()];
        double gasDNeg[segments.length()];

        double a;
        double b;
        double c[segments.length()]; // This both parameter have to be cached
        double d[segments.length()];

        for(int i = 0; i < segments.length(); ++i)
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

        for(int i = 0; i < segments.length(); ++i)
        {
            // Calculate gas concentration
            double gas_radius = _config.offset_gas_radius + floatFromGeneInput( segments[i][gene_Gas_radius], _config.range_gas_radius);
            if(_gas_emitting[i] > 0.0d && segments[i][gene_TypeGas]%3 != 0)
            {
                for(int j = 0; j < segments.length(); ++j)
                {
                    if(_distances[i][j] > gas_radius)
                    {
                        continue;
                    }
                    double gas_concentration = qExp((-2 * _distances[i][j])/gas_radius) * _gas_emitting[i];
                    switch (segments[i][gene_TypeGas]%9)
                    {
                    case 0:
                        // No Gas is emitted
                        break;

                    case 1:
                        if(_config.a_modulated)
                        {
                            gasAPos[j] += gas_concentration;
                        }
                        break;

                    case 2:
                        if(_config.a_modulated)
                        {
                            gasANeg[j] += gas_concentration;
                        }
                        break;

                    case 3:
                        if(_config.b_modulated)
                        {
                            gasBPos[j] += gas_concentration;
                        }
                        break;

                    case 4:
                        if(_config.b_modulated)
                        {
                            gasBNeg[j] += gas_concentration;
                        }
                        break;

                    case 5:
                        if(_config.c_modulated)
                        {
                            gasCPos[j] += gas_concentration;
                        }
                        break;

                    case 6:
                        if(_config.c_modulated)
                        {
                            gasCNeg[j] += gas_concentration;
                        }
                        break;

                    case 7:
                        if(_config.d_modulated)
                        {
                            gasDPos[j] += gas_concentration;
                        }
                        break;

                    case 8:
                        if(_config.d_modulated)
                        {
                            gasDNeg[j] += gas_concentration;
                        }
                        break;

                    default:
                        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas" << segments[i][gene_TypeGas]%3 << "- ignoring";
                        break;
                    }
                }
            }
        }

        for(int i = 0; i < segments.length(); ++i)
        {
            // Calculate a,b,c,d
            a = getModulatedValue(_config.a_modulated, gasAPos[i], gasANeg[i], segments[i][gene_a]%_Pa.length(), _Pa);
            b = getModulatedValue(_config.b_modulated, gasBPos[i], gasBNeg[i], segments[i][gene_b]%_Pb.length(), _Pb);
            c[i] = getModulatedValue(_config.c_modulated, gasCPos[i], gasCNeg[i], segments[i][gene_c]%_Pc.length(), _Pc);
            d[i] = getModulatedValue(_config.d_modulated, gasDPos[i], gasDNeg[i], segments[i][gene_d]%_Pd.length(), _Pd);


            // Calculate new input

            double newValue = 0;

            // Connections
            for(int j = 0; j < segments.length(); ++j)
            {
                newValue += _network[j] * _weights[i][j];
            }

            // Input
            if(segments[i][gene_input]%(_len_input+1) != 0)
            {
                newValue += input[segments[i][gene_input]%(_len_input+1)-1];
            }

            // Calculate potential
            newNetwork[i] = 0.04d * _network[i] * _network[i] + 5.0d * _network[i] + 140.0d - _u[i] + newValue;

            // Calculate U
            newU[i] = a * (b * _network[i] - _u[i]);
        }

        delete [] _network;
        _network = newNetwork;
        delete [] _u;
        _u = newU;

        for(int i = 0; i < segments.length(); ++i)
        {
            // Calculate emition of gas
            bool emittingGas = false;
            switch (segments[i][gene_WhenGas]%9)
            {
            case 0: // Electric charge
                if(_network[i] > _config.electric_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 1:
                if(gasAPos[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 2:
                if(gasANeg[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 3:
                if(gasBPos[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 4:
                if(gasBNeg[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 5:
                if(gasCPos[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 6:
                if(gasCNeg[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 7:
                if(gasDPos[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            case 8:
                if(gasDNeg[i] > _config.gas_threshhold)
                {
                    emittingGas = true;
                }
                break;

            default:
                qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas circumstances" << segments[i][gene_WhenGas]%3 << "- ignoring";
                break;
            }

            if(emittingGas)
            {
                _gas_emitting[i] = cut01((_gas_emitting[i] + 1.0d) / (_config.offset_rate_of_gas + floatFromGeneInput(segments[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
            }
            else
            {
                _gas_emitting[i] = cut01((_gas_emitting[i] - 1.0d) / (_config.offset_rate_of_gas + floatFromGeneInput(segments[i][gene_Rate_of_gas], _config.range_rate_of_gas)));
            }
        }

        for(int i = 0; i < segments.length(); ++i)
        {
            // Check if fired
            if(_network[i] >= 30.0d)
            {
                _network[i] = c[i];
                _u[i] = _u[i] + d[i];
                ++_firecount[i];
            }
        }
    }
}

double ModulatedSpikingNeuronsNetwork::_getNeuronOutput(int i)
{
    if(i >= 0 && i < _len_output)
    {
        return _firecount[i] * _config.timestep_size;
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0;
    }
}

