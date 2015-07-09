#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include "../network/abstractneuralnetwork.h"

class GenericSimulation
{
public:
    GenericSimulation(AbstractNeuralNetwork *network);
    virtual ~GenericSimulation();

    virtual double getScore();

protected:
    AbstractNeuralNetwork *_network;
};

#endif // GENERICSIMULATION_H
