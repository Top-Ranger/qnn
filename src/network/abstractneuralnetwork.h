#ifndef ABSTRACTNEURALNETWORK_H
#define ABSTRACTNEURALNETWORK_H

#include "genericgene.h"
#include <QList>

class AbstractNeuralNetwork
{
public:
    AbstractNeuralNetwork(int len_input, int len_output);
    virtual ~AbstractNeuralNetwork();

    void initialise(GenericGene *gene);
    void processInput(QList<double> input);
    double getNeuronOutput(int i);

    virtual GenericGene *getRandomGene() = 0;
    virtual AbstractNeuralNetwork *createConfigCopy() = 0;

protected:
    AbstractNeuralNetwork();

    virtual void _initialise() = 0;
    virtual void _processInput(QList<double> input) = 0;
    virtual double _getNeuronOutput(int i) = 0;

    int _len_input;
    int _len_output;
    GenericGene *_gene;
};

#endif // ABSTRACTNEURALNETWORK_H
