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

/*!
 * \brief The NonParallelGenericGeneticAlgorithm class is a genetic algorithm which does not run parallel.
 */
class QNNSHARED_EXPORT NonParallelGenericGeneticAlgorithm : public GenericGeneticAlgorithm
{
public:
    /*!
     * Constructor of NonParallelGenericGeneticAlgorithm
     *
     * \brief Constructor of NonParallelGenericGeneticAlgorithm
     * \param network The network which should be optimised. Might not be NULL
     * \param simulation The simulation for which the network should be optimised. Might not be NULL
     * \param population_size The population size
     * \param fitness_to_reach The fitness which should be reached. Once it has been reached the genetic algorithm will finish
     * \param max_rounds The maximum amount of rounds. The genetic algorithm will abort after the amount of rounds
     * \param parent The parent of the object
     */
    NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size = 300, double fitness_to_reach = 0.99, qint32 max_rounds = 200, QObject *parent = 0);

    /*!
     * \brief Deconstructor
     */
    ~NonParallelGenericGeneticAlgorithm();

protected:
    /*!
     * \brief Empty constructor.
     *
     * This constructor may be useful for subclasses.
     */
    explicit NonParallelGenericGeneticAlgorithm(QObject *parent = 0);

    /*!
     * \brief In this function the initial population is created. This is an overwritten function.
     */
    virtual void create_initial_population();

    /*!
     * \brief In this function the children in the genetic algorithm are created. This is an overwritten function.
     */
    void create_children();

    /*!
     * \brief In this function the survivors are created. This is an overwritten function.
     */
    void survivor_selection();
};

#endif // NONPARALLELGENERICGENETICALGORITHM_H
