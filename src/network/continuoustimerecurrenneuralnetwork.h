#ifndef CONTINUOUSTIMERECURRENNEURALNETWORK_H
#define CONTINUOUSTIMERECURRENNEURALNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, int size_network);
    ~ContinuousTimeRecurrenNeuralNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ContinuousTimeRecurrenNeuralNetwork();

    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

private:
    int _size_network;
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
