#include "continuoustimerecurrenneuralnetwork.h"
#include <QDebug>
#include <QtCore/qmath.h>

// GENE ENCODING: θ, input, τ, W
// weight between -5,5

namespace {
double weight(int gene_input)
{
    return (((double) gene_input - RAND_MAX/2.0) * 2.0 / RAND_MAX) * 5;
}

double sigmoid(double d)
{
    return 1.0d / (1.0d + qExp(-1.0d * d));
}
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, int size_network) :
    AbstractNeuralNetwork(len_input, len_output),
    _size_network(size_network),
    _network(NULL)
{
    if(_size_network < len_input || _size_network < len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: size_network must be bigger then len_input and len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork() :
    AbstractNeuralNetwork(),
    _size_network(0),
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
    return new ContinuousTimeRecurrenNeuralNetwork(_len_input, _len_output, _size_network);
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
            d += weight(_gene->segments()[j][0]); // θj
            d += _network[j]; // yj
            d = sigmoid(d);
            newValue += d * weight(_gene->segments()[i][3+i]); // wij
        }
        newNetwork[i] = newValue / ((_gene->segments()[i][2]%4)+1); // τ
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
