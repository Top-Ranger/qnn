/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn.
 *
 * qnn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with qnn.  If not, see <http://www.gnu.org/licenses/>.
 */

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

qint32 TMazeSimulation::needInputLength()
{
    return _config.range_input;
}

qint32 TMazeSimulation::needOutputLength()
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

    for(qint32 trial = 0; trial < _config.trials; ++trial)
    {
        QList<qint32> TMaze = _config.generateTMaze();
        qint32 position = 0;
        bool goalNotReached = true;

        AbstractNeuralNetwork *network = _network->createConfigCopy();
        network->initialise(_gene);
        for(qint32 timestep = 0; timestep < _config.max_timesteps && goalNotReached; ++timestep)
        {
            Direction direction = start_direction;
            double max_output = -10.0d;

            QList<double> input;
            for(qint32 i = 0; i < _config.range_input; ++i)
            {
                input << 0.0d;
            }
            if(TMaze[position] != 0)
            {
                if(Q_UNLIKELY(TMaze[position] > _config.range_input))
                {
                    qFatal(QString("FATAL ERROR in %1 %2: Value out of range!").arg(__FILE__).arg(__LINE__).toLatin1().data());
                }
                input[TMaze[position]-1] = 1.0d;
            }
            network->processInput(input);

            for(qint32 i = 0; i < 4; ++i)
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
        delete network;
    }

    return score / _config.trials;
}

QList<qint32> TMazeSimulation::generateStandardTMaze()
{
    QList<qint32> list;
    qint32 number = (qrand()%5)+1;
    list << number;
    for(qint32 i = 0; i < 5; ++i)
    {
        list << 0;
    }
    if(qrand()%2)
    {
        list << number;
    }
    else
    {
        list << (qrand()%5)+1;
    }
    return list;
}

bool TMazeSimulation::standardG1Correct(QList<qint32> list)
{
    if(list.length() < 2)
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": List shorter than 2 - returning default false";
        return false;
    }
    return list.first() == list.last();
}
