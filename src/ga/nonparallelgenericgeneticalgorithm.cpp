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

#include "nonparallelgenericgeneticalgorithm.h"

#include <QTime>

NonParallelGenericGeneticAlgorithm::NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size, double fitness_to_reach, qint32 max_rounds, QObject *parent) :
    GenericGeneticAlgorithm(network, simulation, population_size, fitness_to_reach, max_rounds, parent)
{
}

NonParallelGenericGeneticAlgorithm::NonParallelGenericGeneticAlgorithm(QObject *parent) :
    GenericGeneticAlgorithm(parent)
{
}

NonParallelGenericGeneticAlgorithm::~NonParallelGenericGeneticAlgorithm()
{
}

void NonParallelGenericGeneticAlgorithm::run_ga()
{
    if(Q_UNLIKELY(_network == NULL))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network might not be NULL").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(Q_UNLIKELY(_simulation == NULL))
    {
        qFatal(QString("FATAL ERROR in %1 %2: Simulation might not be NULL").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    // Initialise
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

    delete _best.network;
    _best.network = NULL;
    delete _best.gene;
    _best.gene = NULL;

    _population.clear();

    qint32 currentRound = 0;

    for(qint32 i = 0; i < _population_size; ++i)
    {
        GeneContainer container;
        container.gene = _network->getRandomGene();
        container.network = _network->createConfigCopy();
        GenericSimulation *simulation = _simulation->createConfigCopy();
        simulation->initialise(container.network, container.gene);
        container.fitness = simulation->getScore();
        delete simulation;
        _population.append(container);
    }

    qSort(_population);

    emit ga_current_round(0, _max_rounds, _population.last().fitness, calculate_average_fitness());

    // Main loop

    while(currentRound++ < _max_rounds && _population.last().fitness < _fitness_to_reach)
    {
        create_children();
        survivor_selection();
        qSort(_population);
        emit ga_current_round(currentRound, _max_rounds, _population.last().fitness, calculate_average_fitness());
    }

    // Find the best individuum
    _best.fitness = _population.last().fitness;
    _best.gene = _population.last().gene;
    _best.network = _population.last().network;

    _average_fitness = calculate_average_fitness();
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

void NonParallelGenericGeneticAlgorithm::create_children()
{
    QList<GeneContainer> newPopulation;
    QList<GeneContainer> temp;
    QList<GeneContainer> newChildren;
    QList<GenericGene *> childrenGene;

    while(!_population.empty())
    {
        temp.clear();
        newChildren.clear();
        childrenGene.clear();
        for(qint32 i = 0; i < 8 && !_population.empty(); ++i)
        {
            temp.append(_population.takeAt(qrand()%_population.length()));
        }
        qSort(temp);
        if(Q_LIKELY(temp.length() >= 2))
        {
            childrenGene = temp[temp.length()-1].gene->combine(temp[temp.length()-1].gene, temp[temp.length()-2].gene);
            for(qint32 i = 0; i < childrenGene.length(); ++i)
            {
                childrenGene[i]->mutate();
                GeneContainer container;
                container.gene = childrenGene[i];
                container.network = _network->createConfigCopy();
                GenericSimulation *simulation = _simulation->createConfigCopy();
                simulation->initialise(container.network, container.gene);
                container.fitness = simulation->getScore();
                newChildren.append(container);
                delete simulation;
            }
            temp.append(newChildren);
            qSort(temp);
            for(qint32 i = 0; i < childrenGene.length(); ++i)
            {
                GeneContainer container = temp.takeFirst();
                delete container.network;
                delete container.gene;
            }
        }
        newPopulation.append(temp);
    }
    _population = newPopulation;
}

void NonParallelGenericGeneticAlgorithm::survivor_selection()
{
    // Not needed because survivors are selected in create_children
}
