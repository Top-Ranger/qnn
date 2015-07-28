#include "gasnet.h"

#include "lengthchanginggene.h"
#include "commonnetworkfunctions.h"

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

GasNet::GasNet(int len_input, int len_output, config config) :
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
    if(_config.min_size != -1 && _config.min_size < _len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: min_size_network must not be smaller then output lenght!").arg(__FILE__).arg(__LINE__).toLatin1().data());
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

    return new LengthChangingGene(initial_length, 16, config);
}

AbstractNeuralNetwork *GasNet::createConfigCopy()
{
    return new GasNet(_len_input, _len_output, _config);
}

void GasNet::_initialise()
{
    if(_gene->segments().length() < _len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: gene length must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
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
    QList< QList<int> > segments = _gene->segments();

    double gas1[segments.length()];
    double gas2[segments.length()];
    double k[segments.length()];

    for(int i = 0; i < segments.length(); ++i)
    {
        // Initiation
        gas1[i] = 0;
        gas2[i] = 0;
        k[i] = 0;
    }

    for(int i = 0; i < segments.length(); ++i)
    {
        // Calculate gas concentration
        double gas_radius = _config.offset_gas_radius + floatFromGeneInput( segments[i][gene_Gas_radius], _config.range_gas_radius);
        if(_gas_emitting[i] > 0.0d && segments[i][gene_TypeGas]%3 != 0)
        {
            for(int j = 0; j < segments.length(); ++j)
            {
                double distance = calculate_distance(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                                     floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                                     floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                                     floatFromGeneInput(segments[j][gene_y], _config.area_size));
                if(distance > gas_radius)
                {
                    continue;
                }
                double gas_concentration = qExp((-2 * distance)/gas_radius * _gas_emitting[i]);
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
                    qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas" << segments[i][gene_TypeGas]%3 << "- ignoring";
                    break;
                }
            }
        }
    }

    for(int i = 0; i < segments.length(); ++i)
    {
        // Calculate k
        int basis_index = segments[i][gene_basis_index]%_P.length();
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

    double *newNetwork = new double[segments.length()];

    for(int i = 0; i < segments.length(); ++i)
    {
        // Calculate new input
        double newValue = 0;

        // Connections
        for(int j = 0; j < segments.length(); ++j)
        {
            if(i == j)
            {
                continue;
            }
            if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                 floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                 floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                 floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                 floatFromGeneInput(segments[i][gene_PositivConeRadius], _config.area_size*_config.cone_ratio),
                                 floatFromGeneInput(segments[i][gene_PositivConeExt], 2*M_PI),
                                 floatFromGeneInput(segments[i][gene_PositivConeOrientation], 2*M_PI)))
            {
                newValue += _network[j];
            }
            if(areNodesConnected(floatFromGeneInput(segments[i][gene_x], _config.area_size),
                                 floatFromGeneInput(segments[i][gene_y], _config.area_size),
                                 floatFromGeneInput(segments[j][gene_x], _config.area_size),
                                 floatFromGeneInput(segments[j][gene_y], _config.area_size),
                                 floatFromGeneInput(segments[i][gene_NegativConeRadius], _config.area_size*_config.cone_ratio),
                                 floatFromGeneInput(segments[i][gene_NegativConeExt], 2*M_PI),
                                 floatFromGeneInput(segments[i][gene_NegativConeOrientation], 2*M_PI)))
            {
                newValue -= _network[j] * -1;
            }
        }

        // Recurrent connection
        switch(segments[i][gene_recurrent]%3)
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

    for(int i = 0; i < segments.length(); ++i)
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
            qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown gas circumstances" << segments[i][gene_WhenGas]%3 << "- ignoring";
            break;
        }

        if(emittingGas)
        {
            _gas_emitting[i] = cut01((_gas_emitting[i] + 1.0d) / segments[i][gene_Rate_of_gas]);
        }
        else
        {
            _gas_emitting[i] = cut01((_gas_emitting[i] - 1.0d) / segments[i][gene_Rate_of_gas]);
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

