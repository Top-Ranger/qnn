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
#include <QDebug>

AbstractNeuralNetwork::AbstractNeuralNetwork(int len_input, int len_output) :
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
    if(_gene != NULL)
    {
        delete _gene;
    }
}

void AbstractNeuralNetwork::initialise(GenericGene *gene)
{
    if(_gene != NULL)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network already initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _gene = gene->createCopy();
    _initialise();
}

void AbstractNeuralNetwork::processInput(QList<double> input)
{
    if(_gene == NULL)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network not initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    _processInput(input);
}

double AbstractNeuralNetwork::getNeuronOutput(int i)
{
    if(_gene == NULL)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Network not initialised").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    return _getNeuronOutput(i);
}

bool AbstractNeuralNetwork::saveNetworkConfig(QIODevice *device)
{
    if(device == NULL)
    {
        return false;
    }

    if(_gene == NULL)
    {
        qCritical() << "WARNING in " __FILE__ << " " << __LINE__ << ": Network not initialised";
        return false;
    }

    if(device->isOpen())
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Saving to an open device is not permitted";
        return false;
    }
    if(!device->open(QIODevice::WriteOnly))
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Can not open device";
        return false;
    }

    QXmlStreamWriter stream(device);
    bool result = _saveNetworkConfig(&stream);
    result = result && !stream.hasError();
    device->close();
    return result;
}
