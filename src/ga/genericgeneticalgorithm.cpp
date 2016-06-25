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

#include "genericgeneticalgorithm.h"

#include <QThread>
#include <QtAlgorithms>
#include <QTime>
#include <QtConcurrentRun>
#include <QFuture>
#include <randomhelper.h>

namespace {

static const qint32 MAX_FORWARD_RANDOM = 256;

double runOneSimulation(AbstractSimulation *simulation)
{
    double result = simulation->getScore();
    delete simulation;
    return result;
}
}

GenericGeneticAlgorithm::GenericGeneticAlgorithm(AbstractNeuralNetwork *network, AbstractSimulation *simulation, qint32 population_size, double fitness_to_reach, qint32 max_rounds, QObject *parent) :
    QObject(parent),
    _population(),
    _best(),
    _network(NULL),
    _simulation(NULL),
    _population_size(population_size),
    _fitness_to_reach(fitness_to_reach),
    _max_rounds(max_rounds),
    _average_fitness(-1.0),
    _rounds_to_finish(-1)
{
    if(Q_UNLIKELY(network == NULL))
    {
        QNN_FATAL_MSG("Network might not be NULL");
    }
    if(Q_UNLIKELY(simulation == NULL))
    {
        QNN_FATAL_MSG("Simulation might not be NULL");
    }
    if(Q_UNLIKELY(population_size <= 0))
    {
        QNN_FATAL_MSG("Population size must be greater then 0");
    }
    if(Q_UNLIKELY(max_rounds <= 0))
    {
        QNN_FATAL_MSG("Max rounds must be greater then 0");
    }
    _network = network->createConfigCopy();
    _simulation = simulation->createConfigCopy();

    _best.fitness = -1.0;
    _best.gene = NULL;
    _best.network = NULL;
}

GenericGeneticAlgorithm::GenericGeneticAlgorithm(QObject *parent) :
    QObject(parent),
    _population(),
    _best(),
    _network(NULL),
    _simulation(NULL),
    _population_size(0),
    _fitness_to_reach(0.0),
    _max_rounds(0),
    _average_fitness(0),
    _rounds_to_finish(0)
{
    _best.fitness = -1.0;
    _best.gene = NULL;
    _best.network = NULL;
}

GenericGeneticAlgorithm::~GenericGeneticAlgorithm()
{
    delete _best.network;
    delete _best.gene;
    delete _network;
    delete _simulation;
}

void GenericGeneticAlgorithm::runGa()
{
    if(Q_UNLIKELY(_network == NULL))
    {
        QNN_FATAL_MSG("Network might not be NULL");
    }
    if(Q_UNLIKELY(_simulation == NULL))
    {
        QNN_FATAL_MSG("Simulation might not be NULL");
    }

    delete _best.network;
    _best.network = NULL;
    delete _best.gene;
    _best.gene = NULL;

    _population.clear();

    createInitialPopulation();

    Q_ASSERT_X(_population.length() == _population_size, "GenericGeneticAlgorithm::run_ga after create_initial_population()", "size of population does not match _population_size");

    qSort(_population);

    emit ga_current_round(0, _max_rounds, _population.last().fitness, calculateAverageFitness());

    // Main loop    
    qint32 currentRound = 0;
    while(currentRound++ < _max_rounds && _population.last().fitness < _fitness_to_reach)
    {
        createChildren();
        survivorSelection();
        Q_ASSERT_X(_population.length() == _population_size, "GenericGeneticAlgorithm::run_ga after create_children(), survivor_selection()", "size of population does not match _population_size");
        qSort(_population);
        emit ga_current_round(currentRound, _max_rounds, _population.last().fitness, calculateAverageFitness());
    }

    // Find the best individuum
    _best.fitness = _population.last().fitness;
    _best.gene = _population.last().gene;
    _best.network = _population.last().network;

    _average_fitness = calculateAverageFitness();
    _rounds_to_finish = currentRound-1;

    // Clean-up
    for(qint32 i = 0; i < _population.length()-1; ++i)
    {
        delete _population[i].network;
        delete _population[i].gene;
    }
    _population.clear();
    emit ga_finished(_best.fitness, _average_fitness, _rounds_to_finish);
}

double GenericGeneticAlgorithm::bestFitness()
{
    return _best.fitness;
}

