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

#ifndef NONPARALLELGENERICGENETICALGORITHM_H
#define NONPARALLELGENERICGENETICALGORITHM_H

#include <qnn-global.h>

#include "genericgeneticalgorithm.h"

class QNNSHARED_EXPORT NonParallelGenericGeneticAlgorithm : public GenericGeneticAlgorithm
{
public:
    NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size = 300, double fitness_to_reach = 0.99, qint32 max_rounds = 200, QObject *parent = 0);
    ~NonParallelGenericGeneticAlgorithm();

    void run_ga();

protected:
    explicit NonParallelGenericGeneticAlgorithm(QObject *parent = 0);

    void create_children();
    void survivor_selection();
};

#endif // NONPARALLELGENERICGENETICALGORITHM_H
