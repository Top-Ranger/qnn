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
#include <randomhelper.h>

NonParallelGenericGeneticAlgorithm::NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, AbstractSimulation *simulation, qint32 population_size, double fitness_to_reach, qint32 max_rounds, QObject *parent) :
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

void NonParallelGenericGeneticAlgorithm::createInitialPopulation()
{
    for(qint32 i = 0; i < _population_size; ++i)
    {
        GeneContainer container;
        container.gene = _network->getRandomGene();
        container.network = _network->createConfigCopy();
        AbstractSimulation *simulation = _simulation->createConfigCopy();
        simulation->initialise(container.network, container.gene);
        container.fitness = simulation->getScore();
        delete simulation;
        _population.append(container);
    }
}

void NonParallelGenericGeneticAlgorithm::createChildren()
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
            temp.append(_population.takeAt(RandomHelper::getRandomInt(0, _population.length()-1)));
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
                AbstractSimulation *simulation = _simulation->createConfigCopy();
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

void NonParallelGenericGeneticAlgorithm::survivorSelection()
{
    // Not needed because survivors are selected in create_children
}
