#include "generalgene.h"
#include <QDebug>
#include <cstdlib>

GeneralGene::GeneralGene()
{
}

GeneralGene::GeneralGene(int initialLength, int segment_size) :
    _gene(),
    _segment_size(segment_size)
{
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

GeneralGene::GeneralGene(QList< QList<int> > gene, int segment_size) :
    _gene(gene),
    _segment_size(segment_size)
{
}

GeneralGene::~GeneralGene()
{
}


QList< QList<int> > GeneralGene::segments()
{
    return _gene;
}

void GeneralGene::mutate()
{
    //Simple mutation py probability - the chance of mutating a value is the same for every value.
    for(int i = 0; i < _gene.length(); ++i)
    {
        for(int j = 0; j < _gene[i].length(); ++j)
        {
            if(qrand()/RAND_MAX < MUTATION_RATE)
            {
                _gene[i][j] = qrand();
            }
        }
    }
}

QList<GeneralGene> GeneralGene::combine(GeneralGene gene1, GeneralGene gene2)
{
    if(gene1._gene[0].length() != gene2._gene[0].length())
    {
        qWarning() << "Attemted crossover of different type of genes";
        return QList<GeneralGene>();
    }
    QList< QList<int> > newGene1;
    QList< QList<int> > newGene2;
    int smallerLength = gene1._gene.length() < gene2._gene.length() ? gene1._gene.length() : gene2._gene.length();
    int largerLength = gene1._gene.length() > gene2._gene.length() ? gene1._gene.length() : gene2._gene.length();
    int outer_crossover = qrand() % smallerLength;
    int inner_crossover = qrand() % gene1._gene[0].length();
    int i;
    for(i = 0; i < outer_crossover; ++i)
    {
        newGene1.append(gene1._gene[i]);
        newGene2.append(gene2._gene[i]);
    }
    QList<int> crossover1;
    QList<int> crossover2;
    for(int j = 0; j < gene1._gene[0].length(); ++j)
    {
        if(i < inner_crossover)
        {
            crossover1.append(gene1._gene[i][j]);
            crossover2.append(gene2._gene[i][j]);
        }
        else
        {
            crossover1.append(gene2._gene[i][j]);
            crossover2.append(gene1._gene[i][j]);
        }
    }
    newGene1.append(crossover1);
    newGene2.append(crossover2);
    for(++i; i < largerLength; ++i)
    {
        if(i < gene2._gene.length())
        {
            newGene1.append(gene2._gene[i]);
        }
        if(i < gene1._gene.length())
        {
            newGene2.append(gene1._gene[i]);
        }
    }

    QList<GeneralGene> geneList;
    geneList.append(GeneralGene(newGene1, gene1._gene[0].length()));
    geneList.append(GeneralGene(newGene2, gene2._gene[0].length()));
    return geneList;
}
