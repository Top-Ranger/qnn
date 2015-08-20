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

#ifndef GENERICGENE_H
#define GENERICGENE_H

#include <qnn-global.h>

#include <QList>
#include <QIODevice>
#include <QTextStream>

class QNNSHARED_EXPORT GenericGene
{
public:
    GenericGene(int initialLength, int segment_size = 1);
    virtual ~GenericGene();

    virtual void mutate();
    virtual QList< QList<int> > segments();
    virtual GenericGene *createCopy();

    static QList<GenericGene *> combine(GenericGene *gene1, GenericGene *gene2);

    bool saveGene(QIODevice *device);
    GenericGene *loadGene(QIODevice *device);
    bool canLoad(QIODevice *device);

    int getIndependentRandomInt();

    static GenericGene *loadThisGene(QIODevice *device);

protected:
    GenericGene();
    GenericGene(QList< QList<int> > gene, int segment_size);

    virtual GenericGene *createGene(QList< QList<int> > gene, int segment_size);

    virtual QString identifier();
    virtual bool _saveGene(QTextStream *stream);
    virtual GenericGene *_loadGene(QList< QList<int> > gene, int segment_size, QTextStream *stream);

    QList< QList<int> > _gene;
    int _segment_size;

    static const double MUTATION_RATE = 0.03d;
};

#endif // GENERICGENE_H
