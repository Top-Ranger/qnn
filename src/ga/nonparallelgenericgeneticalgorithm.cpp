#include "nonparallelgenericgeneticalgorithm.h"

#include <QTime>

NonParallelGenericGeneticAlgorithm::NonParallelGenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, int population_size, double fitness_to_reach, int max_rounds, QObject *parent) :
    GenericGeneticAlgorithm(network, simulation, population_size, fitness_to_reach, max_rounds, parent)
{
}

NonParallelGenericGeneticAlgorithm::NonParallelGenericGeneticAlgorithm(QObject *parent) :
    GenericGeneticAlgorithm(parent)
{
}

NonParallelGenericGeneticAlgorithm::~NonParallelGenericGeneticAlgorithm()
{
}

void NonParallelGenericGeneticAlgorithm::run_ga()
{
    // Initialise
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    if(_best.gene != NULL)
    {
        delete _best.gene;
        _best.gene = NULL;
    }
    if(_best.network != NULL)
    {
        delete _best.network;
        _best.network = NULL;
    }

    _population.clear();

    int currentRound = 0;

    for(int i = 0; i < _population_size; ++i)
    {
        GeneContainer container;
        container.gene = _network->getRandomGene();
        container.network = _network->createConfigCopy();
        GenericSimulation *simulation = _simulation->createConfigCopy();
        simulation->initialise(container.network, container.gene);
        container.fitness = simulation->getScore();
        delete simulation;
        _population.append(container);
    }

    qSort(_population);

    emit ga_current_round(0, _max_rounds, _population.last().fitness, calculate_average_fitness());

    // Main loop

    while(currentRound++ < _max_rounds && _population.last().fitness < _fitness_to_reach)
    {
        create_children();
        survivor_selection();
        qSort(_population);
        emit ga_current_round(currentRound, _max_rounds, _population.last().fitness, calculate_average_fitness());
    }

    // Find the best individuum
    _best.fitness = _population.last().fitness;
    _best.gene = _population.last().gene;
    _best.network = _population.last().network;

    _average_fitness = calculate_average_fitness();
    _rounds_to_finish = currentRound-1;

    // Clean-up
    for(int i = 0; i < _population.length()-1; ++i)
    {
        delete _population[i].network;
        delete _population[i].gene;
    }
    _population.clear();
    emit ga_finished(_best.fitness, _average_fitness, _rounds_to_finish);
}

void NonParallelGenericGeneticAlgorithm::create_children()
{
    QList<GeneContainer> newPopulation;
    QList<GeneContainer> temp;
    QList<GeneContainer> newChildren;
    QList<GenericGene *> childrenGene;

    while(!_population.empty())
    {
        temp.clear();
        newChildren.clear();
        childrenGene.clear();
        for(int i = 0; i < 8 && !_population.empty(); ++i)
        {
            temp.append(_population.takeAt(qrand()%_population.length()));
        }
        qSort(temp);
        if(temp.length() >= 2)
        {
            childrenGene = temp[temp.length()-1].gene->combine(temp[temp.length()-1].gene, temp[temp.length()-2].gene);
            for(int i = 0; i < childrenGene.length(); ++i)
            {
                childrenGene[i]->mutate();
                GeneContainer container;
                container.gene = childrenGene[i];
                container.network = _network->createConfigCopy();
                GenericSimulation *simulation = _simulation->createConfigCopy();
                simulation->initialise(container.network, container.gene);
                container.fitness = simulation->getScore();
                newChildren.append(container);
                delete simulation;
            }
            temp.append(newChildren);
            qSort(temp);
            for(int i = 0; i < childrenGene.length(); ++i)
            {
                GeneContainer container = temp.takeFirst();
                delete container.network;
                delete container.gene;
            }
            newPopulation.append(temp);
        }
    }
    _population = newPopulation;
}

void NonParallelGenericGeneticAlgorithm::survivor_selection()
{
    // Not needed because survivors are selected in create_children
}
