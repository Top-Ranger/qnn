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

#ifndef LENGTHCHANGINGGENE_H
#define LENGTHCHANGINGGENE_H

#include <qnn-global.h>

#include "genericgene.h"

class QNNSHARED_EXPORT LengthChangingGene : public GenericGene
{
public:
    struct config {
        qint32 min_length; // -1 = initialLength
        qint32 max_length; // -1 = initialLength*4

        config() :
            min_length(-1),
            max_length(-1)
        {
        }
    };

    LengthChangingGene(qint32 initialLength, qint32 segment_size = 1, config config = config());
    ~LengthChangingGene();

    void mutate();
    GenericGene *createCopy();

    static GenericGene *loadThisGene(QIODevice *device);

protected:
    LengthChangingGene();
    LengthChangingGene(QList< QList<qint32> > gene, qint32 segment_size, config config = config());

    GenericGene *createGene(QList< QList<qint32> > gene, qint32 segment_size);

    QString identifier();
    bool _saveGene(QTextStream *stream);
    GenericGene *_loadGene(QList< QList<qint32> > gene, qint32 segment_size, QTextStream *stream);

    config _config;
};

#endif // LENGTHCHANGINGGENE_H
