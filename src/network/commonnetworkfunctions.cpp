#include <QtCore/qmath.h>

namespace CommonNetworkFunctions {
double floatFromGeneInput(int gene_input, double scalar)
{
    return (double) gene_input/RAND_MAX * scalar;
}

double weight(int gene_input, double scalar)
{
    return (((double) gene_input - RAND_MAX/2.0) * 2.0 / RAND_MAX) * scalar;
}

double sigmoid(double d)
{
    return 1.0d / (1.0d + qExp(-1.0d * d));
}
}
