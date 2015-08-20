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

#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include <qnn-global.h>

#include "../network/abstractneuralnetwork.h"
#include "../network/genericgene.h"

class QNNSHARED_EXPORT GenericSimulation
{
public:
    GenericSimulation();
    virtual ~GenericSimulation();

    void initialise(AbstractNeuralNetwork *network, GenericGene *gene);
    double getScore();
    virtual qint32 needInputLength();
    virtual qint32 needOutputLength();

    virtual GenericSimulation *createConfigCopy();

protected:
    virtual void _initialise();
    virtual double _getScore();

    AbstractNeuralNetwork *_network;
    GenericGene *_gene;
};

#endif // GENERICSIMULATION_H
