#ifndef CONTINUOUSTIMERECURRENNEURALNETWORK_H
#define CONTINUOUSTIMERECURRENNEURALNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    struct ContinuousTimeRecurrenNeuralNetwork_config {
        int size_network;
        int max_time_constant;
        int weight_scalar;
        int bias_scalar;

        ContinuousTimeRecurrenNeuralNetwork_config() :
            size_network(7),
            max_time_constant(5),
            weight_scalar(5),
            bias_scalar(5)
        {
        }
    };

    ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, ContinuousTimeRecurrenNeuralNetwork_config config = ContinuousTimeRecurrenNeuralNetwork_config());
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
    ContinuousTimeRecurrenNeuralNetwork_config _config;
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
