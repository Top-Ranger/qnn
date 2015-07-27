#ifndef FEEDFORWARDNETWORK_H
#define FEEDFORWARDNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT FeedForwardNetwork : public AbstractNeuralNetwork
{
public:
    static int num_segments(int len_input, int len_output, int hidden_layer, int len_hidden);
    static double standard_activision_function(double input);

    struct config {
        int num_hidden_layer;
        int len_hidden;
        double (*activision_function)(double);
        double weight_scalar;

        config() :
            num_hidden_layer(2),
            len_hidden(5),
            activision_function(&standard_activision_function),
            weight_scalar(1.0d)
        {
        }
    };

    FeedForwardNetwork(int len_input, int len_output, config config = config());
    ~FeedForwardNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

private:
    FeedForwardNetwork();

    config _config;
    double **_hidden_layers;
    double *_output;
};

#endif // FEEDFORWARDNETWORK_H
