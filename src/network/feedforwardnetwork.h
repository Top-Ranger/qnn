#ifndef FEEDFORWARDNETWORK_H
#define FEEDFORWARDNETWORK_H

#include "abstractneuralnetwork.h"

class FeedForwardNetwork : public AbstractNeuralNetwork
{
public:
    FeedForwardNetwork(int len_input, int len_output, int hidden_layer = 1, int len_hidden = 5,
                       double (*activision_function)(double) = &standard_activision_function);
    ~FeedForwardNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

    static int num_segments(int len_input, int len_output, int hidden_layer = 1, int len_hidden = 5);
    static double standard_activision_function(double input);

protected:
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

private:
    FeedForwardNetwork();

    int _num_hidden_layer;
    int _len_hidden;

    double **_hidden_layers;
    double *_output;
    double (*_activision_function)(double input);
};

#endif // FEEDFORWARDNETWORK_H
