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

#ifndef QNN_GLOBAL_H
#define QNN_GLOBAL_H

#include <QtCore/qglobal.h>

/*!
 * \mainpage QNeuralNetwork documentation
 *
 * QNeuralNetwork (qnn) is a simple to use, customisable and efficient library for neural network. QNeuralNetwork is written in C++/Qt.
 *
 * QNeuralNetwork uses an easy internal model. A network is solely defined through its type, its configuration and its gene.
 * A gene contains a number of segments, each segment is a list of numbers.
 *
 * A network can be trained for a task using a genetic algorithm. The task is contained in a simulation.
 *
 * QNeuralNetwork is licensed under the terms of the GNU Lesser General Public License Version 3 or (at your option) any later version.
 */

/*!
  * \brief This macro will ensure import / export of symbols will work correctly on all platforms
  */
#if defined(QNN_LIBRARY)
#  define QNNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QNNSHARED_EXPORT Q_DECL_IMPORT
#endif

/*!
 * \brief MAX_GENE_VALUE contains the maximum value in a gene segment.
 */
static const qint32 MAX_GENE_VALUE = 0x7FFFFFFF; // Should be MAX_INT on 32 bit

#endif // QNN_GLOBAL_H
