#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include "Networks/abstractneuralnetwork.h"

class GeneralSimulation
{
public:
    GeneralSimulation(AbstractNeuralNetwork *network);
    virtual ~GeneralSimulation();

    virtual double getScore() = 0;

protected:
    AbstractNeuralNetwork *_network;
};

#endif // GENERICSIMULATION_H
