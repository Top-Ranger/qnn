#include "genericsimulation.h"

#include <QList>

GenericSimulation::GenericSimulation(AbstractNeuralNetwork *network) :
    _network(network)
{
}

GenericSimulation::~GenericSimulation()
{
}

double GenericSimulation::getScore()
{
    QList<double> input;
    input << 0.2d << 0.4d << 0.6d << 0.8d << 1.0d;
    _network->processInput(input);
    return _network->getNeuronOutput(0);
}
