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

#ifndef REBERGRAMMARSIMULATION_H
#define REBERGRAMMARSIMULATION_H

#include <qnn-global.h>

#include "abstractsimulation.h"

/*!
 * \brief The ReberGrammarSimulation class represents the reber grammar task.
 *
 * The network has to either detect if a given word is part of the grammar or to complete a given
 * word beginning. The used grammar is a reber grammar.
 *
 * For more information see http://www.willamette.edu/~gorr/classes/cs449/reber.html
 */
class QNNSHARED_EXPORT ReberGrammarSimulation : public AbstractSimulation
{
public:

    /*!
     * \brief The SimulationMode enum contains the two possible simulation modes.
     */
    enum SimulationMode {
        DetectGrammar,
        CreateWords
    };

    /*!
     * \brief This struct contains all configuration option of the simulation
     */
    struct config {
        /*!
         * \brief mode The simulation mode.
         */
        SimulationMode mode;

        /*!
         * \brief embedded contains if the embedded reber grammar should be used.
         */
        bool embedded;

        /*!
         * \brief trials_detect includes the number of trials at the "DetectGrammar" mode
         */
        qint32 trials_detect;

        /*!
         * \brief trials_detect includes the number of trials at the "CreateWords" mode
         */
        qint32 trials_create;

        /*!
         * \brief detect_threshold holds the value at which neuron output the network sees a word as correct ("DetectGrammar").
         *
         * The value should be in (0,1) for best compatibility.
         */
        double detect_threshold;

        /*!
         * \brief max_depth contains the maximum depth at the reber word creation. The bigger the number the longer the words can get.
         */
        qint32 max_depth;

        /*!
         * \brief Configuration of the simulation
         */
        config() :
            mode(DetectGrammar),
            embedded(false),
            trials_detect(500),
            trials_create(50),
            detect_threshold(0.50),
            max_depth(50)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param config Configuration of ReberGrammarSimulation
     */
    ReberGrammarSimulation(config config = config());

    /*!
     * \brief Destructor
     */
    ~ReberGrammarSimulation();

    /*!
     * \brief Overwritten function to get the needed input length
     * \return Input length
     */
    qint32 needInputLength();

    /*!
     * \brief Overwritten function to get the needed output length
     * \return Output length
     */
    qint32 needOutputLength();

    /*!
     * \brief Creates an uninitialised copy of the simulation
     * \return Copy of the simulation. The caller must delete the simulation
     */
    AbstractSimulation *createConfigCopy();

protected:
    /*!
     * \brief Overwritten function to initialise the simulation
     */
    void _initialise();

    /*!
     * \brief Overwritten function to calculate the score
     */
    double _getScore();

    /*!
     * \brief Configuration of the simulation
     */
    config _config;
};

#endif // REBERGRAMMARSIMULATION_H
