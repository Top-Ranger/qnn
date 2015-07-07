#ifndef GENERALGENE_H
#define GENERALGENE_H

#include <QList>

class GeneralGene
{
public:
    GeneralGene(int initialLength);

    virtual void mutate();
    virtual QList<QList<int>> segments();

    static QList<GenericGene> mutate(GeneralGene gene1, GeneralGene gene2);

protected:
    virtual int _segment_size = 1;
};

#endif // GENERALGENE_H
