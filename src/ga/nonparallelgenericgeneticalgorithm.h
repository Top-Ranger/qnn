#ifndef NONPARALLELGENERICGENETICALGORITHM_H
#define NONPARALLELGENERICGENETICALGORITHM_H

#include <qnn-global.h>

#include "genericgeneticalgorithm.h"

class NonParallelGenericGeneticAlgorithm : public GenericGeneticAlgorithm
{
public:
    NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, int population_size = 300, double fitness_to_reach = 0.99, int max_rounds = 200, QObject *parent = 0);
    ~NonParallelGenericGeneticAlgorithm();

    void run_ga();

protected:
    explicit NonParallelGenericGeneticAlgorithm(QObject *parent = 0);

    void create_children();
    void survivor_selection();
};

#endif // NONPARALLELGENERICGENETICALGORITHM_H
