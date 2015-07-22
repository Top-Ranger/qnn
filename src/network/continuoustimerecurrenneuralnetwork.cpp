#include "continuoustimerecurrenneuralnetwork.h"

#include "commonnetworkfunctions.h"

#include <QDebug>
#include <QtCore/qmath.h>

// GENE ENCODING: θ, input, τ, W
// weight between -5,5

using CommonNetworkFunctions::sigmoid;
using CommonNetworkFunctions::weight;

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, ContinuousTimeRecurrenNeuralNetwork_config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _network(NULL)
{
    if(_config.size_network < len_output)
    {
        qFatal(QString("FATAL ERROR in %1 %2: size_network must be bigger then len_output!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(_config.max_time_constant < 1)
    {
        qFatal(QString("FATAL ERROR in %1 %2: max_time_constant must be 1 or bigger!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

ContinuousTimeRecurrenNeuralNetwork::ContinuousTimeRecurrenNeuralNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _network(NULL)
{
}

ContinuousTimeRecurrenNeuralNetwork::~ContinuousTimeRecurrenNeuralNetwork()
{
    if(_network != NULL)
    {
        delete [] _network;
    }
}

GenericGene *ContinuousTimeRecurrenNeuralNetwork::getRandomGene()
{
    return new GenericGene(_config.size_network, 3 + _config.size_network);
}

AbstractNeuralNetwork *ContinuousTimeRecurrenNeuralNetwork::createConfigCopy()
{
    return new ContinuousTimeRecurrenNeuralNetwork(_len_input, _len_output, _config);
}

void ContinuousTimeRecurrenNeuralNetwork::_initialise()
{
    if(_gene->segments().length() < _config.size_network || _gene->segments()[0].length() < (3 + _config.size_network))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Gene lenght does not fit!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = new double[_config.size_network];
    for(int i = 0; i < _config.size_network; ++i)
    {
        _network[i] = 0;
    }
}

void ContinuousTimeRecurrenNeuralNetwork::_processInput(QList<double> input)
{
    QList< QList<int> > segments = _gene->segments();

    double *newNetwork = new double[_config.size_network];

    // do calculation
    for(int i = 0; i < _config.size_network; ++i)
    {
        double newValue = -1 * _network[i]; // -y

        if(segments[i][gene_input]%(_len_input+1) != 0)
        {
            newValue += input[segments[i][gene_input]%(_len_input+1)-1];; // input
        }

        for(int j = 0; j < _config.size_network; ++j)
        {
            double d = 0.0d;
            d += weight(segments[j][gene_bias], _config.bias_scalar); // θj
            d += _network[j]; // yj
            d = sigmoid(d);
            newValue += d * weight(segments[i][gene_W_start+i], _config.weight_scalar); // wij
        }
        newNetwork[i] = newValue / ((segments[i][gene_time_constraint]%_config.max_time_constant)+1); // τ
    }

    delete [] _network;
    _network = newNetwork;
}

double ContinuousTimeRecurrenNeuralNetwork::_getNeuronOutput(int i)
{
    if(_network != NULL && i < _config.size_network)
    {
        return sigmoid(_network[i]);
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0d;
    }
}
