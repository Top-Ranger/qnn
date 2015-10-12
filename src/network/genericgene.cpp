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

#include "genericgene.h"
#include <QTime>
#include <cstdlib>

namespace {
static qint32 random_exponent = 0;
static double random_divisor = RAND_MAX;
void calculateRandomFactor()
{
    random_exponent = 1;
    random_divisor = RAND_MAX;
    while(random_divisor < MAX_GENE_VALUE)
    {
        ++random_exponent;
        random_divisor *= RAND_MAX;
    }
}
}

GenericGene::GenericGene()
{
}

GenericGene::GenericGene(qint32 initialLength, qint32 segment_size) :
    _gene(),
    _segment_size(segment_size)
{
    if(Q_UNLIKELY(initialLength < 0))
    {
        QNN_FATAL_MSG("Initial length can not be negativ");
    }
    if(Q_UNLIKELY(segment_size <= 0))
    {
        QNN_FATAL_MSG("Segment size must be greater then 0");
    }

    _gene.reserve(initialLength);
    for(qint32 i = 0; i < initialLength; ++i)
    {
        QVector<qint32> list;
        for(qint32 j = 0; j < _segment_size; ++j)
        {
            list.reserve(_segment_size);
            list.append(getIndependentRandomInt());
        }
        _gene.append(list);
    }
}

GenericGene::GenericGene(QVector< QVector<qint32> > gene, qint32 segment_size) :
    _gene(gene),
    _segment_size(segment_size)
{
}

GenericGene::~GenericGene()
{
}

QVector<QVector<qint32> > &GenericGene::segments()
{
    return _gene;
}

GenericGene *GenericGene::createCopy()
{
    return new GenericGene(_gene, _segment_size);
}

void GenericGene::mutate()
{
    //Simple mutation py probability - the chance of mutating a value is the same for every value.
    for(qint32 i = 0; i < _gene.size(); ++i)
    {
        for(qint32 j = 0; j < _gene[i].size(); ++j)
        {
            if((double) qrand()/(double) RAND_MAX < MUTATION_RATE)
            {
                _gene[i][j] = getIndependentRandomInt();
            }
        }
    }
}

QList<GenericGene *> GenericGene::combine(GenericGene *gene1, GenericGene *gene2)
{
    if(gene1->_gene[0].size() != gene2->_gene[0].size())
    {
        QNN_CRITICAL_MSG("Attemted crossover of different type of genes");
        return QList<GenericGene *>();
    }
    QVector< QVector<qint32> > newGene1;
    QVector< QVector<qint32> > newGene2;
    qint32 smallerLength = gene1->_gene.size() < gene2->_gene.size() ? gene1->_gene.size() : gene2->_gene.size();
    qint32 largerLength = gene1->_gene.size() > gene2->_gene.size() ? gene1->_gene.size() : gene2->_gene.size();
    qint32 outer_crossover = qrand() % smallerLength;
    qint32 inner_crossover = qrand() % gene1->_gene[0].size();
    qint32 i;
    for(i = 0; i < outer_crossover; ++i)
    {
        newGene1.append(gene1->_gene[i]);
        newGene2.append(gene2->_gene[i]);
    }
    QVector<qint32> crossover1;
    QVector<qint32> crossover2;
    for(qint32 j = 0; j < gene1->_gene[0].size(); ++j)
    {
        if(i < inner_crossover)
        {
            crossover1.append(gene1->_gene[i][j]);
            crossover2.append(gene2->_gene[i][j]);
        }
        else
        {
            crossover1.append(gene2->_gene[i][j]);
            crossover2.append(gene1->_gene[i][j]);
        }
    }
    newGene1.append(crossover1);
    newGene2.append(crossover2);
    for(++i; i < largerLength; ++i)
    {
        if(i < gene2->_gene.size())
        {
            newGene1.append(gene2->_gene[i]);
        }
        if(i < gene1->_gene.size())
        {
            newGene2.append(gene1->_gene[i]);
        }
    }

    QList<GenericGene *> geneList;
    geneList.append(gene1->createGene(newGene1, gene1->_gene[0].size()));
    geneList.append(gene2->createGene(newGene2, gene2->_gene[0].size()));
    return geneList;
}

