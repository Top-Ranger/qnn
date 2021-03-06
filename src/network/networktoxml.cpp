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

namespace NetworkToXML {
void writeSingleElement(QString key, QVariant value, QXmlStreamWriter *stream)
{
    if(Q_UNLIKELY(stream == NULL))
    {
        QNN_WARNING_MSG("stream is NULL");
        return;
    }

    switch(value.type())
    {
    case QVariant::Invalid:
        QNN_WARNING_MSG("Invalid QVariant");
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
        QNN_WARNING_MSG("Unknown QVariant type");
        stream->writeStartElement("unknown");
        break;
    }

    stream->writeAttribute("key", key);
    stream->writeAttribute("value", value.toString());
    stream->writeEndElement();
}

void writeConfigStart(QString type, QMap<QString, QVariant> config, QXmlStreamWriter *stream)
{
    if(Q_UNLIKELY(stream == NULL))
    {
        QNN_WARNING_MSG("stream is NULL");
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
    if(Q_UNLIKELY(stream == NULL))
    {
        QNN_WARNING_MSG("stream is NULL");
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
    if(Q_UNLIKELY(stream == NULL))
    {
        QNN_WARNING_MSG("stream is NULL");
        return;
    }
    stream->writeEndElement(); // network
    stream->writeEndDocument();
}
}
