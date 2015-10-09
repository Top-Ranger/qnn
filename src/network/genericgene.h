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

#ifndef GENERICGENE_H
#define GENERICGENE_H

#include <qnn-global.h>

#include <QList>
#include <QIODevice>
#include <QTextStream>

/*!
 * @brief GenericGene provides a base class for all genes
 *
 * The GenericGene class provides a base class for all genes. A gene defines the basic structure
 * of a given network type. A gene is platform-independent.
 *
 * A gene consists of multiple segemnts. Each of the segments has the same size.
 *
 * Other types of genes should inherent this class and m can override all virtual methods. The following methods are mandatory to override:
 *  - identifier()
 *  - _saveGene(QTextStream *stream)
 *  - _loadGene(QList< QList<qint32> > gene qint32 segment_size, QTextStream *stream)
 *  - createGene(QList< QList<qint32> > gene, qint32 segment_size)
 *  - createCopy()
 *
 *
 * Each implementation should also provide the following static methods:
 *  - static GenericGene *loadThisGene(QIODevice *device)
 */
class QNNSHARED_EXPORT GenericGene
{
public:
    /*!
     * \brief Public constructor for a GenricGene. This creates a gene with 'initialLength' segments, each segment has the size 'segment_size'
     * \param initialLength Amount of segments
     * \param segment_size Size of the segments
     */
    GenericGene(qint32 initialLength, qint32 segment_size = 1);

    /*!
     * \brief Deconstructur
     */
    virtual ~GenericGene();

    /*!
     * \brief Mutates the gene
     *
     * This method mutates the gene which means that each segment value has a low probability to be changed to a random value.
     * This is important for genetic algorithms to overcome local maxima.
     */
    virtual void mutate();

    /*!
     * \brief segments Return the list of segments from this gene.
     *
     * Please note: The list is returned by reference which means the content of the gene can be changed through it.
     * Although this might be desired for implementing other learning methods and such the segments should not be changed without explicit actions.
     *
     * \return Reference of list of segments
     */
    virtual QList< QList<qint32> >& segments();

    /*!
     * \brief createCopy Creates a deep copy of the gene.
     * \return Deep copy of gene. The caller must delete the gene
     */
    virtual GenericGene *createCopy();

    /*!
     * \brief Combines two genes and returns their two children.
     *
     * The combination used in this method is a one-point-crossover. The children are created using the createGene method. Both genes must have the same segment size.
     *
     * \param gene1 First parent gene
     * \param gene2 second parent gene
     * \return List of children. The caller must delete the children
     */
    static QList<GenericGene *> combine(GenericGene *gene1, GenericGene *gene2);

    /*!
     * \brief Saves this gene to a given device.
     *
     * This is a wrapper function which manages the device as well saves the GenericGene.
     * This method will then call _saveGene where subclasses can save their values.
     *
     * \param device The QIODevice to which the save may go. Must not be opened
     * \return True if save was successful
     */
    bool saveGene(QIODevice *device);

    /*!
     * \brief Creates a gene from a given device.
     *
     * This is a wrapper function which manages the device as well saves the GenericGene.
     * This method will then call _saveGene where subclasses can save their values.
     *
     * If the gene can't be loaded NULL will be returned.
     *
     * \param device The QIODevice from which to load the gene. Must not be opened
     * \return Loaded Gene (NULL if unsuccessful). The caller must delete the gene
     */
    GenericGene *loadGene(QIODevice *device);

    /*!
     * \brief Test if the gene can be loaded from the device.
     *
     * This method performs a simple test if the gene can be loaded from a given device.
     * However if this method returns true it does not mean that the load procedure will be successful.
     *
     * \param device The QIODevice to test
     * \return True if the gene can be loaded
     */
    bool canLoad(QIODevice *device);

    /*!
     * \brief Returns a random value independent of platform
     *
     * qrand return a value in the range [0, RAND_MAX]. As RAND_MAX is library-dependent genes that were created
     * with qrand would not be transferable to another c++ library.
     *
     * This method creates a random value in the range of [0, MAX_GENE_VALUE] making the whole gene independent of any
     * standard library. However as qrand is used qsrand() has to be called to get true random values.
     *
     * \return A random int in the range [0, MAX_GENE_VALUE]
     */
    qint32 getIndependentRandomInt();

    /*!
     * \brief A static method to create a GenericGene from a given device.
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
    GenericGene();

    /*!
     * \brief A constructor which crates a gene from a given segment list
     * \param gene Segment list
     * \param segment_size Length of the segments
     */
    GenericGene(QList< QList<qint32> > gene, qint32 segment_size);

    /*!
     * \brief Creates a gene out of a given segment list. The created gene should hold the same configuration as the object on which the method is called.
     * \param gene Segment list
     * \param segment_size Length of the segments
     * \return Created gene. The caller must delete the gene
     */
    virtual GenericGene *createGene(QList< QList<qint32> > gene, qint32 segment_size);

    /*!
     * \brief Returns an identifier unique to the gene class
     *
     * This method should return a unique identifier. This is used e.g. to determine the gene type from a save file.
     * One simple method is to return the class name, e.g. "GenericGene" for GenericGene
     *
     * \return Identifier
     */
    virtual QString identifier();

    /*!
     * \brief Saves gene-specific values
     *
     * This method is called by saveGene after the segments have been saved. It should be used to save gene-specific values of subclasses.
     *
     * \param stream Stream to save values to. Stream is guaranteed to be a valid pointer
     * \return True if save was successful
     */
    virtual bool _saveGene(QTextStream *stream);

    /*!
     * \brief Loads gene-specific values
     *
     * This method is called by loadGene after the segments have been saved. It should be used to save gene-specific values of subclasses.
     * The stream should be at the first value after the segment values.
     * The result should be a pointer on a new gene object or NULL if the gene could not be loaded.
     *
     * \param gene The already loaded segment list
     * \param segment_size Loaded length of segments
     * \param stream Stream to load values from. Stream is guaranteed to be a valid pointer
     * \return Loaded gene. The caller must delete the gene
     */
    virtual GenericGene *_loadGene(QList< QList<qint32> > gene, qint32 segment_size, QTextStream *stream);

    /*!
     * \brief The list of segments.
     *
     * A segment is a QList of qint32
     */
    QList< QList<qint32> > _gene;

    /*!
     * \brief The length of a segment
     */
    qint32 _segment_size;

    /*!
     * \brief MUTATION_RATE is the probability of a mutation occuring.
     */
    static const double MUTATION_RATE = 0.03;
};

#endif // GENERICGENE_H
