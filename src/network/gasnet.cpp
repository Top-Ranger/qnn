#include "gasnet.h"

#include <QtCore/qmath.h>
#include <QString>
#include <QDebug>
#include <math.h>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius, basis index, bias
//                0  1   2   3      4    5    6     7     8     9             10     12               13        14         15        16

namespace {
bool calculate_distance(double x_source, double y_source, double x_target, double y_target)
{
    double difference_x = x_source - x_target;
    double difference_y = y_source - y_target;
    return qSqrt((difference_x * difference_x) + (difference_y * difference_y));
}

bool areNodesConnected(double x_source, double y_source, double x_target, double y_target, double radius, double angularExtend, double orientation)
{
    if(x_source == x_target && y_source == y_target)
    {
        return false;
    }

    double difference_x = x_source - x_target;
    double difference_y = y_source - y_target;
    double distance = calculate_distance(x_source, y_source, x_target, y_target);

    if(distance > radius)
    {
        return false;
    }

    double angleCone = qAsin(difference_x / distance);

    if(difference_y < 0.0d)
    {
        angleCone = M_PI - angleCone;
    }
    else if (angleCone < 0.0d)
    {
        angleCone = 2.0d * M_PI + angleCone;
    }

    angleCone -= angularExtend;

    if (angleCone < 0.0d)
    {
        angleCone = 2.0d * M_PI + angleCone;
    }

    return angleCone < orientation;
}

double floatFromGeneInput(int gene_input, double scalar)
{
    return (double) gene_input/RAND_MAX * scalar;
}

double weight(int gene_input)
{
    return (((double) gene_input - RAND_MAX/2.0) * 2.0 / RAND_MAX);
}

double cut(double d)
{
    if(d > 1.0d)
    {
        return 1.0d;
    }
    else if(d < 0.0d)
    {
        return 0.0d;
    }
    return d;
}
}

GasNet::GasNet(int len_input, int len_output, GasNet_config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _network(NULL),
    _gas_emitting(NULL),
    _P()
{
    if(_config.area_size <= 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Area needs to be greater then 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
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
    AbstractNeuralNetwork(1,1),
    _config(),
    _network(NULL),
    _gas_emitting(NULL),
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
    if(_network != NULL)
    {
        delete [] _network;
    }
    if(_gas_emitting != NULL)
    {
        delete [] _gas_emitting;
    }
}

GenericGene *GasNet::getRandomGene()
{
    return new GenericGene(_len_output*2, 16);
}

AbstractNeuralNetwork *GasNet::createConfigCopy()
{
    return new GasNet(_len_input, _len_output, _config);
}

void GasNet::_initialise()
{
    if(_gene->segments()[0].length() != 16)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene segment length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[_gene->segments().length()];
    _gas_emitting = new double[_gene->segments().length()];

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        _network[i] = 0;
        _gas_emitting[i] = 0;
    }
}

void GasNet::_processInput(QList<double> input)
{
    double gas1[_gene->segments().length()];
    double gas2[_gene->segments().length()];
    double k[_gene->segments().length()];

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate gas concentration
        double gas_radius = _config.offset_gas_radius + floatFromGeneInput( _gene->segments()[i][gene_Gas_radius], _config.range_gas_radius);
        if(_gas_emitting[i] > 0.0d && _gene->segments()[i][gene_TypeGas]%3 != 0)
        {
            for(int j = 0; j < _gene->segments().length(); ++j)
            {
                double distance = calculate_distance(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                                     floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                                     floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                                     floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size));
                if(distance > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * distance)/gas_radius * _gas_emitting[i]);
                switch (_gene->segments()[i][gene_TypeGas]%3)
                {
                case 1:
                    gas1[j] += gas_concentration;
                    break;

                case 2:
                    gas2[j] += gas_concentration;
                    break;

                default:
                    qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas" << _gene->segments()[i][gene_TypeGas]%3 << "- ignoring";
                    break;
                }
            }
        }
    }

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate k
        int basis_index = _gene->segments()[i][gene_basis_index]%_P.length();
        int index = qFloor(basis_index + gas1[i] * (_P.length() - basis_index) + gas2[i] * basis_index);
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

    double *newNetwork = new double[_gene->segments().length()];

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate new input
        double newValue = 0;

        // Connections
        for(int j = 0; j < _gene->segments().length(); ++j)
        {
            if(i == j)
            {
                continue;
            }
            if(areNodesConnected(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio),
                                 floatFromGeneInput(_gene->segments()[i][gene_PositivConeExt], 2*M_PI),
                                 floatFromGeneInput(_gene->segments()[i][gene_PositivConeOrientation], 2*M_PI)))
            {
                newValue += _network[j];
            }
            if(areNodesConnected(floatFromGeneInput(_gene->segments()[i][gene_x], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[i][gene_y], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[j][gene_x], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[j][gene_y], _config.area_size),
                                 floatFromGeneInput(_gene->segments()[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio),
                                 floatFromGeneInput(_gene->segments()[i][gene_NegativConeExt], 2*M_PI),
                                 floatFromGeneInput(_gene->segments()[i][gene_NegativConeOrientation], 2*M_PI)))
            {
                newValue -= _network[j] * -1;
            }
        }

        // Recurrent connection
        switch(_gene->segments()[i][gene_recurrent]%3)
        {
        case 1:
            newValue += _network[i];
            break;
        case 2:
            newValue -= _network[i] * -1;
            break;
        default:
            break;
        }

        // Input
        if(_gene->segments()[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[_gene->segments()[i][gene_input]%(_len_input+1)-1];
        }

        // K
        newValue *= k[i];

        // Bias
        newValue += weight(_gene->segments()[i][gene_bias]);

        // tanh
        newNetwork[i] = tanh(newValue);
    }

    delete _network;
    _network = newNetwork;

    for(int i = 0; i < _gene->segments().length(); ++i)
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
            qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas circumstances" << _gene->segments()[i][gene_WhenGas]%3 << "- ignoring";
            break;
        }

        if(emittingGas)
        {
            _gas_emitting[i] = cut((_gas_emitting[i] + 1.0d) / _gene->segments()[i][gene_Rate_of_gas]);
        }
        else
        {
            _gas_emitting[i] = cut((_gas_emitting[i] - 1.0d) / _gene->segments()[i][gene_Rate_of_gas]);
        }
    }
}

double GasNet::_getNeuronOutput(int i)
{
    if(i >= 0 && i < _len_output)
    {
        return _network[i];
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0;
    }
}

