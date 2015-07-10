#include "genericgeneticalgorithm.h"

#include <QThread>
#include <QtAlgorithms>
#include <QDebug>

namespace {
class SimulationThread : public QThread
{
private:
    GenericSimulation *_simulation;
    double _fitness;

public:
    explicit SimulationThread(GenericSimulation *simulation, QObject *parent = 0) :
        QThread(parent),
        _simulation(simulation),
        _fitness(0.0d)
    {
    }

    ~SimulationThread()
    {
        if(_simulation != NULL)
        {
            delete _simulation;
        }
    }

    void run()
    {
        _fitness = _simulation->getScore();
    }

    double getFitness()
    {
        return _fitness;
    }

};
}

GenericGeneticAlgorithm::GenericGeneticAlgorithm(AbstractNeuralNetwork *network, GenericSimulation *simulation, int population_size, double fitness_to_reach, int max_rounds, QObject *parent) :
    QObject(parent),
    _population(),
    _best(),
    _network(network),
    _simulation(simulation),
    _population_size(population_size),
    _fitness_to_reach(fitness_to_reach),
    _max_rounds(max_rounds)
{
    _best.fitness = -1.0;
    _best.gene = NULL;
    _best.network = NULL;
}

GenericGeneticAlgorithm::GenericGeneticAlgorithm(QObject *parent) :
    QObject(parent),
    _population(),
    _best(),
    _network(NULL),
    _simulation(NULL),
    _population_size(0),
    _fitness_to_reach(0.0d),
    _max_rounds(0)
{
    _best.fitness = -1.0;
    _best.gene = NULL;
    _best.network = NULL;
}

GenericGeneticAlgorithm::~GenericGeneticAlgorithm()
{
    if(_best.gene != NULL)
    {
        delete _best.gene;
    }
    if(_best.network != NULL)
    {
        delete _best.network;
    }
}

void GenericGeneticAlgorithm::run_ga()
{
    // Initialise
    emit ga_current_round(0, _max_rounds, 0.0d);

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

    QList<SimulationThread *> threadList;
    int currentRound = 0;

    for(int i = 0; i < _population_size; ++i)
    {
        GeneContainer container;
        container.fitness = -1.0d;
        container.gene = _network->getRandomGene();
        container.network = _network->createConfigCopy();
        container.network->initialise(container.gene);
        _population.append(container);
    }

    for(int i = 0; i < _population_size; ++i)
    {
        GenericSimulation *simulation = _simulation->createConfigCopy();
        simulation->initialise(_population[i].network);
        threadList.append(new SimulationThread(simulation));
        threadList[i]->start();
    }

    for(int i = 0; i < _population_size; ++i)
    {
        threadList[i]->wait();
        _population[i].fitness = threadList[i]->getFitness();
    }
    qDeleteAll(threadList.begin(), threadList.end());
    threadList.clear();

    // Main loop

    while(currentRound++ < _max_rounds && _population.last().fitness < _fitness_to_reach)
    {
        create_children();
        survivor_selection();
        qSort(_population);
        emit ga_current_round(currentRound, _max_rounds, _population.last().fitness);
    }

    // Find the best individuum
    _best.fitness = _population.last().fitness;
    _best.gene = _population.last().gene;
    _best.network = _population.last().network;

    // Clean-up
    for(int i = 0; i < _population.length()-1; ++i)
    {
        delete _population[i].gene;
        delete _population[i].network;
    }
    _population.clear();
    emit ga_finished(_best.fitness);
}

double GenericGeneticAlgorithm::best_fitness()
{
    return _best.fitness;
}

GenericGene GenericGeneticAlgorithm::best_gene()
{
    return *(_best.gene);
}

void GenericGeneticAlgorithm::create_children()
{
    QList<GeneContainer> newPopulation;
    QList<GeneContainer> temp;
    QList<GeneContainer> newChildren;
    QList<GenericGene *> childrenGene;
    QList<SimulationThread *> threadList;

    while(!_population.empty())
    {
        temp.clear();
        newChildren.clear();
        childrenGene.clear();
        threadList.clear();
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
                container.network->initialise(childrenGene[i]);
                container.fitness = -1.0d;
                newChildren.append(container);
                GenericSimulation *simulation = _simulation->createConfigCopy();
                simulation->initialise(container.network);
                threadList.append(new SimulationThread(simulation));
                threadList[i]->start();
            }
            for(int i = 0; i < childrenGene.length(); ++i)
            {
                threadList[i]->wait();
                newChildren[i].fitness = threadList[i]->getFitness();
            }
            qDeleteAll(threadList);
            threadList.clear();
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

void GenericGeneticAlgorithm::survivor_selection()
{
    // Not needed because survivors are selected in create_children
}
