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

#ifndef ABSTRACTNEURALNETWORK_H
#define ABSTRACTNEURALNETWORK_H

#include <qnn-global.h>

#include "genericgene.h"

#include <QList>
#include <QXmlStreamWriter>

/*!
 * \brief The AbstractNeuralNetwork class is a virtual class for all neural network.
 *
 * A neural network should be solely defined by the type, its configuration and the gene. Each type of neural network should
 * be implemented in an own subclass of this class.
 *
 * A neural network starts in an unitialised state. Most methods
 * (except AbstractNeuralNetwork::getRandomGene, AbstractNeuralNetwork::createConfigCopy and AbstractNeuralNetwork::initialise) won't work
 * until the network is initialised by calling AbstractNeuralNetwork::initialise(GenericGene *gene).
 *
 * All virtual methods have to be implemented by each subclass.
 */
class QNNSHARED_EXPORT AbstractNeuralNetwork
{
public:
    /*!
     * \brief Basic constructor.
     * \param len_input Length of the input
     * \param len_output Length of the output
     */
    AbstractNeuralNetwork(qint32 len_input, qint32 len_output);

    /*!
     * \brief Deconstructor of AbstractNeuralNetwork
     */
    virtual ~AbstractNeuralNetwork();

    /*!
     * \brief Initialises the network.
     *
     * The network will save a deep copy of the gene so the caller can delete the gene at any time after the initialisation.
     *
     * \param gene Gene of the network. The caller must delete the gene
     */
    void initialise(GenericGene *gene);

    /*!
     * \brief Processes the input in the neural network.
     *
     * The input is a list of doubles which must have the length 'len_input'. The result of the processing can be retrieved using getNeuronOutput.
     *
     * \param input Input to process
     */
    void processInput(QList<double> input);

    /*!
     * \brief Gets the output of the last 'processInput' call.
     *
     * The output is in the range [-1,1] but implementations may only use a subset of the range (e.g. [0,1])
     *
     * \param i Number of neuron (0 <= i < len_output)
     * \return Output of neuron
     */
    double getNeuronOutput(qint32 i);

    /*!
     * \brief Saves the internal configuration of the network.
     *
     * The network configuration is saved into XML file format.
     *
     * \param device The QIODevice to save the configuration to
     * \return  True if the saving was successful
     */
    bool saveNetworkConfig(QIODevice *device);

    /*!
     * \brief Returns a random gene which may be used with the current network configuration
     * \return Random gene. The caller must delete the gene
     */
    virtual GenericGene *getRandomGene() = 0;

    /*!
     * \brief Creates a uninitialised copy of the network
     * \return Copy of the network. The caller must delete the gene
     */
    virtual AbstractNeuralNetwork *createConfigCopy() = 0;

protected:
    /*!
     * \brief Empty constructor
     *
     * This constructor may be useful for subclasses
     */
    AbstractNeuralNetwork();

    /*!
     * \brief Initialises the network
     *
     * This method is the method where subclasses should implement their initialisation.
     * _gene is guaranteed to be valid.
     */
    virtual void _initialise() = 0;

    /*!
     * \brief Processes the input in the neural network.
     *
     * This method is the method where subclasses should implement their processing.
     * _gene is guaranteed to be valid.
     *
     * \param input Input to process
     */
    virtual void _processInput(QList<double> input) = 0;

    /*!
     * \brief Gets the output of the last 'processInput' call.
     *
     * This method is the method where subclasses should implement their output method.
     * _gene is guaranteed to be valid.
     *
     * \param i Number of neuron (0 <= i < len_output)
     * \return Output of neuron i
     */
    virtual double _getNeuronOutput(qint32 i) = 0;

    /*!
     * \brief  Saves the internal configuration of the network.
     *
     * This method is the method where subclasses should implement their saving.
     * _gene is guaranteed to be valid and the network is guaranteed to be initialised.
     *
     * \param stream Stream to save config to. Stream is guaranteed to be a valid pointer
     * \return True if save is successfull
     */
    virtual bool _saveNetworkConfig(QXmlStreamWriter *stream) = 0;

    /*!
     * \brief Contains the input lengt.
     */
    qint32 _len_input;

    /*!
     * \brief Contains the output length.
     */
    qint32 _len_output;

    /*!
     * \brief Contains a deep copy of the gene.
     */
    GenericGene *_gene;
};

#endif // ABSTRACTNEURALNETWORK_H
