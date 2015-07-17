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

    static QList<GenericGene *> combine(GenericGene *gene1, GenericGene *gene2);

    bool saveGene(QIODevice *device);
    GenericGene *loadGene(QIODevice *device);
    bool canLoad(QIODevice *device);

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
