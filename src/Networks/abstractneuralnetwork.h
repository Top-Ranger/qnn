#ifndef ABSTRACTNEURALNETWORK_H
#define ABSTRACTNEURALNETWORK_H

#include "generalgene.h"
#include <QList>

class AbstractNeuralNetwork
{
public:
    AbstractNeuralNetwork(GeneralGene gene, int len_input, int len_output);
    virtual ~AbstractNeuralNetwork();

    virtual void processInput(QList<double> input) = 0;
    virtual double getNeuronOutput(int i) = 0;

protected:
    AbstractNeuralNetwork();
    int _len_input;
    int _len_output;
};

#endif // ABSTRACTNEURALNETWORK_H
