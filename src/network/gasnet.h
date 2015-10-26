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

#ifndef GASNET_H
#define GASNET_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

/*!
 * \brief The GasNet class represents a GasNet which is inspired by the discovery of freely floating gas in the human brain.
 *
 * For more information see:
 * Phil Husbands, Tom Smith, Nick Jakobi, and Michael O’Shea. Better Living Through Chemistry: Evolving GasNets for Robot Control. 1998.
 */
class QNNSHARED_EXPORT GasNet : public AbstractNeuralNetwork
{
public:
    /*!
     * \brief This struct contains all configuration option of GasNets
     */
    struct config {
        /*!
         * \brief area_size contains the size of the area.
         *
         * A lot of values scale with the size of the area.
         */
        double area_size;
        /*!
         * \brief bias_scalar sets the scalar for the bias.
         *
         * The bias will be in the range [-bias_scalar, bias_scalar]
         */
        double bias_scalar;
        /*!
         * \brief gas_threshhold sets the threshold for emitting gas.
         *
         * If the gas concentration of the correct gas is higher then the threshold then the neuron will emit gas.
         */
        double gas_threshhold;
        /*!
         * \brief electric_threshhold sets the threshold for emitting gas.
         *
         * If the gas electric charge of the neuron is high enough then the neuron will emit gas.
         */
        double electric_threshhold;
        /*!
         * \brief cone_ratio hold the percent of area size which will be used for the cone length.
         */
        double cone_ratio;
        /*!
         * \brief offset_gas_radius sets the minimum size of the gas radius.
         */
        double offset_gas_radius;
        /*!
         * \brief range_gas_radius sets the growth of the gas radius.
         *
         * Minimum gas radius = offset_gas_radius
         * Maximum gas radius = offset_gas_radius + range_gas_radius
         */
        double range_gas_radius;
        /*!
         * \brief offset_rate_of_gas sets the minimum value of which the gas will be emitted.
         */
        double offset_rate_of_gas;
        /*!
         * \brief range_rate_of_gas sets the growth of the value of which the gas will be emitted.
         *
         * Minimum emitting = offset_rate_of_gas
         * Maximum emitting = offset_rate_of_gas + range_rate_of_gas
         */
        double range_rate_of_gas;
        /*!
         * \brief min_size holds the minimum size of the network.
         *
         * If set to -1 the minimum size will be the output length
         */
        qint32 min_size;
        /*!
         * \brief max_size holds the maximum size of the network.
         *
         * If set to -1 the maximum size will be set to a default size
         */
        qint32 max_size;

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
         * \brief If neuron_save is not NULL the gas concentration at the neurons will be saved to the QIODevice
         *
         * You should not set this to a value != NULL if multiple networks are created
         * (e.g. running genetic algorithm or  running simulations with multiple trials)
         * using AbstractNeuralNetwork::createConfigCopy()
         * as it might lead to race conditions and multiple runs in one file.
         */
        QIODevice *gas_save;

        /*!
         * \brief This variable saves whether we opened the device. It does not need to be set and will be overwritten.
         */
        bool gas_save_opened;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            area_size(1.0),
            bias_scalar(1.0),
            gas_threshhold(0.1),
            electric_threshhold(0.5),
            cone_ratio(0.5),
            offset_gas_radius(0.1),
            range_gas_radius(0.5),
            offset_rate_of_gas(1.0),
            range_rate_of_gas(10.0),
            min_size(-1),
            max_size(-1),
            neuron_save(NULL),
            neuron_save_opened(false),
            gas_save(NULL),
            gas_save_opened(false)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the GasNet
     */
    GasNet(qint32 len_input, qint32 len_output, config config = config());

    /*!
     * \brief Destructor
     */
    ~GasNet();

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
    GasNet();

    /*!
     * \brief This enum contains the gene position of the different values.
     */
    enum GasNet_gene_positions {gene_x = 0,
                                gene_y = 1,
                                gene_PositivConeRadius = 2,
                                gene_PositivConeExt = 3,
                                gene_PositivConeOrientation = 4,
                                gene_NegativConeRadius = 5,
                                gene_NegativConeExt = 6,
                                gene_NegativConeOrientation = 7,
                                gene_input = 8,
                                gene_recurrent = 9,
                                gene_WhenGas = 10,
                                gene_TypeGas = 11,
                                gene_Rate_of_gas= 12,
                                gene_Gas_radius = 13,
                                gene_basis_index = 14,
                                gene_bias= 15};

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

    /*!
     * \brief The configuration of the network
     */
    config _config;

    /*!
     * \brief The actual network
     */
    double *_network;

    /*!
     * \brief Holds the strength of which the neurons are emitting at t timesteps
     */
    double *_gas_emitting;

    /*!
     * \brief Holds precalculated distances
     */
    double **_distances;

    /*!
     * \brief Holds precalculated weights
     */
    double **_weights;

    /*!
     * \brief P array as defined by Husbands
     */
    QList<double> _P;
};

#endif // GASNET_H
