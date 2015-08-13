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

#ifndef REBERGRAMMARSIMULATION_H
#define REBERGRAMMARSIMULATION_H

#include <qnn-global.h>

#include "genericsimulation.h"

class QNNSHARED_EXPORT ReberGrammarSimulation : public GenericSimulation
{
public:

    enum SimulationMode {
        DetectGrammar,
        CreateWords
    };

    struct config {
        SimulationMode mode;
        bool embedded;
        int trials_detect;
        int trials_create;
        double detect_threshold;
        int max_depth;

        config() :
            mode(DetectGrammar),
            embedded(false),
            trials_detect(500),
            trials_create(50),
            detect_threshold(0.50d),
            max_depth(50)
        {
        }
    };

    ReberGrammarSimulation(config config = config());
    ~ReberGrammarSimulation();

    int needInputLength();
    int needOutputLength();

    GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    config _config;
};

#endif // REBERGRAMMARSIMULATION_H
