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

#ifndef LENGTHCHANGINGGENE_H
#define LENGTHCHANGINGGENE_H

#include <qnn-global.h>

#include "genericgene.h"

/*!
 * \brief The LengthChangingGene is a special form of GenericGene which can add or delete segments during mutation.
 */
class QNNSHARED_EXPORT LengthChangingGene : public GenericGene
{
public:
    /*!
     * \brief This struct contains all configuration option of LengthChangingGenes
     */
    struct config {
        /*!
         * \brief min_length contains the minimum amount of segments.
         *
         * If set to -1 it will default to the initial length of the gene
         */
        qint32 min_length;
        /*!
         * \brief max_length contains the maximum amount of segments.
         *
         * If set to -1 it will default to four times the initial length of the gene
         */
        qint32 max_length;

        /*!
         * \brief Constructor for standard values
         */
        config() :
            min_length(-1),
            max_length(-1)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param initialLength Amount of segments
     * \param segment_size Size of the segments
     * \param config Configuration of the LengthChangingGene
     */
    LengthChangingGene(qint32 initialLength, qint32 segment_size = 1, config config = config());

    /*!
     * \brief Deconstructur
     */
    ~LengthChangingGene();

    /*!
     * \brief Mutates the gene
     *
     * This method mutates the gene which means that each segment value has a low probability to be changed to a random value.
     * In addition to that there is a small chance that a segment will be deleted and there is a small chance that an additional segment will be added
     * This is important for genetic algorithms to overcome local maxima.
     */
    void mutate();

    /*!
     * \brief createCopy Creates a deep copy of the gene.
     * \return Deep copy of gene. The caller must delete the gene
     */
    GenericGene *createCopy();

    /*!
     * \brief A static method to create a LengthChangingGene from a given device.
     *
     * If the gene can't be created NULL will be returned.
     *
     * \param device The QIODevice from which to load the gene. Must not be opened
     * \return Loaded gene (NULL if unsuccessful). The caller must delete the gene
     */
    static GenericGene *loadThisGene(QIODevice *device);

protected:
    /*!
     * \brief Empty constructor
     *
     * This constructor may be useful if the gene is inherented
     */
    LengthChangingGene();

    /*!
     * \brief A constructor which crates a gene from a given segment list
     * \param gene Segment list
     * \param segment_size Length of the segments
     * \param config Configuration of the LengthChangingGene
     */
    LengthChangingGene(QVector< QVector<qint32> > gene, qint32 segment_size, config config = config());

    /*!
     * \brief Creates a gene out of a given segment list. The created gene should hold the same configuration as the object on which the method is called.
     * \param gene Segment list
     * \param segment_size Length of the segments
     * \return
     */
    GenericGene *createGene(QVector< QVector<qint32> > gene, qint32 segment_size);

    /*!
     * \brief Overwritten method to get an identifier.
     * \return Identifier
     */
    QString identifier();

    /*!
     * \brief Overwritten method to save gene.
     * \param stream Stream to save values to. Stream is guaranteed to be a valid pointer
     * \return True if save was successful
     */
    bool _saveGene(QTextStream *stream);

    /*!
     * \brief Overwritten method to load gene.
     * \param gene The already loaded segment list
     * \param segment_size Loaded length of segments
     * \param stream Stream to load values from. Stream is guaranteed to be a valid pointer
     * \return Loaded gene. The caller must delete the gene
     */
    GenericGene *_loadGene(QVector< QVector<qint32> > gene, qint32 segment_size, QTextStream *stream);

    /*!
     * \brief Holds the configuration of the gene.
     */
    config _config;
};

#endif // LENGTHCHANGINGGENE_H
