#ifndef GASNET_H
#define GASNET_H

#include <qnn-global.h>

#include "abstractneuralnetwork.h"

class QNNSHARED_EXPORT GasNet : public AbstractNeuralNetwork
{
public:
    struct GasNet_config {
        double area_size;
        double gas_threshhold;
        double electric_threshhold;
        double cone_ratio;
        double offset_gas_radius;
        double range_gas_radius;

        GasNet_config() :
            area_size(1.0d),
            gas_threshhold(0.1d),
            electric_threshhold(0.1d),
            cone_ratio(0.5d),
            offset_gas_radius(0.1d),
            range_gas_radius(0.5d)
        {
        }
    };

    GasNet(int len_input, int len_output, GasNet_config config = GasNet_config());
    ~GasNet();

    GenericGene *getRandomGene();
    AbstractNeuralNetwork *createConfigCopy();

protected:
    GasNet();

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
    void _initialise();
    void _processInput(QList<double> input);
    double _getNeuronOutput(int i);

    GasNet_config _config;
    double *_network;
    double *_gas_emitting;
    QList<double> _P;
};

#endif // GASNET_H
