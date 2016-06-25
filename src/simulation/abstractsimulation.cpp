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

#include "abstractsimulation.h"

#include <QList>
#include <QString>

AbstractSimulation::AbstractSimulation() :
    _network(NULL),
    _gene(NULL)
{ 
}

AbstractSimulation::~AbstractSimulation()
{
    delete _network;
    delete _gene;
}

void AbstractSimulation::initialise(AbstractNeuralNetwork *network, GenericGene *gene)
{
    if(Q_UNLIKELY(_network != NULL || _gene != NULL))
    {
        QNN_FATAL_MSG("Simulation already initialised");
    }
    if(Q_UNLIKELY(network == NULL || gene == NULL))
    {
        QNN_FATAL_MSG("Trying to initialise with NULL");
    }

    _network = network->createConfigCopy();
    _gene = gene->createCopy();
    _initialise();
}

double AbstractSimulation::getScore()
{
    if(Q_UNLIKELY(_network == NULL || _gene == NULL))
    {
        QNN_FATAL_MSG("Network not initialised");
    }
    return _getScore();
}
