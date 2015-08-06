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

#ifndef NETWORKTOXML_H
#define NETWORKTOXML_H

#include <QString>
#include <QVariant>
#include <QMap>
#include <QXmlStreamWriter>

namespace NetworkToXML {
void writeConfigStart(QString type, QMap<QString, QVariant> config, QXmlStreamWriter *stream);
void writeConfigNeuron(int id, QMap<QString, QVariant> config, QMap<int, double> connections, QXmlStreamWriter *stream);
void writeConfigEnd(QXmlStreamWriter *stream);
}

#endif // NETWORKTOXML_H
