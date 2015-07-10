#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include <qnn-global.h>

#include "../network/abstractneuralnetwork.h"

class QNNSHARED_EXPORT GenericSimulation
{
public:
    GenericSimulation();
    virtual ~GenericSimulation();

    void initialise(AbstractNeuralNetwork *network);
    double getScore();
    virtual int needInputLength();
    virtual int needOutputLength();

    virtual GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    AbstractNeuralNetwork *_network;
};

#endif // GENERICSIMULATION_H
