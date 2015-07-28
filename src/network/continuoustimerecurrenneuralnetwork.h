#ifndef CONTINUOUSTIMERECURRENNEURALNETWORK_H
#define CONTINUOUSTIMERECURRENNEURALNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    static double standard_activision_function(double input);

    struct config {
        int size_network;
        bool size_changing;
        int max_size_network;
        int max_time_constant;
        int weight_scalar;
        int bias_scalar;
        double (*activision_function)(double);

        config() :
            size_network(-1),
            size_changing(false),
            max_size_network(-1),
            max_time_constant(5),
            weight_scalar(5),
            bias_scalar(5),
            activision_function(&standard_activision_function)
        {
        }
    };

    ContinuousTimeRecurrenNeuralNetwork(int len_input, int len_output, config config = config());
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
    config _config;
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