GenericGene *GenericGeneticAlgorithm::bestGene()
{
    if(_best.gene == NULL)
    {
        return NULL;
    }
    return _best.gene->createCopy();
}

double GenericGeneticAlgorithm::averageFitness()
{
    return _average_fitness;
}

qint32 GenericGeneticAlgorithm::roundsToFinish()
{
    return _rounds_to_finish;
}

AbstractNeuralNetwork *GenericGeneticAlgorithm::getNetworkCopy()
{
    return _network->createConfigCopy();
}

AbstractSimulation *GenericGeneticAlgorithm::getSimulationCopy()
{
    return _simulation->createConfigCopy();
}

void GenericGeneticAlgorithm::createInitialPopulation()
{
    QList< QFuture<double> > threadList;

    for(qint32 i = 0; i < _population_size; ++i)
    {
        GeneContainer container;
        container.fitness = -1.0;
        container.gene = _network->getRandomGene();
        container.network = _network->createConfigCopy();
        _population.append(container);
    }

    for(qint32 i = 0; i < _population_size; ++i)
    {
        AbstractSimulation *simulation = _simulation->createConfigCopy();
        simulation->initialise(_population[i].network, _population[i].gene);
        threadList.append(QtConcurrent::run(runOneSimulation, simulation));
    }

    for(qint32 i = 0; i < _population_size; ++i)
    {
        _population[i].fitness = threadList[i].result();
    }
}

void GenericGeneticAlgorithm::createChildren()
{
    QList< QList<GeneContainer> > temp;
    QList< QList<GeneContainer> > newChildren;
    QList<GenericGene *> childrenGene;
    QList< QList< QFuture<double> > > threadList;
    QList<GeneContainer> saveSmallPopulation;
    qint32 number_list = 0;

    while(!_population.empty())
    {
        childrenGene.clear();
        temp.append(QList<GeneContainer>());
        for(qint32 i = 0; i < 8 && !_population.empty(); ++i)
        {
            temp[number_list].append(_population.takeAt(RandomHelper::getRandomInt(0, _population.length()-1)));
        }
        qSort(temp[number_list]);
        if(Q_LIKELY(temp[number_list].length() >= 2))
        {
            childrenGene = temp[number_list][temp[number_list].length()-1].gene->combine(temp[number_list][temp[number_list].length()-1].gene, temp[number_list][temp[number_list].length()-2].gene);
            newChildren.append(QList<GeneContainer>());
            threadList.append(QList< QFuture<double> >());
            for(qint32 i = 0; i < childrenGene.length(); ++i)
            {
                childrenGene[i]->mutate();
                GeneContainer container;
                container.gene = childrenGene[i];
                container.network = _network->createConfigCopy();
                container.fitness = -1.0;
                newChildren[number_list].append(container);
                AbstractSimulation *simulation = _simulation->createConfigCopy();
                simulation->initialise(container.network, container.gene);
                threadList[number_list].append(QtConcurrent::run(runOneSimulation, simulation));
            }
            ++number_list;
        }
        else
        {
            if(Q_LIKELY(!temp.isEmpty()))
            {
                saveSmallPopulation.append(temp.takeLast());
            }
            else
            {
                QNN_WARNING_MSG("Trying to append empty list");
            }
        }

    }

    for(qint32 j = 0; j < number_list; ++j)
    {
        for(qint32 i = 0; i < newChildren[j].length(); ++i)
        {
            newChildren[j][i].fitness = threadList[j][i].result();
        }
        temp[j].append(newChildren[j]);
        qSort(temp[j]);
        for(qint32 i = 0; i < newChildren[j].length(); ++i)
        {
            GeneContainer container = temp[j].takeFirst();
            delete container.network;
            delete container.gene;
        }
        _population.append(temp[j]);
    }
    _population.append(saveSmallPopulation);
}

void GenericGeneticAlgorithm::survivorSelection()
{
    // Not needed because survivors are selected in create_children
}

double GenericGeneticAlgorithm::calculateAverageFitness()
{
    if(_population.length() == 0)
    {
        QNN_CRITICAL_MSG("Calling calculate_average_fitness with empty population");
        return -1.0;
    }

    double d = 0.0;
    foreach(GeneContainer container, _population)
    {
        d += container.fitness;
    }
    return d / _population.length();
}
