#include "cuckoosearch.h"

#include <math.h>
#include <QtCore/qmath.h>
#include <QtConcurrentRun>
#include <QFuture>
#include <QtAlgorithms>
#include <random>

namespace {

static const qint32 MAX_FORWARD_RANDOM = 256;

double runSimulation(GenericSimulation *simulation, AbstractNeuralNetwork *network, GenericGene *gene, qint32 rand_seed, qint32 forward_random)
{
    // We need to seed RNG on each thread to get different random values in the simulations
    qsrand(rand_seed);

    // Advance random a bit for better randomness in the simulations
    for(qint32 i = 0; i < forward_random; ++i)
    {
        qrand();
    }

    // Run simulation
    double score;
    GenericSimulation *newSimulation = simulation->createConfigCopy();
    newSimulation->initialise(network, gene);
    score = newSimulation->getScore();
    delete newSimulation;
    return score;
}
}

CuckooSearch::CuckooSearch(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size, double fitness_to_reach, qint32 max_rounds, config config, QObject *parent) :
    GenericGeneticAlgorithm(network, simulation, population_size, fitness_to_reach, max_rounds, parent),
    _config(config)
{
}

CuckooSearch::~CuckooSearch()
{
}

CuckooSearch::CuckooSearch(config config, QObject *parent) :
    GenericGeneticAlgorithm(parent),
    _config(config)
{
}

void CuckooSearch::createChildren()
{
    // Create new eggs
    QList< QFuture<GeneContainer *> > newEggs;
    for(qint32 i = 0; i < _population_size; ++i)
    {
        newEggs.append(QtConcurrent::run(this, &CuckooSearch::performLevyFlight, &_population[i], _simulation));
    }

    // Replace eggs
    for(qint32 i = 0; i < _population_size; ++i)
    {
        GeneContainer *egg = newEggs[i].result();
        qint32 chosenNest = qrand()%_population_size;
        if(egg->fitness > _population[chosenNest].fitness)
        {
            // Replace egg
            delete _population[chosenNest].network;
            delete _population[chosenNest].gene;
            _population[chosenNest] = *egg;
        }
        else
        {
            // Do not replace egg
            delete egg->gene;
            delete egg->network;
        }
        delete egg;
    }
}

void CuckooSearch::survivorSelection()
{
    qSort(_population);

    // Replace random nests
    qint32 numberNests = _population_size * _config.abandoned_nests;

    for(qint32 i = 0; i < numberNests; ++i)
    {
        GeneContainer container = _population.takeFirst();
        delete container.network;
        delete container.gene;
    }

    QList< QFuture<double> > futureList;
    QList<GeneContainer> nestList;
    for(qint32 i = 0; i < numberNests; ++i)
    {
        GeneContainer nest;
        nest.network = _network->createConfigCopy();
        nest.gene = _network->getRandomGene();
        nest.fitness = -1.0;
        nestList.append(nest);
        futureList.append(QtConcurrent::run(runSimulation, _simulation, nest.network, nest.gene, qrand(), qrand()%MAX_FORWARD_RANDOM));
    }
    for(qint32 i = 0; i < numberNests; ++i)
    {
        nestList[i].fitness = futureList[i].result();
    }
    _population.append(nestList);
}

GenericGeneticAlgorithm::GeneContainer *CuckooSearch::performLevyFlight(GenericGeneticAlgorithm::GeneContainer *cuckoo, GenericSimulation *simulation)
{
    std::random_device rd;
    std::normal_distribution<double> nd;

    qsrand((double) MAX_GENE_VALUE * (double) rd() / (double) rd.max());

    // Create new egg
    GeneContainer *newEgg = new GeneContainer;
    newEgg->fitness = -1.0;
    newEgg->network = cuckoo->network->createConfigCopy();
    GenericGene *newGene = cuckoo->gene->createCopy();

    // Create new gene using Levy flight
    for(int segment = 0; segment < newGene->segments().size(); ++segment)
    {
        for(int i = 0; i < newGene->segments()[segment].size(); ++i)
        {
            double u = nd(rd) * levy_sigma;
            double v = nd(rd);
            double stepsize = 0.01 * MAX_GENE_VALUE * u/qPow(qAbs(v),(1/levy_beta));
            qint64 newValue = qAbs((double) newGene->segments()[segment][i] + stepsize * nd(rd));
            if(Q_UNLIKELY(newValue < 0))
            {
                newValue = 0;
            }
            else if(Q_UNLIKELY(newValue > MAX_GENE_VALUE))
            {
                newValue = MAX_GENE_VALUE;
            }
            newGene->segments()[segment][i] = newValue;
        }
    }
    newEgg->gene = newGene;

    // Calculate fitness
    GenericSimulation *newSimulation = simulation->createConfigCopy();
    newSimulation->initialise(newEgg->network, newEgg->gene);
    newEgg->fitness = newSimulation->getScore();
    delete newSimulation;
    return newEgg;
}
