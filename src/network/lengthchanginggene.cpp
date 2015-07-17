#include "lengthchanginggene.h"

#include <QDebug>

LengthChangingGene::LengthChangingGene(int initialLength, int segment_size, LengthChangingGene_config config) :
    GenericGene(initialLength, segment_size),
    _config(config)
{
    if(config.max_length < config.min_length)
    {
        qFatal(QString("FATAL ERROR in %1 %2: min length must be smaller or equal to max length").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
    if(initialLength < config.min_length || initialLength > config.max_length)
    {
        qFatal(QString("FATAL ERROR in %1 %2: initial length must be between min length and max length").arg(__FILE__).arg(__LINE__).toLatin1().data());
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

LengthChangingGene::LengthChangingGene(QList< QList<int> > gene, int segment_size, LengthChangingGene_config config) :
    GenericGene(gene, segment_size),
    _config(config)
{
    if(gene.length() < config.min_length || gene.length() > config.max_length)
    {
        qFatal(QString("FATAL ERROR in %1 %2: gene length must be between min length and max length").arg(__FILE__).arg(__LINE__).toLatin1().data());
    }
}

void LengthChangingGene::mutate()
{
    GenericGene::mutate();
    if(_gene.length() > _config.min_length)
    {
        if((double) qrand()/(double) RAND_MAX < MUTATION_RATE)
        {
            _gene.removeAt(qrand()%_gene.length());
        }
    }
    if(_gene.length() < _config.max_length)
    {
        if((double) qrand()/(double) RAND_MAX < MUTATION_RATE)
        {
            QList<int> newSegment;
            for(int j = 0; j < _segment_size; ++j)
            {
                newSegment.append(qrand());
            }
            _gene.append(newSegment);
        }
    }
}

GenericGene *LengthChangingGene::loadThisGene(QIODevice *device)
{
    LengthChangingGene gene;
    return gene.loadGene(device);
}

GenericGene *LengthChangingGene::createGene(QList< QList<int> > gene, int segment_size)
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

GenericGene *LengthChangingGene::_loadGene(QList< QList<int> > gene, int segment_size, QTextStream *stream)
{
    LengthChangingGene_config config;

    QString command;

    // min_length
    *stream >> command;
    if(command != "min_length")
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": No min_length";
        return NULL;
    }
    *stream >> config.min_length;

    // max_length
    *stream >> command;
    if(command != "max_length")
    {
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": No max_length";
        return NULL;
    }
    *stream >> config.max_length;

    return new LengthChangingGene(gene, segment_size, config);
}
