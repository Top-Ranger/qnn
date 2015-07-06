#ifndef GENERALGENE_H
#define GENERALGENE_H

#include <QList>

class GeneralGene
{
public:
    GeneralGene(int initialLength);

    virtual void mutate();
    virtual QList<int> data();

    static QList<GenericGene> mutate(GeneralGene gene1, GeneralGene gene2);
};

#endif // GENERALGENE_H
