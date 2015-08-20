/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn.
 *
 * qnn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with qnn.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GENERICSIMULATION_H
#define GENERICSIMULATION_H

#include <qnn-global.h>

#include "../network/abstractneuralnetwork.h"
#include "../network/genericgene.h"

/*!
 * \brief The GenericSimulation class is the base class for all simulations.
 *
 * A simulation is a task which the network has to perform. This task may be performed one time or multiple times.
 * Each simulation should return a score in the range [0,1] describing how well the task was done.
 * All calculation should be done inside the getScore method.
 *
 * A simulation starts in an uninitialised state. The function getScore can only be called after the simulation has be initialised.
 *
 * A subclass of this must overwrite all virtual methods.
 */
class QNNSHARED_EXPORT GenericSimulation
{
public:
    /*!
     * \brief Constructor
     */
    GenericSimulation();

    /*!
     * \brief Deconstructior
     */
    virtual ~GenericSimulation();

    /*!
     * \brief Initialises the simulation.
     *
     * The simulation will save a deep copy of the network/gene so the caller can delete both at any time after initialisation.
     *
     * \param network The network used for the simulation. The caller has to delete the network
     * \param gene The gene defining the network to test. The caller has to delete the gene
     */
    void initialise(AbstractNeuralNetwork *network, GenericGene *gene);

    /*!
     * \brief Returns the score of the network.
     * \return Score of network
     */
    double getScore();

    /*!
     * \brief Returns the lengh of input that is used for this simulation
     * \return Input length needed
     */
    virtual qint32 needInputLength();

    /*!
     * \brief Returns the lengh of output that is used for this simulation
     * \return Output length needed
     */
    virtual qint32 needOutputLength();

    /*!
     * \brief Creates an uninitialised copy of the simulation
     * \return Copy of the simulation. The caller must delete the simulation
     */
    virtual GenericSimulation *createConfigCopy();

protected:
    /*!
     * \brief Initialises the simulation
     *
     * This method is the method where subclasses should implement their initialisation.
     * _gene and _network is guaranteed to be valid.
     */
    virtual void _initialise();

    /*!
     * \brief Initialises the network
     *
     * This method is the method where subclasses should implement their simulation.
     * _gene and _network is guaranteed to be valid.
     */
    virtual double _getScore();

    /*!
     * \brief The uninitialised network that should be tested.
     */
    AbstractNeuralNetwork *_network;

    /*!
     * \brief The gene of the network that should be tested.
     */
    GenericGene *_gene;
};

#endif // GENERICSIMULATION_H
