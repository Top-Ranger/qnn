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

/*!
 * \brief This namespace contains some function which should make it easy to save a network configuration as XML
 */
namespace NetworkToXML {
/*!
 * \brief Writes the beginning of a configuration.
 *
 * This method should only be called once per file.
 *
 * \param type The type of the network
 * \param config A map containing the configuration of the network in the form QMap<key, value>
 * \param stream The QXmlStreamWriter to which write the output
 */
void writeConfigStart(QString type, QMap<QString, QVariant> config, QXmlStreamWriter *stream);

/*!
 * \brief Writes a single neuron into the XML file.
 *
 * This method should be called after writeConfigStart and before writeConfigEnd.
 *
 * \param id The id of the neuron. Each neuron should have a unique id
 * \param config A map containing the configuration of the neuron in the form QMap<key, value>
 * \param connections A map containing all incoming connections in the form QMap<source neuron id, weight of connection>
 * \param stream The QXmlStreamWriter to which write the output
 */
void writeConfigNeuron(qint32 id, QMap<QString, QVariant> config, QMap<qint32, double> connections, QXmlStreamWriter *stream);

/*!
 * \brief Completes the XML file.
 *
 * This method should only be called once per file. After this method is called there should be no other writes to the XML file.
 *
 * \param stream The QXmlStreamWriter to which write the output
 */
void writeConfigEnd(QXmlStreamWriter *stream);
}

#endif // NETWORKTOXML_H
