#ifndef GASNET_H
#define GASNET_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT GasNet : public AbstractNeuralNetwork
{
public:
    GasNet(int len_input, int len_output, double area_size = 1.0d);
    ~GasNet();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    GasNet();
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

    double _area_size;
    double *_network;
    double *_gas_emitting;
};

#endif // GASNET_H
