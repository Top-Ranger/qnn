#ifndef GENERICGENE_H
#define GENERICGENE_H

#include <QList>

class GenericGene
{
public:
    GenericGene(int initialLength, int segment_size = 1);
    virtual ~GenericGene();

    virtual void mutate();
    virtual QList< QList<int> > segments();

    static QList<GenericGene> combine(GenericGene gene1, GenericGene gene2);

protected:
    GenericGene();
    GenericGene(QList< QList<int> > gene, int segment_size);
    QList< QList<int> > _gene;
    int _segment_size;

private:
    static const double MUTATION_RATE = 0.03;
};

#endif // GENERICGENE_H