bool GenericGene::saveGene(QIODevice *device)
{
    if(Q_UNLIKELY(device == NULL))
    {
        return false;
    }
    if(Q_UNLIKELY(device->isOpen()))
    {
        QNN_CRITICAL_MSG("Saving to an open device is not permitted");
        return false;
    }
    if(!device->open(QIODevice::WriteOnly))
    {
        QNN_CRITICAL_MSG("Can not open device");
        return false;
    }
    QTextStream stream(device);

    stream << identifier() << " ";
    stream << "segments " << _segment_size << " ";
    stream << "gene ";
    foreach(QVector<qint32> segment, _gene)
    {
        stream << "genesegment ";
        foreach(qint32 i, segment)
        {
            stream << i << " ";
        }
    }
    stream << "geneend ";
    bool result = _saveGene(&stream);
    device->close();
    return result;
}

GenericGene *GenericGene::loadGene(QIODevice *device)
{
    if(Q_UNLIKELY(device == NULL))
    {
        return NULL;
    }

    qint32 segment_size;
    QVector< QVector<qint32> > gene;

    if(Q_UNLIKELY(device->isOpen()))
    {
        QNN_CRITICAL_MSG("Loading to an open device is not permitted");
        return NULL;
    }
    if(!device->open(QIODevice::ReadOnly) || device->atEnd())
    {
        device->close();
        QNN_CRITICAL_MSG("Can not open device");
        return NULL;
    }
    QTextStream stream(device);
    QString command;
    stream >> command;

    // Identifier
    if(command != identifier())
    {
        device->close();
        QNN_CRITICAL_MSG("Wrong gene type");
        return NULL;
    }

    // segments
    stream >> command;
    if(command != "segments")
    {
        device->close();
        QNN_CRITICAL_MSG("No segment size");
        return NULL;
    }
    stream >> segment_size;

    // gene
    stream >> command;
    if(command != "gene")
    {
        device->close();
        QNN_CRITICAL_MSG("No segment size");
        return NULL;
    }

    bool gene_reading = true;
    while(gene_reading)
    {
        stream >> command;
        if(command == "genesegment")
        {
            QVector<qint32> segment;
               for(qint32 i = 0; i < segment_size; ++i)
               {
                   qint32 value;
                   stream >> value;
                   segment.append(value);
               }
               gene.append(segment);
        }
        else if (command == "geneend")
        {
            gene_reading = false;
        }
        else
        {
            device->close();
            QNN_CRITICAL_MSG("Unknown command");
            return NULL;
        }
    }

    GenericGene *newGene = _loadGene(gene, segment_size, &stream);
    device->close();
    return newGene;
}

bool GenericGene::canLoad(QIODevice *device)
{
    if(Q_UNLIKELY(device == NULL))
    {
        return false;
    }
    if(Q_UNLIKELY(device->isOpen()))
    {
        QNN_WARNING_MSG("Loading to an open device is not permitted");
        return false;
    }
    if(!device->open(QIODevice::ReadOnly) || device->atEnd())
    {
        device->close();
        return false;
    }
    QString gene_identifier;

    QTextStream stream(device);

    stream >> gene_identifier;

    device->close();

    return gene_identifier == identifier();
}

qint32 GenericGene::getIndependentRandomInt()
{
    // Because the range of the RNG in different libraries does not need to be equal we have to calculate a platform-independent value to make genes platform-independent
    if(Q_UNLIKELY(random_exponent == 0))
    {
        calculateRandomFactor();
    }
    double d = 0;
    for(qint32 i = 0; i < random_exponent; ++i)
    {
        d *= RAND_MAX;
        d += qrand();
    }

    // Sometimes d might be slightly bigger then 1 because of float imprecision
    // Therefore we have to check for overflows
    qint64 result = MAX_GENE_VALUE * (d / random_divisor);
    if(Q_UNLIKELY(result > MAX_GENE_VALUE))
    {
        // Overflow has occured
        result = MAX_GENE_VALUE;
    }
    else if(Q_UNLIKELY(result < 0))
    {
        // negativ value has occured
        result = 0;
    }
    return result;
}

GenericGene *GenericGene::loadThisGene(QIODevice *device)
{
    GenericGene gene;
    return gene.loadGene(device);
}

GenericGene *GenericGene::createGene(QVector< QVector<qint32> > gene, qint32 segment_size)
{
    return new GenericGene(gene, segment_size);
}

QString GenericGene::identifier()
{
    return QString("GenericGene");
}

bool GenericGene::_saveGene(QTextStream *stream)
{
    Q_UNUSED(stream); // Nothing extra to save here
    return true;
}

GenericGene *GenericGene::_loadGene(QVector< QVector<qint32> > gene, qint32 segment_size, QTextStream *stream)
{
    Q_UNUSED(stream); // Nothing extra to save here
    return new GenericGene(gene, segment_size);
}
