#ifndef COMMONNETWORKFUNCTIONS_H
#define COMMONNETWORKFUNCTIONS_H

namespace CommonNetworkFunctions {

// Transforms gene input to [0, scalar]
double floatFromGeneInput(int gene_input, double scalar);

// Transforms gene input to [-scalar, scalar]
double weight(int gene_input, double scalar);

double sigmoid(double d);
double calculate_distance(double x_source, double y_source, double x_target, double y_target);
bool areNodesConnected(double x_source, double y_source, double x_target, double y_target, double radius, double angularExtend, double orientation);
double cut01(double d);
}

#endif // COMMONNETWORKFUNCTIONS_H
