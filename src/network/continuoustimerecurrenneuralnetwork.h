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

#ifndef CONTINUOUSTIMERECURRENNEURALNETWORK_H
#define CONTINUOUSTIMERECURRENNEURALNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

/*!
 * \brief The ContinuousTimeRecurrenNeuralNetwork class implements a continuous-time recurren neural network.
 *
 * The CTRNN is implemented as defined in:
 * Beer Randall D. On the dynamics of small continuous-time recurrent neural networks, volume 03 of Adaptive Behavior, pages 469–509. 1995.
 */
class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    /*!
     * \brief The standard activision function of CTRNN
     *
     * This function is the sigmoid function
     *
     * \param input Input value
     * \return Sigmoid of input value
     */
    static double standard_activision_function(double input);

    /*!
     * \brief This struct contains all configuration option of CTRNNs
     */
    struct config {
        /*!
         * \brief size_network is the size of the network.
         *
         * If this value is set to -1 the size will be calculated at creation time.
         * If size_changing is set to true this is the minimum size.
         */
        qint32 size_network;
        /*!
         * \brief If size_changing is true the network will change it size in the genetic algorithm
         */
        bool size_changing;
        /*!
         * \brief max_size_network describes maximum size of the network.
         *
         * If this value is set to -1 the maximum size will be calculated at creation time.
         * If size_changing is set to false this will do nothing.
         */
        qint32 max_size_network;
        /*!
         * \brief max_time_constant is the maximum time of the network
         */
        qint32 max_time_constant;
        /*!
         * \brief weight_scalar sets the scalar for the weight.
         *
         * The weight will be in the range [-weight_scalar, weight_scalar]
         */
        qint32 weight_scalar;
        /*!
         * \brief bias_scalar sets the scalar for the bias.
         *
         * The bias will be in the range [-bias_scalar, bias_scalar]
         */
        qint32 bias_scalar;
        /*!
         * \brief network_default_size_grow sets the step size in which the network will grow if size_network is set to -1.
         */
        qint32 network_default_size_grow;
        /*!
         * \brief activision_function holds the activation function used by the CTRNN
         */
        double (*activision_function)(double);

        /*!
         * \brief If neuron_save is not NULL the value of the neurons will be saved to the QIODevice
         *
         * You should not set this to a value != NULL if multiple networks are created
         * (e.g. running genetic algorithm or  running simulations with multiple trials)
         * using AbstractNeuralNetwork::createConfigCopy()
         * as it might lead to race conditions and multiple runs in one file.
         */
        QIODevice *neuron_save;

        /*!
         * \brief This variable saves whether we opened the device. It does not need to be set and will be overwritten.
         */
        bool neuron_save_opened;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            size_network(-1),
            size_changing(false),
            max_size_network(-1),
            max_time_constant(5),
            weight_scalar(5),
            bias_scalar(5),
            network_default_size_grow(7),
            activision_function(&standard_activision_function),
            neuron_save(NULL),
            neuron_save_opened(false)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the CTRNN
     */
    ContinuousTimeRecurrenNeuralNetwork(qint32 len_input, qint32 len_output, config config = config());

    /*!
     * \brief Destructor
     */
    ~ContinuousTimeRecurrenNeuralNetwork();

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
     * \brief Empty constructor
     *
     * This constructor may be useful for subclasses
     */
    ContinuousTimeRecurrenNeuralNetwork();

    /*!
     * \brief This enum contains the gene position of the different values.
     */
    enum CTRNNgene_positions {gene_bias = 0,
                              gene_input = 1,
                              gene_time_constraint = 2,
                              gene_W_start = 3};

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
     * \brief Configuration of the CTRNN
     */
    config _config;

    /*!
     * \brief This pointer containes the network
     */
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
