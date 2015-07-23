#include "commonnetworkfunctions.h"

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

double calculate_distance(double x_source, double y_source, double x_target, double y_target)
{
    double difference_x = x_source - x_target;
    double difference_y = y_source - y_target;
    return qSqrt((difference_x * difference_x) + (difference_y * difference_y));
}

bool areNodesConnected(double x_source, double y_source, double x_target, double y_target, double radius, double angularExtend, double orientation)
{
    if(x_source == x_target && y_source == y_target)
    {
        return false;
    }

    double difference_x = x_source - x_target;
    double difference_y = y_source - y_target;
    double distance = calculate_distance(x_source, y_source, x_target, y_target);

    if(distance > radius)
    {
        return false;
    }

    double angleCone = qAsin(difference_x / distance);

    if(difference_y < 0.0d)
    {
        angleCone = M_PI - angleCone;
    }
    else if (angleCone < 0.0d)
    {
        angleCone = 2.0d * M_PI + angleCone;
    }

    angleCone -= angularExtend;

    if (angleCone < 0.0d)
    {
        angleCone = 2.0d * M_PI + angleCone;
    }

    return angleCone < orientation;
}

double cut01(double d)
{
    if(d > 1.0d)
    {
        return 1.0d;
    }
    else if(d < 0.0d)
    {
        return 0.0d;
    }
    return d;
}
}
