#ifndef GENERICGENETICALGORITHM_H
#define GENERICGENETICALGORITHM_H

#include "../network/abstractneuralnetwork.h"
#include <QList>

class GenericGeneticAlgorithm
{
public:
    GenericGeneticAlgorithm(AbstractNeuralNetwork *network, int population_size, double fitness_to_reach = 0.99, int max_rounds = 200);

    virtual void run_ga();
    double best_fitness();
    GenericGene best_gene();

signals:
    void ga_current_round(int current, int max, double best_fitness_value);

protected:
    virtual void create_children();
    virtual void survivor_selection();

private:
    struct GeneContainer {
        double fitness;
        GenericGene* gene;

        bool operator<(GeneContainer &other)
        {
            return fitness < other.fitness;
        }
    };

    QList<GeneContainer> _population;
    double best_fitness;
    GenericGene* bestGene;
};

#endif // GENERICGENETICALGORITHM_H
