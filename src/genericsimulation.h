#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include "Networks/abstractneuralnetwork.h"

class GeneralSimulation
{
public:
    GeneralSimulation(AbstractNeuralNetwork *network);
    virtual ~GeneralSimulation();

    virtual void getScore() = 0;

protected:
    AbstractNeuralNetwork *_network;
};

#endif // GENERICSIMULATION_H
