#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include <qnn-global.h>

#include "../network/abstractneuralnetwork.h"
#include "../network/genericgene.h"

class QNNSHARED_EXPORT GenericSimulation
{
public:
    GenericSimulation();
    virtual ~GenericSimulation();

    void initialise(AbstractNeuralNetwork *network, GenericGene *gene);
    double getScore();
    virtual int needInputLength();
    virtual int needOutputLength();

    virtual GenericSimulation *createConfigCopy();

protected:
    virtual void _initialise();
    virtual double _getScore();

    AbstractNeuralNetwork *_network;
    GenericGene *_gene;
};

#endif // GENERICSIMULATION_H
