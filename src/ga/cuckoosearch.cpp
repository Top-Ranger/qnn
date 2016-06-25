#include "cuckoosearch.h"

#include <math.h>
#include <QtCore/qmath.h>
#include <QtConcurrentRun>
#include <QFuture>
#include <QtAlgorithms>
#include <random>
#include <randomhelper.h>

namespace {

static const qint32 MAX_FORWARD_RANDOM = 256;

double runSimulation(AbstractSimulation *simulation, AbstractNeuralNetwork *network, GenericGene *gene)
{
    // Run simulation
    double score;
    AbstractSimulation *newSimulation = simulation->createConfigCopy();
    newSimulation->initialise(network, gene);
    score = newSimulation->getScore();
    delete newSimulation;
    return score;
}
}

CuckooSearch::CuckooSearch(AbstractNeuralNetwork *network, AbstractSimulation *simulation, qint32 population_size, double fitness_to_reach, qint32 max_rounds, config config, QObject *parent) :
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
    if(Q_UNLIKELY(_config.abandoned_nests < 0 || _config.abandoned_nests > 1.0))
    {
        QNN_FATAL_MSG("_config.abandoned_nests is out of bounds");
    }
}

void CuckooSearch::createChildren()
{
    // Create new eggs
    QList< QFuture<GeneContainer *> > newEggs;
    for(qint32 i = 0; i < _population_size; ++i)
    {
        // Create new container to prevent modification of the pointers
        GeneContainer cuckoo;
        cuckoo.fitness = _population[i].fitness;
        cuckoo.gene = _population[i].gene;
        cuckoo.network = _population[i].network;
        newEggs.append(QtConcurrent::run(this, &CuckooSearch::performLevyFlight, cuckoo, _simulation));
    }

    // Replace eggs
    // Because the genes / networks may be accessed in a parallel running performLevyFlight we have to store them for now and delete them later
    QList<GenericGene *> geneToDelete;
    QList<AbstractNeuralNetwork *> networksToDelete;
    for(qint32 i = 0; i < _population_size; ++i)
    {
        GeneContainer *egg = newEggs[i].result();
        qint32 chosenNest = RandomHelper::getRandomInt(0, _population_size-1);
        if(egg->fitness > _population[chosenNest].fitness)
        {
            // Replace egg
            // Cache genes / networks for deletion
            networksToDelete.append(_population[chosenNest].network);
            geneToDelete.append(_population[chosenNest].gene);
            _population[chosenNest] = *egg;
        }
        else
        {
            // Do not replace egg
            // Because this genes / networks are never in the population we can delete them directly
            delete egg->gene;
            delete egg->network;
        }
        delete egg;
    }
    // Now we can delete the genes / networks
    qDeleteAll(geneToDelete);
    qDeleteAll(networksToDelete);
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
        futureList.append(QtConcurrent::run(runSimulation, _simulation, nest.network, nest.gene));
    }
    for(qint32 i = 0; i < numberNests; ++i)
    {
        nestList[i].fitness = futureList[i].result();
    }
    _population.append(nestList);
}

GenericGeneticAlgorithm::GeneContainer *CuckooSearch::performLevyFlight(GenericGeneticAlgorithm::GeneContainer cuckoo, AbstractSimulation *simulation)
{
    // Create new egg
    GeneContainer *newEgg = new GeneContainer;
    newEgg->fitness = -1.0;
    newEgg->network = cuckoo.network->createConfigCopy();
    GenericGene *newGene = cuckoo.gene->createCopy();

    // Create new gene using Levy flight
    for(int segment = 0; segment < newGene->segments().size(); ++segment)
    {
        for(int i = 0; i < newGene->segments()[segment].size(); ++i)
        {
            double u = RandomHelper::getNormalDistributedDouble() * levy_sigma;
            double v = RandomHelper::getNormalDistributedDouble();
            double stepsize = levy_alpha * u/qPow(qAbs(v),(1/levy_beta));
            qint64 newValue = qAbs((double) newGene->segments()[segment][i] + stepsize * RandomHelper::getNormalDistributedDouble());
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
    AbstractSimulation *newSimulation = simulation->createConfigCopy();
    newSimulation->initialise(newEgg->network, newEgg->gene);
    newEgg->fitness = newSimulation->getScore();
    delete newSimulation;
    return newEgg;
}
