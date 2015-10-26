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

#include "abstractneuralnetwork.h"
#include <QString>

AbstractNeuralNetwork::AbstractNeuralNetwork(qint32 len_input, qint32 len_output) :
    _len_input(len_input),
    _len_output(len_output),
    _gene(NULL)
{
}

AbstractNeuralNetwork::AbstractNeuralNetwork() :
    _len_input(0),
    _len_output(0),
    _gene(NULL)
{
}

AbstractNeuralNetwork::~AbstractNeuralNetwork()
{
    delete _gene;
}

void AbstractNeuralNetwork::initialise(GenericGene *gene)
{
    if(Q_UNLIKELY(_gene != NULL))
    {
        QNN_FATAL_MSG("Network already initialised");
    }
    if(Q_UNLIKELY(gene == NULL))
    {
        QNN_CRITICAL_MSG("Can not initialise with NULL gene");
        return;
    }
    _gene = gene->createCopy();
    _initialise();
}

void AbstractNeuralNetwork::processInput(QList<double> input)
{
    if(Q_UNLIKELY(_gene == NULL))
    {
        QNN_FATAL_MSG("Network not initialised");
    }
    if(Q_UNLIKELY(input.length() != _len_input))
    {
        QNN_FATAL_MSG("input length != _len_input");
    }
    _processInput(input);
}

double AbstractNeuralNetwork::getNeuronOutput(qint32 i)
{
    if(Q_UNLIKELY(_gene == NULL))
    {
        QNN_FATAL_MSG("Network not initialised");
    }
    return _getNeuronOutput(i);
}

bool AbstractNeuralNetwork::saveNetworkConfig(QIODevice *device)
{
    if(Q_UNLIKELY(device == NULL))
    {
        return false;
    }

    if(Q_UNLIKELY(_gene == NULL))
    {
        QNN_CRITICAL_MSG("Network not initialised");
        return false;
    }

    bool opened_device = false;
    if(!device->isOpen())
    {
        QNN_DEBUG_MSG("Opening device");
        if(!device->open(QIODevice::WriteOnly))
        {
            QNN_CRITICAL_MSG("Can not open device");
            return false;
        }
        opened_device = true;
    }

    QXmlStreamWriter stream(device);
    bool result = _saveNetworkConfig(&stream);
    result = result && !stream.hasError();
    if(opened_device)
    {
        device->close();
    }
    return result;
}
