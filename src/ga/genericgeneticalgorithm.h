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

#ifndef GENERICGENETICALGORITHM_H
#define GENERICGENETICALGORITHM_H

#include <qnn-global.h>

#include "../network/abstractneuralnetwork.h"
#include "../simulation/genericsimulation.h"
#include <QList>
#include <QObject>

/*!
 * \brief The GenericGeneticAlgorithm class is the base class of all genetic algorithms.
 *
 * A genetic algorithm is a way to optimise networks. The idea of the algorithm is based on the natural evolution.
 *
 * The algorithm is easily customizable by overwriting create_children, survivor_selection or create_initial_population although you can also override run_ga to change the behaviour compleately.
 * You must always overwrite the following functions together:
 *  - create_children()
 *  - survivor_selection()
 */
class QNNSHARED_EXPORT GenericGeneticAlgorithm : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor of GenericGeneticAlgorithm
     *
     * The genetic algorithm will save a deep copy of the network/simulation so the caller can delete both at any time.
     *
     * \param network The network which should be optimised. Might not be NULL
     * \param simulation The simulation for which the network should be optimised. Might not be NULL
     * \param population_size The population size
     * \param fitness_to_reach The fitness which should be reached. Once it has been reached the genetic algorithm will finish
     * \param max_rounds The maximum amount of rounds. The genetic algorithm will abort after the amount of rounds
     * \param parent The parent of the object
     */
    explicit GenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size = 300, double fitness_to_reach = 0.99, qint32 max_rounds = 200, QObject *parent = 0);

    /*!
     * \brief Deconstructor
     */
    virtual ~GenericGeneticAlgorithm();

    /*!
     * \brief This method starts the genetic algorithm
     */
    virtual void runGa();

    /*!
     * \brief Return the best fitness of the last run.
     *
     * This function will return -1 if no genetic algorithm has be started.
     *
     * \return Best fitness
     */
    double bestFitness();

    /*!
     * \brief Return the best gene of the last run.
     *
     * This function will return NULL if no genetic algorithm has be started.
     *
     * \return Best gene. The caller must delete the gene
     */
    GenericGene *bestGene();

    /*!
     * \brief Return the average fitness of the last run.
     *
     * This function will return -1 if no genetic algorithm has be started.
     *
     * \return Best fitness
     */
    double averageFitness();

    /*!
     * \brief Return the number of rounds of the last run.
     *
     * This function will return -1 if no genetic algorithm has be started.
     *
     * \return Best fitness
     */
    qint32 roundsToFinish();

    /*!
     * \brief Returns a copy of the network used by the genetic algorithm.
     *
     * \return Network copy. The caller must delete the network
     */
    AbstractNeuralNetwork *getNetworkCopy();

    /*!
     * \brief Returns a copy of the simulation used by the genetic algorithm.
     *
     * \return Simulation copy. The caller must delete the simulation
     */
    GenericSimulation *getSimulationCopy();

signals:
    /*!
     * \brief ga_current_round is emittet after each rounds.
     * \param current Current round
     * \param max Maximum rounds
     * \param best_fitness_value Best current fitness
     * \param average_fitness_value Average current fitness
     */
    void ga_current_round(qint32 current, qint32 max, double best_fitness_value, double average_fitness_value);

    /*!
     * \brief ga_finished is emitted after all rounds have finished.
     * \param best_fitness_value Best fitness
     * \param average_fitness_value Average fitness
     * \param rounds Number of rounds the genetic algorithm run
     */
    void ga_finished(double best_fitness_value, double average_fitness_value, qint32 rounds);

protected:
    /*!
     * \brief Empty constructor.
     *
     * This constructor may be useful for subclasses.
     */
    explicit GenericGeneticAlgorithm(QObject *parent = 0);

    /*!
     * \brief In this function the initial population is created.
     */
    virtual void createInitialPopulation();

    /*!
     * \brief In this function the children in the genetic algorithm are created.
     */
    virtual void createChildren();

    /*!
     * \brief In this function the survivors are created.
     */
    virtual void survivorSelection();

    /*!
     * \brief Calculates the average fitness of the population
     * \return Average fitness
     */
    double calculateAverageFitness();

    /*!
     * \brief A simple container used in the population
     */
    struct GeneContainer {

        /*!
         * \brief fitness
         */
        double fitness;

        /*!
         * \brief gene
         */
        GenericGene* gene;

        /*!
         * \brief network
         */
        AbstractNeuralNetwork *network;

        /*!
         * \brief A container is greater if the fitness is smaller
         * \param other Other container
         * \return True if this container is greater
         */
        bool operator<(const GeneContainer &other) const
        {
            return fitness < other.fitness;
        }
    };

    /*!
     * \brief The population.
     */
    QList<GeneContainer> _population;

    /*!
     * \brief The best result from the last run.
     */
    GeneContainer _best;

    /*!
     * \brief The network which should be optimised
     */
    AbstractNeuralNetwork *_network;

    /*!
     * \brief The simulation which is used for the optimisation
     */
    GenericSimulation *_simulation;

    /*!
     * \brief The size of the population
     */
    qint32 _population_size;

    /*!
     * \brief The fitness which should be reached
     */
    double _fitness_to_reach;

    /*!
     * \brief The maximum amount of runs
     */
    qint32 _max_rounds;

    /*!
     * \brief The average fitness of the last run
     */
    double _average_fitness;

    /*!
     * \brief The rounds needed for the last run
     */
    qint32 _rounds_to_finish;
};

#endif // GENERICGENETICALGORITHM_H
