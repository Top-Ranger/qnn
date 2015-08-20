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

#ifndef TMAZESIMULATION_H
#define TMAZESIMULATION_H

#include <qnn-global.h>

#include "genericsimulation.h"
#include <QList>

/*!
 * \brief The TMazeSimulation class represents the t-maze simulation.
 *
 * In the Tmaze simulation a robot must walk a discrete corridor. Each step contains a number.
 * Based on the numbers the robot must make a decision at the end if he wants to go to the left (G1) or to the right (G2).
 */
class QNNSHARED_EXPORT TMazeSimulation : public GenericSimulation
{
public:
    /*!
     * \brief Generates a standard t-maze.
     *
     * This maze is 7 steps long and contains a number at the start and at the end.
     *
     * \return T-maze
     */
    static QList<qint32> generateStandardTMaze();

    /*!
     * \brief Standard function to determine if G1 is correct.
     *
     * This function returns true if the first and the last step have the same number.
     *
     * \param list T-maze
     * \return G1 correct
     */
    static bool standardG1Correct(QList<qint32> list);

    /*!
     * \brief This struct contains all configuration option of the simulation
     */
    struct config {
        /*!
         * \brief trials saves the amount of trials.
         */
        qint32 trials;

        /*!
         * \brief max_timesteps contains the amount of timesteps in which the robot has to reach the goal.
         */
        qint32 max_timesteps;

        /*!
         * \brief range_input contains the amount of numbers which can appear.
         *
         * The range of the appearing numbers must be in [0,range_input].
         */
        qint32 range_input;

        /*!
         * \brief generateTMaze is the functions that generates the t-maze
         *
         * The function must return a QList<qint32>. The numbers must be in [0,range_input].
         */
        QList<qint32> (*generateTMaze)();

        /*!
         * \brief G1Correct contains the function that checks if G1 is correct (true) or G2 is correct (false) for the given t-maze
         */
        bool (*G1Correct)(QList<qint32> list);

        /*!
         * \brief Constructor for standard values
         */
        config() :
            trials(24),
            max_timesteps(50),
            range_input(5),
            generateTMaze(&generateStandardTMaze),
            G1Correct(&standardG1Correct)
        {
        }
    };

    /*!
     * \brief Constructor
     * \param config Configuration of t-maze
     */
    TMazeSimulation(config config = config());

    /*!
     * \brief Destructor
     */
    ~TMazeSimulation();

    /*!
     * \brief Overwritten function to get the needed input length
     *
     * The input length equals config.range_input
     *
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
    GenericSimulation *createConfigCopy();

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

#endif // TMAZESIMULATION_H
