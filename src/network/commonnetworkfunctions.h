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

#ifndef COMMONNETWORKFUNCTIONS_H
#define COMMONNETWORKFUNCTIONS_H

#include <qnn-global.h>

namespace CommonNetworkFunctions {

// Transforms gene input to [0, scalar]
double floatFromGeneInput(qint32 gene_input, double scalar);

// Transforms gene input to [-scalar, scalar]
double weight(qint32 gene_input, double scalar);

double sigmoid(double d);
double calculate_distance(double x_source, double y_source, double x_target, double y_target);
bool areNodesConnected(double x_source, double y_source, double x_target, double y_target, double radius, double angularExtend, double orientation);
double cut01(double d);
}

#endif // COMMONNETWORKFUNCTIONS_H
