#include "continuoustimerecurrenneuralnetwork.h"
#include <QDebug>
#include <QtCore/qmath.h>

// GENE ENCODING: θ, input, τ, W
// weight between -5,5

namespace {
double weight(int gene_input, int scalar)
{
    return (((double) gene_input - RAND_MAX/2.0) * 2.0 / RAND_MAX) * scalar;
}

double sigmoid(double d)
{
    return 1.0d / (1.0d + qExp(-1.0d * d));
}
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, int size_network, int max_time_constant, int weight_scalar, int bias_scalar) :
    AbstractNeuralNetwork(len_input, len_output),
    _size_network(size_network),
    _max_time_constant(max_time_constant),
    _weight_scalar(weight_scalar),
    _bias_scalar(bias_scalar),
    _network(NULL)
{
    if(_size_network < len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: size_network must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_max_time_constant < 1)
    {
        qFatal(QString("FATAL ERROR in %1 %2: max_time_constant must be 1 or bigger!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork() :
    AbstractNeuralNetwork(),
    _size_network(0),
    _max_time_constant(0),
    _weight_scalar(0),
    _bias_scalar(0),
    _network(NULL)
{
}

ContinuousTimeRecurrenNeuralNetwork::~ContinuousTimeRecurrenNeuralNetwork()
{
    if(_network != NULL)
    {
        delete _network;
    }
}

GenericGene *ContinuousTimeRecurrenNeuralNetwork::getRandomGene()
{
    return new GenericGene(_size_network, 3 + _size_network);
}

AbstractNeuralNetwork *ContinuousTimeRecurrenNeuralNetwork::createConfigCopy()
{
    return new ContinuousTimeRecurrenNeuralNetwork(_len_input, _len_output, _size_network, _max_time_constant, _weight_scalar, _bias_scalar);
}

void ContinuousTimeRecurrenNeuralNetwork::_initialise()
{
    if(_gene->segments().length() < _size_network || _gene->segments()[0].length() < (3 + _size_network))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Gene lenght does not fit!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[_size_network];
    for(int i = 0; i < _size_network; ++i)
    {
        _network[i] = 0;
    }
}

void ContinuousTimeRecurrenNeuralNetwork::_processInput(QList<double> input)
{
    double *newNetwork = new double[_size_network];

    // do calculation
    for(int i = 0; i < _size_network; ++i)
    {
        double newValue = -1 * _network[i]; // -y

        if(_gene->segments()[i][1]%(_len_input+1) != 0)
        {
            newValue += input[_gene->segments()[i][1]%(_len_input+1)-1];; // input
        }

        for(int j = 0; j < _size_network; ++j)
        {
            double d = 0.0d;
            d += weight(_gene->segments()[j][0], _bias_scalar); // θj
            d += _network[j]; // yj
            d = sigmoid(d);
            newValue += d * weight(_gene->segments()[i][3+i], _weight_scalar); // wij
        }
        newNetwork[i] = newValue / ((_gene->segments()[i][2]%_max_time_constant)+1); // τ
    }

    delete [] _network;
    _network = newNetwork;
}

double ContinuousTimeRecurrenNeuralNetwork::_getNeuronOutput(int i)
{
    if(_network != NULL && i < _size_network)
    {
        return sigmoid(_network[i]);
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0d;
    }
}
