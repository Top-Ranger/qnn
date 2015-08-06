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

#ifndef TMAZESIMULATION_H
#define TMAZESIMULATION_H

#include <qnn-global.h>

#include "genericsimulation.h"
#include <QList>

class QNNSHARED_EXPORT TMazeSimulation : public GenericSimulation
{
public:
    static QList<int> generateStandardTMaze();
    static bool standardG1Correct(QList<int> list);

    struct config {
        int trials;
        int max_timesteps;
        int range_input;
        QList<int> (*generateTMaze)();
        bool (*G1Correct)(QList<int> list);

        config() :
            trials(24),
            max_timesteps(50),
            range_input(5),
            generateTMaze(&generateStandardTMaze),
            G1Correct(&standardG1Correct)
        {
        }
    };

    TMazeSimulation(config config = config());
    ~TMazeSimulation();

    int needInputLength();
    int needOutputLength();

    GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    config _config;
};

#endif // TMAZESIMULATION_H
