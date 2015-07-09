#ifndef FEEDFORWARDNETWORK_H
#define FEEDFORWARDNETWORK_H

#include "abstractneuralnetwork.h"

// TODO: Bias

class FeedForwardNetwork : public AbstractNeuralNetwork
{
public:
    FeedForwardNetwork(GenericGene gene, int len_input, int len_output, int hidden_layer = 1, int hidden_size = 5,
                       double (*activision_function)(double) = &standard_activision_function);
    ~FeedForwardNetwork();

    void processInput(QList<double> input);
    double getNeuronOutput(int i);

    static int num_segments(int len_input, int len_output, int hidden_layer = 1, int hidden_size = 5);
    static double standard_activision_function(double input);
private:
    FeedForwardNetwork();

    int _num_hidden_layer;
    int _size_hidden;

    double **_hidden_layers;
    double *_output;
    GenericGene _gene;
    double (*_activision_function)(double input);
};

#endif // FEEDFORWARDNETWORK_H
