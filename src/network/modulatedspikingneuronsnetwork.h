#ifndef MODULATEDSPIKINGNEURONSNETWORK_H
#define MODULATEDSPIKINGNEURONSNETWORK_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT ModulatedSpikingNeuronsNetwork : public AbstractNeuralNetwork
{
public:
    struct config {
        double area_size;
        double bias_scalar;
        double gas_threshhold;
        double electric_threshhold;
        double cone_ratio;
        double offset_gas_radius;
        double range_gas_radius;
        double offset_rate_of_gas;
        double range_rate_of_gas;
        int min_size;
        int max_size;
        bool a_modulated;
        bool b_modulated;
        bool c_modulated;
        bool d_modulated;
        double timestep_size;

        config() :
            area_size(1.0d),
            bias_scalar(1.0d),
            gas_threshhold(0.1d),
            electric_threshhold(0.5d),
            cone_ratio(0.5d),
            offset_gas_radius(0.1d),
            range_gas_radius(0.5d),
            offset_rate_of_gas(1.0d),
            range_rate_of_gas(10.0d),
            min_size(-1),
            max_size(-1),
            a_modulated(false),
            b_modulated(false),
            c_modulated(false),
            d_modulated(true),
            timestep_size(0.1d)

        {
        }
    };

    ModulatedSpikingNeuronsNetwork(int len_input, int len_output, config config = config());
    ~ModulatedSpikingNeuronsNetwork();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    ModulatedSpikingNeuronsNetwork();

    void initialiseP();

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
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

    config _config;
    double *_network;
    double *_gas_emitting;
    double *_u;
    double *_firecount;
    double **_distances;
    double **_weights;
    QList<double> _Pa;
    QList<double> _Pb;
    QList<double> _Pc;
    QList<double> _Pd;
};

#endif // MODULATEDSPIKINGNEURONSNETWORK_H
