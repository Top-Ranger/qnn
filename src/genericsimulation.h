#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include "Networks/abstractneuralnetwork.h"

class GeneralSimulation
{
public:
    GeneralSimulation(AbstractNeuralNetwork network);

    virtual void getScore();
};

#endif // GENERICSIMULATION_H
