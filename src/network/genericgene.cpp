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
#include <QDebug>
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
    if(initialLength < 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: initial length can not be negativ!").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(segment_size <= 0)
    {
        qFatal(QString("FATAL ERROR in %1 %2: segment size must be 1 or greater").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }

    for(qint32 i = 0; i < initialLength; ++i)
    {
        QList<qint32> list;
        for(qint32 j = 0; j < _segment_size; ++j)
        {
            list.append(getIndependentRandomInt());
        }
        _gene.append(list);
    }
}

GenericGene::GenericGene(QList< QList<qint32> > gene, qint32 segment_size) :
    _gene(gene),
    _segment_size(segment_size)
{
}

GenericGene::~GenericGene()
{
}

QList< QList<qint32> > GenericGene::segments()
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
    for(qint32 i = 0; i < _gene.length(); ++i)
    {
        for(qint32 j = 0; j < _gene[i].length(); ++j)
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
    if(gene1->_gene[0].length() != gene2->_gene[0].length())
    {
        qCritical() << "WARNING in " __FILE__ << " " << __LINE__ << ": Attemted crossover of different type of genes";
        return QList<GenericGene *>();
    }
    QList< QList<qint32> > newGene1;
    QList< QList<qint32> > newGene2;
    qint32 smallerLength = gene1->_gene.length() < gene2->_gene.length() ? gene1->_gene.length() : gene2->_gene.length();
    qint32 largerLength = gene1->_gene.length() > gene2->_gene.length() ? gene1->_gene.length() : gene2->_gene.length();
    qint32 outer_crossover = qrand() % smallerLength;
    qint32 inner_crossover = qrand() % gene1->_gene[0].length();
    qint32 i;
    for(i = 0; i < outer_crossover; ++i)
    {
        newGene1.append(gene1->_gene[i]);
        newGene2.append(gene2->_gene[i]);
    }
    QList<qint32> crossover1;
    QList<qint32> crossover2;
    for(qint32 j = 0; j < gene1->_gene[0].length(); ++j)
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
        if(i < gene2->_gene.length())
        {
            newGene1.append(gene2->_gene[i]);
        }
        if(i < gene1->_gene.length())
        {
            newGene2.append(gene1->_gene[i]);
        }
    }

    QList<GenericGene *> geneList;
    geneList.append(gene1->createGene(newGene1, gene1->_gene[0].length()));
    geneList.append(gene2->createGene(newGene2, gene2->_gene[0].length()));
    return geneList;
}

bool GenericGene::saveGene(QIODevice *device)
{
    if(device == NULL)
    {
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
    QTextStream stream(device);

    stream << identifier() << " ";
    stream << "segments " << _segment_size << " ";
    stream << "gene ";
    foreach(QList<qint32> segment, _gene)
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
    if(device == NULL)
    {
        return NULL;
    }

    qint32 segment_size;
    QList< QList<qint32> > gene;

    if(device->isOpen())
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Loading to an open device is not permitted";
        return NULL;
    }
    if(!device->open(QIODevice::ReadOnly) || device->atEnd())
    {
        device->close();
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Can not open device";
        return NULL;
    }
    QTextStream stream(device);
    QString command;
    stream >> command;

    // Identifier
    if(command != identifier())
    {
        device->close();
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Wrong gene type";
        return NULL;
    }

    // segments
    stream >> command;
    if(command != "segments")
    {
        device->close();
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": No segment size";
        return NULL;
    }
    stream >> segment_size;

    // gene
    stream >> command;
    if(command != "gene")
    {
        device->close();
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": No segment size";
        return NULL;
    }

    bool gene_reading = true;
    while(gene_reading)
    {
        stream >> command;
        if(command == "genesegment")
        {
               QList<qint32> segment;
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
            qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown command" << command;
            return NULL;
        }
    }

    GenericGene *newGene = _loadGene(gene, segment_size, &stream);
    device->close();
    return newGene;
}

bool GenericGene::canLoad(QIODevice *device)
{
    if(device == NULL)
    {
        return false;
    }
    if(device->isOpen())
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Loading to an open device is not permitted";
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
    d /= random_divisor;

    // Sometimes d might be slightly bigger then 1 because of float imprecision

    if(d > 1.0d)
    {
        d = 1.0d;
    }
    return MAX_GENE_VALUE * d;
}

GenericGene *GenericGene::loadThisGene(QIODevice *device)
{
    GenericGene gene;
    return gene.loadGene(device);
}

GenericGene *GenericGene::createGene(QList< QList<qint32> > gene, qint32 segment_size)
{
    return new GenericGene(gene, segment_size);
}

QString GenericGene::identifier()
{
    return QString("GenericGene");
}

bool GenericGene::_saveGene(QTextStream *stream)
{
    Q_UNUSED(stream);
    return true;
}

GenericGene *GenericGene::_loadGene(QList< QList<qint32> > gene, qint32 segment_size, QTextStream *stream)
{
    Q_UNUSED(stream);
    return new GenericGene(gene, segment_size);
}
