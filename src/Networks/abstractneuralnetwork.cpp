#include "abstractneuralnetwork.h"

AbstractNeuralNetwork::AbstractNeuralNetwork(GeneralGene gene, int len_input, int len_output) :
    _len_input(len_input),
    _len_output(len_output)
{
}

AbstractNeuralNetwork::AbstractNeuralNetwork() :
    _len_input(0),
    _len_output(0)
{
}

AbstractNeuralNetwork::~AbstractNeuralNetwork()
{
}
