#include "feedforwardnetwork.h"

#include "commonnetworkfunctions.h"

#include <QDebug>
#include <QtCore/qmath.h>

using CommonNetworkFunctions::weight;
using CommonNetworkFunctions::sigmoid;

FeedForwardNetwork::FeedForwardNetwork(int len_input, int len_output, config config) :
    AbstractNeuralNetwork(len_input, len_output),
    _config(config),
    _hidden_layers(NULL),
    _output(NULL)
{
}

FeedForwardNetwork::FeedForwardNetwork() :
    AbstractNeuralNetwork(),
    _config(),
    _hidden_layers(NULL),
    _output(NULL)
{
}

FeedForwardNetwork::~FeedForwardNetwork()
{
    if(_hidden_layers != NULL)
    {
        for(int i = 0; i < _config.num_hidden_layer; ++i)
        {
            delete [] _hidden_layers[i];
        }
        delete [] _hidden_layers;
    }
    if(_output != NULL)
    {
        delete [] _output;
    }
}

void FeedForwardNetwork::_initialise()
{
    if(_gene->segments().length() < num_segments(_len_input, _len_output, _config.num_hidden_layer, _config.len_hidden))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Wrong gene length!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    else if(_config.len_hidden <= 0 || _config.num_hidden_layer < 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: invalid hidden layer size!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    if(_config.len_hidden > 0)
    {
        _hidden_layers = new double*[_config.num_hidden_layer];
        for(int i = 0; i < _config.num_hidden_layer; ++i)
        {
            _hidden_layers[i] = new double[_config.len_hidden];
        }
    }
    _output = new double[_len_output];
}

void FeedForwardNetwork::_processInput(QList<double> input)
{
    if(input.length() != _len_input)
    {
        qCritical() << "CRITICAL ERROR in " __FILE__ << " " << __LINE__ << ": input length" << input.length() << "!= _len_input" << _len_input;
        return;
    }

    QList< QList<int> > segments = _gene->segments();

    if(_config.num_hidden_layer == 0)
    {
        int current_segment = 0;
        for(int i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0d;
            for(int i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(segments[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(segments[current_segment++][0], _config.weight_scalar);
            _output[i_output] = _config.activision_function(sum);
        }
    }
    else
    {
        int current_segment = 0;

        // Input to hidden
        for(int i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
        {
            double sum = 0.0d;
            for(int i_input = 0; i_input < _len_input; ++i_input)
            {
                sum += input[i_input] * weight(segments[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(segments[current_segment++][0], _config.weight_scalar);
            _hidden_layers[0][i_hidden] = _config.activision_function(sum);
        }

        // Hidden to hidden
        for(int current_hidden = 1; current_hidden < _config.num_hidden_layer; ++current_hidden)
        {
            for(int i_output = 0; i_output < _config.len_hidden; ++i_output)
            {
                double sum = 0.0d;
                for(int i_input = 0; i_input < _config.len_hidden; ++i_input)
                {
                    sum += _hidden_layers[current_hidden-1][i_input] * weight(segments[current_segment++][0], _config.weight_scalar);
                }
                sum += 1.0d * weight(segments[current_segment++][0], _config.weight_scalar);
                _hidden_layers[current_hidden][i_output] = _config.activision_function(sum);
            }
        }

        // Hidden to output
        for(int i_output = 0; i_output < _len_output; ++i_output)
        {
            double sum = 0.0d;
            for(int i_hidden = 0; i_hidden < _config.len_hidden; ++i_hidden)
            {
                sum += _hidden_layers[_config.num_hidden_layer-1][i_hidden] * weight(segments[current_segment++][0], _config.weight_scalar);
            }
            sum += 1.0d * weight(segments[current_segment++][0], _config.weight_scalar);
            _output[i_output] = _config.activision_function(sum);
        }
    }
}

double FeedForwardNetwork::_getNeuronOutput(int i)
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

int FeedForwardNetwork::num_segments(int len_input, int len_output, int hidden_layer, int len_hidden)
{
    if(hidden_layer == 0)
    {
        return len_input * len_output + len_output;
    }
    else
    {
        return len_input*len_hidden + len_hidden*len_hidden*(hidden_layer-1) + len_output*len_hidden + len_output + hidden_layer*len_hidden;
    }
}

double FeedForwardNetwork::standard_activision_function(double input)
{
    return sigmoid(input);
}

GenericGene *FeedForwardNetwork::getRandomGene()
{
    return new GenericGene(num_segments(_len_input, _len_output, _config.num_hidden_layer, _config.len_hidden));
}

AbstractNeuralNetwork *FeedForwardNetwork::createConfigCopy()
{
    return new FeedForwardNetwork(_len_input, _len_output, _config);
}
