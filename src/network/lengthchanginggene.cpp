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

#include "lengthchanginggene.h"

LengthChangingGene::LengthChangingGene(qint32 initialLength, qint32 segment_size, config config) :
    GenericGene(initialLength, segment_size),
    _config(config)
{
    if(_config.min_length == -1)
    {
        _config.min_length = initialLength;
    }

    if(_config.max_length == -1)
    {
        _config.max_length = initialLength*4;
    }

    if(Q_UNLIKELY(_config.max_length < _config.min_length))
    {
        QNN_FATAL_MSG("Min length must be smaller or equal to max length");
    }
    if(Q_UNLIKELY(initialLength < _config.min_length || initialLength > _config.max_length))
    {
        QNN_FATAL_MSG("Initial length must be between min length and max length");
    }
}

LengthChangingGene::~LengthChangingGene()
{
}

LengthChangingGene::LengthChangingGene() :
    GenericGene(),
    _config()
{
}

LengthChangingGene::LengthChangingGene(QVector< QVector<qint32> > gene, qint32 segment_size, config config) :
    GenericGene(gene, segment_size),
    _config(config)
{
    if(Q_UNLIKELY(gene.size() < config.min_length || gene.size() > config.max_length))
    {
        QNN_FATAL_MSG("Gene length must be between min length and max length");
    }
}

void LengthChangingGene::mutate()
{
    GenericGene::mutate();
    if(_gene.size() > _config.min_length)
    {
        if(((double) qrand()/(double) RAND_MAX) < MUTATION_RATE)
        {
            _gene.remove(qrand()%_gene.size());
        }
    }
    if(_gene.size() < _config.max_length)
    {
        if(((double) qrand()/(double) RAND_MAX) < MUTATION_RATE)
        {
            QVector<qint32> newSegment;
            newSegment.reserve(_segment_size);
            for(qint32 j = 0; j < _segment_size; ++j)
            {
                newSegment.append(getIndependentRandomInt());
            }
            _gene.append(newSegment);
        }
    }
}

GenericGene *LengthChangingGene::createCopy()
{
    return new LengthChangingGene(_gene, _segment_size, _config);
}

GenericGene *LengthChangingGene::loadThisGene(QIODevice *device)
{
    LengthChangingGene gene;
    return gene.loadGene(device);
}

bool LengthChangingGene::canLoadThisGene(QIODevice *device)
{
    LengthChangingGene gene;
    return gene.canLoad(device);
}

GenericGene *LengthChangingGene::createGene(QVector< QVector<qint32> > gene, qint32 segment_size)
{
    return new LengthChangingGene(gene, segment_size, _config);
}

QString LengthChangingGene::identifier()
{
    return "LengthChangingGene";
}

bool LengthChangingGene::_saveGene(QTextStream *stream)
{
    *stream << "min_length " << _config.min_length << " ";
    *stream << "max_length " << _config.max_length << " ";
    return true;
}

GenericGene *LengthChangingGene::_loadGene(QVector< QVector<qint32> > gene, qint32 segment_size, QTextStream *stream)
{
    config config;

    QString command;

    // min_length
    *stream >> command;
    if(command != "min_length")
    {
        QNN_CRITICAL_MSG("No min_length");
        return NULL;
    }
    *stream >> config.min_length;

    // max_length
    *stream >> command;
    if(command != "max_length")
    {
        QNN_CRITICAL_MSG("No max_length");
        return NULL;
    }
    *stream >> config.max_length;

    return new LengthChangingGene(gene, segment_size, config);
}
