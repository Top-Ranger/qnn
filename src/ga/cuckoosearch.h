#ifndef CUCKOOSEARCH_H
#define CUCKOOSEARCH_H

#include <qnn-global.h>

#include "genericgeneticalgorithm.h"

/*!
 * \brief This class implements the cuckoo search with Lévy flights.
 *
 * The cuckoo search was inspired by the breeding behaviour of various cuckoo species. For more information see:
 *
 * Xin-She Yang and Suash Deb. 2009. Cuckoo Search via Lévy flights. In Nature Biologically Inspired Computing, 2009. NaBIC 2009. World Congress on. 210–214. DOI: http://dx.doi.org/10.1109/NABIC.2009.5393690
 *
 * Xin-She Yang and Suash Deb. 2010. Engineering Optimisation by Cuckoo Search. ArXiv e-prints (May 2010).
 */
class QNNSHARED_EXPORT CuckooSearch : public GenericGeneticAlgorithm
{
public:
    struct config {
        double abandoned_nests;

        config() :
            abandoned_nests(0.2)
        {
        }
    };

    CuckooSearch(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size = 300, double fitness_to_reach = 0.99, qint32 max_rounds = 200, config config = config(), QObject *parent = 0);
    virtual ~CuckooSearch();

protected:
    CuckooSearch(config config = config(), QObject *parent = 0);
    void createChildren();
    void survivorSelection();
    GeneContainer *performLevyFlight(GeneContainer *cuckoo, GenericSimulation *simulation);

    config _config;

private:
    static constexpr double levy_beta = 3.0/2.0;
    static constexpr double levy_sigma = pow((tgamma(1.0+levy_beta)*sin(M_PI*levy_beta/2.0)/(tgamma((1.0+levy_beta)/2.0)*levy_beta*exp2((levy_beta-1.0)/2.0))),(1.0/levy_beta));
};

#endif // CUCKOOSEARCH_H
