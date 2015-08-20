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

#include "networktoxml.h"

#include <QDebug>

namespace NetworkToXML {

/*!
 * \brief Writes a single XML element
 * \param key The key of the element
 * \param value The value of the element
 * \param stream The stream to write the element to
 */
void writeSingleElement(QString key, QVariant value, QXmlStreamWriter *stream)
{
    if(stream == NULL)
    {
        return;
    }

    switch(value.type())
    {
    case QVariant::Invalid:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Invalid QVariant at '" << key <<"' - Skipping";
        return;
        break;
    case QVariant::Bool:
        stream->writeStartElement("bool");
        break;
    case QVariant::Double:
        stream->writeStartElement("double");
        break;
    case QVariant::Int:
        stream->writeStartElement("qint32");
        break;
    case QVariant::String:
        stream->writeStartElement("QString");
        break;
    default:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown QVariant type" << value.type();
        stream->writeStartElement("unknown");
        break;
    }

    stream->writeAttribute("key", key);
    stream->writeAttribute("value", value.toString());
    stream->writeEndElement();
}

void writeConfigStart(QString type, QMap<QString, QVariant> config, QXmlStreamWriter *stream)
{
    if(stream == NULL)
    {
        return;
    }

    stream->setAutoFormatting(true);

    stream->writeStartDocument();
    stream->writeStartElement("network");
    stream->writeAttribute("type", type);

    stream->writeStartElement("config");
    foreach(QString s, config.keys())
    {
        writeSingleElement(s, config[s], stream);
    }
    stream->writeEndElement(); // config
}

void writeConfigNeuron(qint32 id, QMap<QString, QVariant> config, QMap<qint32, double> connections, QXmlStreamWriter *stream)
{
    if(stream == NULL)
    {
        return;
    }

    stream->writeStartElement("neuron");
    stream->writeAttribute("id", QString("%1").arg(id));

    foreach(QString s, config.keys())
    {
        writeSingleElement(s, config[s], stream);
    }

    stream->writeStartElement("input_connections");
    foreach (qint32 i, connections.keys())
    {
        stream->writeStartElement("connection");
        stream->writeAttribute("target", QString("%1").arg(i));
        stream->writeAttribute("weight", QString("%1").arg(connections[i]));
        stream->writeEndElement(); // connection
    }
    stream->writeEndElement(); // input_connections

    stream->writeEndElement(); // neuron
}

void writeConfigEnd(QXmlStreamWriter *stream)
{
    if(stream == NULL)
    {
        return;
    }
    stream->writeEndElement(); // network
    stream->writeEndDocument();
}
}
