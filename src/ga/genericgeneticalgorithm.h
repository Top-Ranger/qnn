#ifndef GENERICGENETICALGORITHM_H
#define GENERICGENETICALGORITHM_H

#include "../network/abstractneuralnetwork.h"
#include "../simulation/genericsimulation.h"
#include <QList>
#include <QObject>

class GenericGeneticAlgorithm : public QObject
{
    Q_OBJECT

public:
    explicit GenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, int population_size = 300, double fitness_to_reach = 0.99, int max_rounds = 200, QObject *parent = 0);
    virtual ~GenericGeneticAlgorithm();

    virtual void run_ga();
    double best_fitness();
    GenericGene best_gene();

signals:
    void ga_current_round(int current, int max, double best_fitness_value);

protected:
    explicit GenericGeneticAlgorithm(QObject *parent = 0);

    virtual void create_children();
    virtual void survivor_selection();

private:
    struct GeneContainer {
        double fitness;
        GenericGene* gene;
        AbstractNeuralNetwork *network;

        bool operator<(const GeneContainer &other) const
        {
            return fitness < other.fitness;
        }
    };

    QList<GeneContainer> _population;
    GeneContainer _best;
    AbstractNeuralNetwork *_network;
    GenericSimulation *_simulation;
    int _population_size;
    double _fitness_to_reach;
    int _max_rounds;
};

#endif // GENERICGENETICALGORITHM_H
