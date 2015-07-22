#ifndef COMMONNETWORKFUNCTIONS_H
#define COMMONNETWORKFUNCTIONS_H

namespace CommonNetworkFunctions {

// Transforms gene input to [0, scalar]
double floatFromGeneInput(int gene_input, double scalar);

// Transforms gene input to [-scalar, scalar]
double weight(int gene_input, double scalar);

double sigmoid(double d);
}

#endif // COMMONNETWORKFUNCTIONS_H
