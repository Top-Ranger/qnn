#include "gasnet.h"

#include <QtCore/qmath.h>
#include <QString>
#include <QDebug>

// GENE ENCODING: x, y, Rp, Rext, Rort, Rn, Rext, Rort, input, recurrent, WhenGas, TypeGas, Rate of gas (1-11), radius, basis index, bias, W

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
}

GasNet::GasNet(int len_input, int len_output, double area_size) :
    AbstractNeuralNetwork(len_input, len_output),
    _area_size(area_size),
    _network(NULL),
    _gas_emitting(NULL)
{
    if(area_size <= 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Area needs to be greater then 0").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

GasNet::GasNet() :
    AbstractNeuralNetwork(1,1),
    _area_size(1.0d),
    _network(NULL),
    _gas_emitting(NULL)
{
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
    return new GenericGene(_gene->segments().length(), 16+_gene->segments().length());
}

AbstractNeuralNetwork *GasNet::createConfigCopy()
{
    return new GasNet(_len_input, _len_output, _area_size);
}

void GasNet::_initialise()
{
    if(_gene->segments()[0].length() != 16+_gene->segments().length())
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
    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate emition of gas
    }

    double k[_gene->segments().length()];
    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate gas concentration
    }

    double *newNetwork = new double[_gene->segments().length()];

    for(int i = 0; i < _gene->segments().length(); ++i)
    {
        // Calculate new input
    }

    delete _network;
    _network = newNetwork;
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

