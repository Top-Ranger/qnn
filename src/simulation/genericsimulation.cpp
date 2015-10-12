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

#include "genericsimulation.h"

#include <QList>
#include <QString>

GenericSimulation::GenericSimulation() :
    _network(NULL),
    _gene(NULL)
{ 
}

GenericSimulation::~GenericSimulation()
{
    delete _network;
    delete _gene;
}

void GenericSimulation::initialise(AbstractNeuralNetwork *network, GenericGene *gene)
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

double GenericSimulation::getScore()
{
    if(Q_UNLIKELY(_network == NULL || _gene == NULL))
    {
        QNN_FATAL_MSG("Network not initialised");
    }
    return _getScore();
}

qint32 GenericSimulation::needInputLength()
{
    return 5;
}

qint32 GenericSimulation::needOutputLength()
{
    return 1;
}

void GenericSimulation::_initialise()
{
    _network->initialise(_gene);
}

GenericSimulation *GenericSimulation::createConfigCopy()
{
    return new GenericSimulation();
}

double GenericSimulation::_getScore()
{
    QList<double> input;
    input << 0.2 << 0.4 << 0.6 << 0.8 << 1.0;
    _network->processInput(input);
    return _network->getNeuronOutput(0);
}
