#include "feedforwardnetwork.h"
#include <QDebug>
#include <QtCore/qmath.h>
#include <cstdlib>

namespace {
double weight(int gene_input)
{
    return ((double) gene_input - RAND_MAX/2) * 2 / RAND_MAX;
}
}

FeedForwardNetwork::FeedForwardNetwork(GenericGene gene, int len_input, int len_output, int hidden_layer, int hidden_size, double (*activision_function)(double)) :
    AbstractNeuralNetwork(gene, len_input, len_output),
    _num_hidden_layer(hidden_layer),
    _size_hidden(hidden_size),
    _hidden_layers(0),
    _output(0),
    _gene(gene),
    _activision_function(activision_function)
{
    if(_gene.segments().length() < num_segments(len_input, len_output, hidden_layer, hidden_size))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    if(_size_hidden > 0)
    {
        _hidden_layers = new double*[_num_hidden_layer];
        for(int i = 0; i < _size_hidden; ++i)
        {
            _hidden_layers[i] = new double[_size_hidden];
        }
    }
    _output = new double[_len_output];
}

FeedForwardNetwork::FeedForwardNetwork() :
    AbstractNeuralNetwork(),
    _num_hidden_layer(0),
    _size_hidden(0),
    _hidden_layers(0),
    _output(0),
    _gene(1),
    _activision_function(&standard_activision_function)
{
}

FeedForwardNetwork::~FeedForwardNetwork()
{
    for(int i = 0; i < _size_hidden; ++i)
    {
        delete [] _hidden_layers[i];
    }
    delete [] _hidden_layers;
    delete [] _output;
}

void FeedForwardNetwork::processInput(QList<double> input)
{
    if(input.length() != _len_input)
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": input length" << input.length() << "!= _len_input" << _len_input;
        return;
    }

    if(_num_hidden_layer == 0)
    {
        int current_segment = 0;
        for(int i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0;
            for(int i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(_gene.segments()[current_segment++][0]);
            }
            sum += 1.0d * weight(_gene.segments()[current_segment++][0]);
            _output[i_output] = _activision_function(sum);
        }
    }
    else
    {
        int current_segment = 0;

        // Input to hidden
        for(int i_hidden = 0; i_hidden < _size_hidden; ++i_hidden)
        {
            double sum = 0.0;
            for(int i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(_gene.segments()[current_segment++][0]); 
            }
            sum += 1.0d * weight(_gene.segments()[current_segment++][0]);
            _hidden_layers[0][i_hidden] = _activision_function(sum);
        }

        // Hidden to hidden
        for(int current_hidden = 1; current_hidden < _num_hidden_layer; ++current_hidden)
        {
            for(int i_output = 0; i_output < _len_output; ++i_output)
            {
                double sum = 0.0;
                for(int i_input = 0; i_input < _len_input; ++i_input)
                {
                    sum += _hidden_layers[current_hidden-1][i_input] * weight(_gene.segments()[current_segment++][0]);
                }
                sum += 1.0d * weight(_gene.segments()[current_segment++][0]);
                _hidden_layers[current_hidden][i_output] = _activision_function(sum);
            }
        }

        // Hidden to output
        for(int i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0;
            for(int i_hidden = 0; i_hidden < _size_hidden; ++i_hidden)
            {
                sum += _hidden_layers[_num_hidden_layer-1][i_hidden] * weight(_gene.segments()[current_segment++][0]);
            }
            sum += 1.0d * weight(_gene.segments()[current_segment++][0]);
            _output[i_output] = _activision_function(sum);
        }
    }
}

double FeedForwardNetwork::getNeuronOutput(int i)
{
    if(i >= 0 && i < _len_output)
    {
        return _output[i];
    }
    else
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": i out of bound";
        return -1.0;
    }
}

int FeedForwardNetwork::num_segments(int len_input, int len_output, int hidden_layer, int hidden_size)
{
    if(hidden_layer == 0)
    {
        return len_input * len_output + len_output;
    }
    else
    {
        return len_input*hidden_size + hidden_size*(hidden_layer-1) + len_output*hidden_size + len_output + hidden_layer*hidden_size;
    }
}

double FeedForwardNetwork::standard_activision_function(double input)
{
    return 1 / (1 + qExp(-1 * input));
}
