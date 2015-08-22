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

#ifndef FEEDFORWARDNETWORK_H
#define FEEDFORWARDNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

/*!
 * \brief The FeedForwardNetwork class represents a multi-layer feed forward network (multi-layer perceptron).
 *
 * This class may eighter have no hidden layers or a number of same-sized hidden layer.
 */
class QNNSHARED_EXPORT FeedForwardNetwork : public AbstractNeuralNetwork
{
public:
    /*!
     * \brief Calculates the number of segments needed for the given network configuration
     * \param len_input Size of input layer
     * \param len_output Size of output layer
     * \param hidden_layer Size of hidden layer
     * \param len_hidden Number of hidden layer
     * \return Number of required segments
     */
    static qint32 num_segments(qint32 len_input, qint32 len_output, qint32 hidden_layer, qint32 len_hidden);

    /*!
     * \brief The standard activision function of FFN
     *
     * This function is the sigmoid function
     *
     * \param input Input value
     * \return Sigmoid of input value
     */
    static double standard_activision_function(double input);

    /*!
     * \brief This struct contains all configuration option of FFNs
     */
    struct config {
        /*!
         * \brief num_hidden_layer holds the number of hidden layer
         */
        qint32 num_hidden_layer;
        /*!
         * \brief len_hidden holds the size of all hidden layer
         */
        qint32 len_hidden;
        /*!
         * \brief activision_function holds the activation function used by the FFN
         */
        double (*activision_function)(double);
        /*!
         * \brief weight_scalar sets the scalar for the weight.
         *
         * The weight will be in the range [-weight_scalar, weight_scalar]
         */
        double weight_scalar;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            num_hidden_layer(2),
            len_hidden(5),
            activision_function(&standard_activision_function),
            weight_scalar(1.0d)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the FFN
     */
    FeedForwardNetwork(qint32 len_input, qint32 len_output, config config = config());

    /*!
     * \brief Destructor
     */
    ~FeedForwardNetwork();

    /*!
     * \brief Returns a random gene which may be used with the current network configuration
     * \return Random gene. The caller must delete the gene
     */
    GenericGene *getRandomGene();

    /*!
     * \brief Creates a uninitialised copy of the network
     * \return Copy of the network. The caller must delete the gene
     */
    AbstractNeuralNetwork *createConfigCopy();

protected:
    /*!
     * \brief Overwritten function to initialise the network.
     */
    void _initialise();

    /*!
     * \brief Overwritten method to process input
     * \param input Input to process
     */
    void _processInput(QList<double> input);

    /*!
     * \brief Overwritten function to get output
     * \param i Number of neuron (0 <= i < len_output)
     * \return Output of neuron i
     */
    double _getNeuronOutput(qint32 i);

    /*!
     * \brief Overwritten function to save network config
     * \param stream Stream to save config to. Stream is guaranteed to be a valid pointer
     * \return True if save is successfull
     */
    bool _saveNetworkConfig(QXmlStreamWriter *stream);

private:
    /*!
     * \brief Empty constructor.
     *
     * FeedForwardNetwork may not be used empty
     */
    FeedForwardNetwork();

    /*!
     * \brief Configuration of the FFN
     */
    config _config;

    /*!
     * \brief Contains the hidden layers
     */
    double **_hidden_layers;

    /*!
     * \brief Contains the output layer
     */
    double *_output;
};

#endif // FEEDFORWARDNETWORK_H
