#ifndef CONTINUOUSTIMERECURRENNEURALNETWORK_H
#define CONTINUOUSTIMERECURRENNEURALNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, int size_network, int max_time_constant = 5, int weight_scalar = 5, int bias_scalar = 5);
    ~ContinuousTimeRecurrenNeuralNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ContinuousTimeRecurrenNeuralNetwork();

    enum CTRNNgene_positions {gene_bias = 0,
                              gene_input = 1,
                              gene_time_constraint = 2,
                              gene_W_start = 3};

                             void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

private:
    int _size_network;
    int _max_time_constant;
    int _weight_scalar;
    int _bias_scalar;
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
