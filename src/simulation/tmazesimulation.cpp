#include "tmazesimulation.h"

#include <QDebug>

namespace {
enum Direction {
    up_direction = 0,
    left_direction = 1,
    down_direction = 2,
    right_direction = 3,
    none_direction = 4,
    start_direction = 5
};
}

TMazeSimulation::TMazeSimulation(config config) :
    GenericSimulation(),
    _config(config)
{
}

TMazeSimulation::~TMazeSimulation()
{
}

int TMazeSimulation::needInputLength()
{
    return 1;
}

int TMazeSimulation::needOutputLength()
{
    return 4;
}

GenericSimulation *TMazeSimulation::createConfigCopy()
{
    return new TMazeSimulation(_config);
}

void TMazeSimulation::_initialise()
{
}

double TMazeSimulation::_getScore()
{
    double score = 0.0d;

    for(int trial = 0; trial < _config.trials; ++trial)
    {
        QList<double> TMaze = _config.generateTMaze();
        int position = 0;
        bool goalNotReached = true;

        AbstractNeuralNetwork *network = _network->createConfigCopy();
        network->initialise(_gene);
        for(int timestep = 0; timestep < _config.max_timesteps && goalNotReached; ++timestep)
        {
            Direction direction = start_direction;
            double max_output = -10.0d;

            QList<double> input;
            input << TMaze[position];
            network->processInput(input);

            for(int i = 0; i < 4; ++i)
            {
                if(network->getNeuronOutput(i) > max_output)
                {
                    max_output = network->getNeuronOutput(i);
                    direction = (Direction) i;
                }
                else if(network->getNeuronOutput(i) == max_output)
                {
                    direction = none_direction;
                }
            }

            switch(direction)
            {
            case start_direction:
                qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Direction ist start_direction";
                break;

            case none_direction:
                break;

            case up_direction:
                if(position < TMaze.length()-1)
                {
                    ++position;
                }
                break;

            case down_direction:
                if(position > 0)
                {
                    --position;
                }
                break;

            case left_direction:
                if(position == TMaze.length()-1)
                {
                    // G1
                    goalNotReached = false;
                    if(_config.G1Correct(TMaze))
                    {
                        score += 1.0d;
                    }
                }
                break;

            case right_direction:
                if(position == TMaze.length()-1)
                {
                    // G2
                    goalNotReached = false;
                    if(!_config.G1Correct(TMaze))
                    {
                        score += 1.0d;
                    }
                }
                break;
            }
        }
    }

    return score / _config.trials;
}

QList<double> TMazeSimulation::generateStandardTMaze()
{
    QList<double> list;
    double number = (qrand()%10)+1;
    list << number;
    for(int i = 0; i < 5; ++i)
    {
        list << 0;
    }
    if(qrand()%2)
    {
        list << number;
    }
    else
    {
        list << (qrand()%10)+1;
    }
    return list;
}

bool TMazeSimulation::standardG1Correct(QList<double> list)
{
    if(list.length() < 2)
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": List shorter than 2 - returning default false";
        return false;
    }
    return list.first() == list.last();
}
