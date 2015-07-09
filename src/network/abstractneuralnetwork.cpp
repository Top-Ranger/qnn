#include "abstractneuralnetwork.h"
#include <QString>
#include <QDebug>

AbstractNeuralNetwork::AbstractNeuralNetwork(int len_input, int len_output) :
    _len_input(len_input),
    _len_output(len_output),
    _gene(NULL)
{
}

AbstractNeuralNetwork::AbstractNeuralNetwork() :
    _len_input(0),
    _len_output(0),
    _gene(NULL)
{
}

AbstractNeuralNetwork::~AbstractNeuralNetwork()
{
}

void AbstractNeuralNetwork::initialise(GenericGene *gene)
{
    if(_gene != 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network already initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _gene = gene;
    _initialise();
}

void AbstractNeuralNetwork::processInput(QList<double> input)
{
    if(_gene == 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network not initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _processInput(input);
}

double AbstractNeuralNetwork::getNeuronOutput(int i)
{
    if(_gene == 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network not initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    return _getNeuronOutput(i);
}
