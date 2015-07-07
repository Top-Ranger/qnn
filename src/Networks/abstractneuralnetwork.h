#ifndef ABSTRACTNEURALNETWORK_H
#define ABSTRACTNEURALNETWORK_H

#include "generalgene.h"
#include <QList>

class AbstractNeuralNetwork
{
public:
    AbstractNeuralNetwork(GenericGene gene, int len_input, int len_output);

    virtual void processInput(QList<double> input);
    virtual double getNeuronOutput(int i);
};

#endif // ABSTRACTNEURALNETWORK_H
