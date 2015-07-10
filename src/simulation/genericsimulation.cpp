#include "genericsimulation.h"

#include <QList>
#include <QString>
#include <QDebug>
#include <QTime>

GenericSimulation::GenericSimulation() :
    _network(NULL)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

GenericSimulation::~GenericSimulation()
{
}

void GenericSimulation::initialise(AbstractNeuralNetwork *network)
{
    if(_network != NULL)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Simulation already initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _network = network;
    _initialise();
}

double GenericSimulation::getScore()
{
    if(_network == NULL)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network not initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    return _getScore();
}

int GenericSimulation::needInputLength()
{
    return 5;
}

int GenericSimulation::needOutputLength()
{
    return 1;
}

void GenericSimulation::_initialise()
{
}

GenericSimulation *GenericSimulation::createConfigCopy()
{
    return new GenericSimulation;
}

double GenericSimulation::_getScore()
{
    QList<double> input;
    input << 0.2d << 0.4d << 0.6d << 0.8d << 1.0d;
    _network->processInput(input);
    return _network->getNeuronOutput(0);
}
