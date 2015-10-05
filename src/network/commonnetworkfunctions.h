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

/*!
 * \brief This namespace contains some functions which are commonly used by networks.
 */
namespace CommonNetworkFunctions {

/*!
 * \brief Transforms an integer from a gene to a double in the range [0, scalar]
 * \param gene_input Integer to transform
 * \param scalar Scalar of the output
 * \return Double [0, scalar]
 */
double floatFromGeneInput(qint32 gene_input, double scalar);

/*!
 * \brief Transforms an integer from a gene to a double in the range [-scalar, scalar]
 * \param gene_input Integer to transform
 * \param scalar Scalar of the output
 * \return Double [-scalar, scalar]
 */
double weight(qint32 gene_input, double scalar);

/*!
 * \brief Sigmoid function
 *
 * The sigmoid function is defined as 1/(1+e^(-d))
 *
 * \param d Input double
 * \return Sigmoid of input
 */
double sigmoid(double d);

/*!
 * \brief Calculates the euclidean distance between to points
 * \param x_source X coordinates of source
 * \param y_source Y coordinates of source
 * \param x_target X coordinates of target
 * \param y_target Y coordinates of target
 * \return Euclidean distance
 */
double calculateDistance(double x_source, double y_source, double x_target, double y_target);

/*!
 * \brief Calculates if a node is located in a cone
 *
 * More information: Phil Husbands, Tom Smith, Nick Jakobi, and Michael O’Shea. Better Living Through Chemistry: Evolving GasNets for Robot Control. 1998.
 *
 * \param x_source X coordinates of source
 * \param y_source Y coordinates of source
 * \param x_target X coordinates of target
 * \param y_target Y coordinates of target
 * \param radius Radius of the cone
 * \param angularExtend Angular extend of the cone
 * \param orientation Orientation of the cone
 * \return True if nodes are connected
 */
bool areNodesConnected(double x_source, double y_source, double x_target, double y_target, double radius, double angularExtend, double orientation);

/*!
 * \brief Cuts a value to [0,1]
 *
 * Returns 0 if d<0, 1 if d>1, else d
 *
 * \param d Value to cut
 * \return Cutted value
 */
double cut01(double d);
}

#endif // COMMONNETWORKFUNCTIONS_H
