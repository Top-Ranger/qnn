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

class QNNSHARED_EXPORT ContinuousTimeRecurrenNeuralNetwork : public AbstractNeuralNetwork
{
public:
    static double standard_activision_function(double input);

    struct config {
        qint32 size_network;
        bool size_changing;
        qint32 max_size_network;
        qint32 max_time_constant;
        qint32 weight_scalar;
        qint32 bias_scalar;
        qint32 network_default_size_grow;
        double (*activision_function)(double);

        config() :
            size_network(-1),
            size_changing(false),
            max_size_network(-1),
            max_time_constant(5),
            weight_scalar(5),
            bias_scalar(5),
            network_default_size_grow(7),
            activision_function(&standard_activision_function)
        {
        }
    };

    ContinuousTimeRecurrenNeuralNetwork(qint32 len_input, qint32 len_output, config config = config());
    ~ContinuousTimeRecurrenNeuralNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ContinuousTimeRecurrenNeuralNetwork();

    enum CTRNNgene_positions {gene_bias = 0,
                              gene_input = 1,
                              gene_time_constraqint32 = 2,
                              gene_W_start = 3};

    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(qint32 i);

    bool _saveNetworkConfig(QXmlStreamWriter *stream);

private:
    config _config;
    double *_network;
};

#endif // CONTINUOUSTIMERECURRENNEURALNETWORK_H
