#ifndef NONPARALLELCUCKOOSEARCH_H
#define NONPARALLELCUCKOOSEARCH_H

#include <qnn-global.h>

#include "genericgeneticalgorithm.h"
#include "nonparallelgenericgeneticalgorithm.h"

/*!
 * \brief This class implements a non-parallel version of the cuckoo search with Lévy flights.
 *
 * The cuckoo search was inspired by the breeding behaviour of various cuckoo species. For more information see:
 *
 * Xin-She Yang and Suash Deb. 2009. Cuckoo Search via Lévy flights. In Nature Biologically Inspired Computing, 2009. NaBIC 2009. World Congress on. 210–214. DOI: http://dx.doi.org/10.1109/NABIC.2009.5393690
 *
 * Xin-She Yang and Suash Deb. 2010. Engineering Optimisation by Cuckoo Search. ArXiv e-prints (May 2010).
 */
class QNNSHARED_EXPORT NonParallelCuckooSearch : public NonParallelGenericGeneticAlgorithm
{
public:
    /*!
     * \brief This struct contains all configuration option of Cuckoo search
     */
    struct config {
        /*!
         * \brief abandoned_nests holds the percent of nests which got abandoned in each generation
         *
         *  The value must be in the range [0.0, 1.0]
         */
        double abandoned_nests;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            abandoned_nests(0.1)
        {
        }
    };

    /*!
     * Constructor of NonParallelCuckooSearch
     *
     * \brief Constructor of NonParallelCuckooSearch
     * \param network The network which should be optimised. Might not be NULL
     * \param simulation The simulation for which the network should be optimised. Might not be NULL
     * \param population_size The population size
     * \param fitness_to_reach The fitness which should be reached. Once it has been reached the cuckoo search will finish
     * \param max_rounds The maximum amount of rounds. The cuckoo search will abort after the amount of rounds
     * \param config Configuration for this cuckoo search
     * \param parent The parent of the object
     */
    NonParallelCuckooSearch(AbstractNeuralNetwork *network, GenericSimulation *simulation, qint32 population_size = 300, double fitness_to_reach = 0.99, qint32 max_rounds = 200, config config = config(), QObject *parent = 0);

    /*!
     * \brief Deconstructor
     */
    virtual ~NonParallelCuckooSearch();

protected:
    /*!
     * \brief Empty constructor
     *
     * This constructor may be useful for subclasses
     */
    NonParallelCuckooSearch(config config = config(), QObject *parent = 0);

    /*!
     * \brief In this function the new population is build. This is an overwritten function.
     *
     * The building of the new population consists of performing Lévy flights and replacing eggs
     */
    void createChildren();

    /*!
     * \brief In this function the survivors are selected. This is an overwritten function.
     *
     * The survivor selection consists of abandoning the worst nests and building new random solutions
     */
    void survivorSelection();

    /*!
     * \brief This function performs the Lévy flight for a single solution (cuckoo).
     * \param cuckoo The initial solution
     * \param simulation The simulation from which the fitness is calculated
     * \return Pointer to GenericGeneticAlgorithm::GeneContainer. The caller must delete the container as well as the network / gene in the container
     */
    GeneContainer *performLevyFlight(GeneContainer cuckoo, GenericSimulation *simulation);

    /*!
     * \brief Configuration of the cuckoo search
     */
    config _config;

private:
    /*!
     * \brief Alpha value (typical step size) of Lévy flight
     *
     *  In QNN the maximum size for a gene value is MAX_GENE_VALUE; Because a typical stepsize should be L/100 we use MAX_GENE_VALUE/100.
     */
    static constexpr double levy_alpha = 0.01 * MAX_GENE_VALUE;

    /*!
     * \brief levy_beta contains the beta value used in the Lévy flight
     *
     * For more information see Xin-She Yang and Suash Deb. 2010. Engineering Optimisation by Cuckoo Search. ArXiv e-prints (May 2010).
     */
    static constexpr double levy_beta = 3.0/2.0;

    /*!
     * \brief levy_beta contains the beta value used in the Lévy flight
     *
     * For more information see Xin-She Yang and Suash Deb. 2010. Engineering Optimisation by Cuckoo Search. ArXiv e-prints (May 2010).
     */
    static constexpr double levy_sigma = pow((tgamma(1.0+levy_beta)*sin(M_PI*levy_beta/2.0)/(tgamma((1.0+levy_beta)/2.0)*levy_beta*exp2((levy_beta-1.0)/2.0))),(1.0/levy_beta));
};

#endif // NONPARALLELCUCKOOSEARCH_H
