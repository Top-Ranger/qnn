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

#ifndef ABSTRACTNEURALNETWORK_H
#define ABSTRACTNEURALNETWORK_H

#include <qnn-global.h>

#include "genericgene.h"

#include <QList>
#include <QXmlStreamWriter>

class QNNSHARED_EXPORT AbstractNeuralNetwork
{
public:
    AbstractNeuralNetwork(qint32 len_input, qint32 len_output);
    virtual ~AbstractNeuralNetwork();

    void initialise(GenericGene *gene);
    void processInput(QList<double> input);
    double getNeuronOutput(qint32 i);

    bool saveNetworkConfig(QIODevice *device);

    virtual GenericGene *getRandomGene() = 0;
    virtual AbstractNeuralNetwork *createConfigCopy() = 0;

protected:
    AbstractNeuralNetwork();

    virtual void _initialise() = 0;
    virtual void _processInput(QList<double> input) = 0;
    virtual double _getNeuronOutput(qint32 i) = 0;

    virtual bool _saveNetworkConfig(QXmlStreamWriter *stream) = 0;

    qint32 _len_input;
    qint32 _len_output;
    GenericGene *_gene;
};

#endif // ABSTRACTNEURALNETWORK_H
