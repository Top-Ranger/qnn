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

#ifndef MODULATEDSPIKINGNEURONSNETWORK_H
#define MODULATEDSPIKINGNEURONSNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

/*!
 * \brief The ModulatedSpikingNeuronsNetwork class represents a modulated spiking-neurons network, a combination of GasNets and spiking neurons.
 *
 * For more information see:
 * Stefan Bruhns. Modulated Spiking-Neurons. Bachelor thesis, University Hamburg, 2015.
 *
 * Used neuron model:
 * Eugene M. Izhikevich. Simple Model of Spiking Neurons. In IEEE Transactions on neural networks, number 6 in volume 14, pages 1569–1572, November 2003.
 */
class QNNSHARED_EXPORT ModulatedSpikingNeuronsNetwork : public AbstractNeuralNetwork
{
public:

    /*!
     * \brief This struct contains all configuration option of MSNNs
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
         * \brief a_modulated determines if variable a should be modulated
         */
        bool a_modulated;
        /*!
         * \brief b_modulated determines if variable b should be modulated
         */
        bool b_modulated;
        /*!
         * \brief c_modulated determines if variable c should be modulated
         */
        bool c_modulated;
        /*!
         * \brief d_modulated determines if variable d should be modulated
         */
        bool d_modulated;
        /*!
         * \brief timestep_size determines the size of one modulated time step
         *
         * The range should be (0,1]
         */
        double timestep_size;

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
            a_modulated(false),
            b_modulated(false),
            c_modulated(false),
            d_modulated(true),
            timestep_size(0.1)

        {
        }
    };

    /*!
     * \brief Constructor
     * \param len_input Length of the input
     * \param len_output Length of the output
     * \param config Configuration of the MSNN
     */
    ModulatedSpikingNeuronsNetwork(qint32 len_input, qint32 len_output, config config = config());

    /*!
     * \brief Destructor
     */
    ~ModulatedSpikingNeuronsNetwork();

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
    ModulatedSpikingNeuronsNetwork();

    /*!
     * \brief This method initialises all P-array with values as defined by Bruhns
     */
    void initialiseP();

    /*!
     * \brief This method initialises all token arrays and sets _emitting_possible according to the config
     */
    void initialiseTokenArrays();

    /*!
     * \brief This enum contains the gene position of the different values.
     */
    enum ModulatedSpikingNeuronsNetwork_gene_positions {gene_x = 0,
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
                                                        gene_a = 14,
                                                        gene_b = 15,
                                                        gene_c = 16,
                                                        gene_d = 17};
    /*!
     * \brief This enum contains all possible tokens for type of gas and reasons of emitting gas.
     */
    enum GasToken { ElectricCharge,
                    APositiv,
                    ANegativ,
                    BPositiv,
                    BNegativ,
                    CPositiv,
                    CNegativ,
                    DPositiv,
                    DNegativ,
                    NoGas};

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
     * \brief Holds the values for the v parameter at timestep t
     */
    double *_network;

    /*!
     * \brief Holds the strength of which the neurons are emitting at t timesteps
     */
    double *_gas_emitting;

    /*!
     * \brief Holds the values for the u parameter at timestep t
     */
    double *_u;

    /*!
     * \brief Counts the amount of times a neuron has fired
     */
    double *_firecount;

    /*!
     * \brief Holds precalculated distances
     */
    double **_distances;

    /*!
     * \brief Holds precalculated weights
     */
    double **_weights;

    /*!
     * \brief P array for a variable as defined by Bruhns
     */
    QVector<double> _Pa;

    /*!
     * \brief P array for b variable as defined by Bruhns
     */
    QVector<double> _Pb;

    /*!
     * \brief P array for c variable as defined by Bruhns
     */
    QVector<double> _Pc;

    /*!
     * \brief P array for d variable as defined by Bruhns
     */
    QVector<double> _Pd;

    /*!
     * \brief This list contains Tokens for all possible reasons for gas emitting.
     */
    QVector<GasToken> _WhenGas_list;

    /*!
     * \brief This list contains Tokens for all possible gases that cam be emitted by neurons.
     */
    QVector<GasToken> _TypeGas_list;

    /*!
     * \brief Holds if the network can emit gas at all.
     */
    bool _emitting_possible;
};

#endif // MODULATEDSPIKINGNEURONSNETWORK_H
