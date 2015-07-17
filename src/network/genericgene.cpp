#include "genericgene.h"
#include <QTime>
#include <QDebug>
#include <cstdlib>

GenericGene::GenericGene()
{
}

GenericGene::GenericGene(int initialLength, int segment_size) :
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

    for(int i = 0; i < initialLength; ++i)
    {
        QList<int> list;
        for(int j = 0; j < _segment_size; ++j)
        {
            list.append(qrand());
        }
        _gene.append(list);
    }
}

GenericGene::GenericGene(QList< QList<int> > gene, int segment_size) :
    _gene(gene),
    _segment_size(segment_size)
{
}

GenericGene::~GenericGene()
{
}

QList< QList<int> > GenericGene::segments()
{
    return _gene;
}

void GenericGene::mutate()
{
    //Simple mutation py probability - the chance of mutating a value is the same for every value.
    for(int i = 0; i < _gene.length(); ++i)
    {
        for(int j = 0; j < _gene[i].length(); ++j)
        {
            if((double) qrand()/(double) RAND_MAX < MUTATION_RATE)
            {
                _gene[i][j] = qrand();
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
    QList< QList<int> > newGene1;
    QList< QList<int> > newGene2;
    int smallerLength = gene1->_gene.length() < gene2->_gene.length() ? gene1->_gene.length() : gene2->_gene.length();
    int largerLength = gene1->_gene.length() > gene2->_gene.length() ? gene1->_gene.length() : gene2->_gene.length();
    int outer_crossover = qrand() % smallerLength;
    int inner_crossover = qrand() % gene1->_gene[0].length();
    int i;
    for(i = 0; i < outer_crossover; ++i)
    {
        newGene1.append(gene1->_gene[i]);
        newGene2.append(gene2->_gene[i]);
    }
    QList<int> crossover1;
    QList<int> crossover2;
    for(int j = 0; j < gene1->_gene[0].length(); ++j)
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
    foreach(QList<int> segment, _gene)
    {
        stream << "genesegment ";
        foreach(int i, segment)
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

    int segment_size;
    QList< QList<int> > gene;

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
               QList<int> segment;
               for(int i = 0; i < segment_size; ++i)
               {
                   int value;
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

GenericGene *GenericGene::loadThisGene(QIODevice *device)
{
    GenericGene gene;
    return gene.loadGene(device);
}

GenericGene *GenericGene::createGene(QList< QList<int> > gene, int segment_size)
{
    return new GenericGene(gene, segment_size);
}

QString GenericGene::identifier()
{
    return QString("GenericGene");
}

bool GenericGene::_saveGene(QTextStream *stream)
{
    return true;
}

GenericGene *GenericGene::_loadGene(QList< QList<int> > gene, int segment_size, QTextStream *stream)
{
    return new GenericGene(gene, segment_size);
}
