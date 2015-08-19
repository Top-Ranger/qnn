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

#if defined(QNN_LIBRARY)
#  define QNNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QNNSHARED_EXPORT Q_DECL_IMPORT
#endif

static const int MAX_GENE_VALUE = 0x7FFFFFFF; // Should be MAX_INT on 32 bit

#endif // QNN_GLOBAL_H
