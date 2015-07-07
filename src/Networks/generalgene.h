#ifndef GENERALGENE_H
#define GENERALGENE_H

#include <QList>

class GeneralGene
{
public:
    GeneralGene(int initialLength, int segment_size = 2);
    virtual ~GeneralGene();

    virtual void mutate();
    virtual QList< QList<int> > segments();

    static QList<GeneralGene> combine(GeneralGene gene1, GeneralGene gene2);

protected:
    GeneralGene();
    GeneralGene(QList< QList<int> > gene, int segment_size);
    QList< QList<int> > _gene;
    int _segment_size;
};

#endif // GENERALGENE_H
