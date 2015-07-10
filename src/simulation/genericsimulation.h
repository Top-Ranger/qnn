#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include "../network/abstractneuralnetwork.h"

class GenericSimulation
{
public:
    GenericSimulation();
    virtual ~GenericSimulation();

    void initialise(AbstractNeuralNetwork *network);
    double getScore();

    virtual GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    AbstractNeuralNetwork *_network;
};

#endif // GENERICSIMULATION_H
