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

#include "rebergrammarsimulation.h"

#include <QDebug>

namespace {

enum ReberMode {
    reber_create_random_word,
    reber_verify_word
};

bool reber_state0(QString &s, ReberMode mode, int &depth);
bool reber_state1(QString &s, ReberMode mode, int &depth);
bool reber_state2(QString &s, ReberMode mode, int &depth);
bool reber_state3(QString &s, ReberMode mode, int &depth);
bool reber_state4(QString &s, ReberMode mode, int &depth);
bool reber_state5(QString &s, ReberMode mode, int &depth);
bool reber(QString &s, ReberMode mode, int max_depth = 50);
bool embedded_reber(QString &s, ReberMode mode, int max_depth = 50);

/*
   Reber grammar

               S
          T -> 1 -X-> 2 - S
         /          / ^    \
 B ---> 0         X   P     3 -E->
         \      </    |    /
          P -> 4 -V-> 5 - V
               T

Input order: BTSXEPV
*/

bool embedded_reber(QString &s, ReberMode mode, int max_depth)
{
    if(max_depth < 1)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Invalid depth").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }

    if(mode == reber_create_random_word)
    {
        s.clear();
        if(reber(s, mode, max_depth))
        {
            if(qrand()%2)
            {
                s.prepend("BT");
                s.append("TE");
            }
            else
            {
                s.prepend("BP");
                s.append("PE");
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() < 5)
        {
            return false;
        }
        if(s.at(0) == QChar('B') && s.at(1) == QChar('T') && s.at(s.length()-2) == QChar('T') && s.at(s.length()-1) == QChar('E'))
        {
            s = s.remove(s.length()-2, 2);
            s = s.remove(0, 2);
            return reber(s, mode, max_depth);
        }
        else if(s.at(0) == QChar('B') && s.at(1) == QChar('P') && s.at(s.length()-2) == QChar('P') && s.at(s.length()-1) == QChar('E'))
        {
            s = s.remove(s.length()-2, 2);
            s = s.remove(0, 2);
            return reber(s, mode, max_depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber(QString &s, ReberMode mode, int max_depth)
{
    if(max_depth < 1)
    {
        qFatal(QString("FATAL ERROR in %1 %2: Invalid depth").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }

    int depth = max_depth;

    if(mode == reber_create_random_word)
    {
        s.clear();
        return reber_state0(s.append('B'), mode, depth);
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() < 5)
        {
            return false;
        }
        else if(s.at(0) == QChar('B'))
        {
            return reber_state0(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state0(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        if(qrand()%2)
        {
            return reber_state1(s.append('T'), mode, depth);
        }
        else
        {
            return reber_state4(s.append('P'), mode, depth);
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.at(0) == QChar('T'))
        {
            return reber_state1(s.remove(0,1), mode, depth);
        }
        else if(s.at(0) == QChar('P'))
        {
            return reber_state4(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state1(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        if(qrand()%2)
        {
            return reber_state1(s.append('S'), mode, depth);
        }
        else
        {
            return reber_state2(s.append('X'), mode, depth);
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.at(0) == QChar('S'))
        {
            return reber_state1(s.remove(0,1), mode, depth);
        }
        else if(s.at(0) == QChar('X'))
        {
            return reber_state2(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state2(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        if(qrand()%2)
        {
            return reber_state3(s.append('S'), mode, depth);
        }
        else
        {
            return reber_state4(s.append('X'), mode, depth);
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.at(0) == QChar('S'))
        {
            return reber_state3(s.remove(0,1), mode, depth);
        }
        else if(s.at(0) == QChar('X'))
        {
            return reber_state4(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state3(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        s += "E";
        return true;
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.length() != 1)
        {
            return false;
        }
        else
        {
            return s.at(0) == QChar('E');
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state4(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        if(qrand()%2)
        {
            return reber_state4(s.append('T'), mode, depth);
        }
        else
        {
            return reber_state5(s.append('V'), mode, depth);
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.at(0) == QChar('T'))
        {
            return reber_state4(s.remove(0,1), mode, depth);
        }
        else if(s.at(0) == QChar('V'))
        {
            return reber_state5(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

bool reber_state5(QString &s, ReberMode mode, int &depth)
{
    if(depth-- <= 0)
    {
        return false;
    }

    if(mode == reber_create_random_word)
    {
        if(qrand()%2)
        {
            return reber_state2(s.append('P'), mode, depth);
        }
        else
        {
            return reber_state3(s.append('V'), mode, depth);
        }
    }
    else if(mode == reber_verify_word)
    {
        if(s.length() == 0)
        {
            return false;
        }
        else if(s.at(0) == QChar('P'))
        {
            return reber_state2(s.remove(0,1), mode, depth);
        }
        else if(s.at(0) == QChar('V'))
        {
            return reber_state3(s.remove(0,1), mode, depth);
        }
        else
        {
            return false;
        }
    }
    else
    {
        qFatal(QString("FATAL ERROR in %1 %2: Unknown mode").arg(__FILE__).arg(__LINE__).toLatin1().data());
        return false;
    }
}

char getRandomReberChar()
{
    switch(qrand()%7)
    {
    case 0:
        return 'B';
        break;
    case 1:
        return 'T';
        break;
    case 2:
        return 'S';
        break;
    case 3:
        return 'X';
        break;
    case 4:
        return 'E';
        break;
    case 5:
        return 'P';
        break;
    case 6:
        return 'V';
        break;
    default:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Impossible random status";
        return 'B';
        break;
    }
}

QList<double> reberCharToInput(char c)
{
    QList<double> input;
    for(int i = 0; i < 7; ++i)
    {
        input << 0.0d;
    }

    switch (c) {
    case 'B':
        input[0] = 1.0d;
        break;
    case 'T':
        input[1] = 1.0d;
        break;
    case 'S':
        input[2] = 1.0d;
        break;
    case 'X':
        input[3] = 1.0d;
        break;
    case 'E':
        input[4] = 1.0d;
        break;
    case 'P':
        input[5] = 1.0d;
        break;
    case 'V':
        input[6] = 1.0d;
        break;
    default:
        break;
    }

    return input;
}

char networkToReberOutput(AbstractNeuralNetwork *network)
{
    double max_value = -10.0d;
    int max = -1;

    for(int i = 0; i < 8; ++i)
    {
        if(network->getNeuronOutput(i) > max_value)
        {
            max = i;
            max_value = network->getNeuronOutput(i);
        }
    }
    switch(max)
    {
    case -1:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Mo max_value found";
        return '\0';
        break;
    case 0:
        return 'B';
        break;
    case 1:
        return 'T';
        break;
    case 2:
        return 'S';
        break;
    case 3:
        return 'X';
        break;
    case 4:
        return 'E';
        break;
    case 5:
        return 'P';
        break;
    case 6:
        return 'V';
        break;
    case 7:
        return '\0';
        break;
    default:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": max =" << max;
        return '\0';
        break;
    }
}
} // End namespace

ReberGrammarSimulation::ReberGrammarSimulation(config config) :
    GenericSimulation(),
    _config(config)
{
}

ReberGrammarSimulation::~ReberGrammarSimulation()
{
}

int ReberGrammarSimulation::needInputLength()
{
    return 7;
}

int ReberGrammarSimulation::needOutputLength()
{
    switch (_config.mode) {
    case DetectGrammar:
        return 1;
        break;
    case CreateWords:
        return 8;
        break;
    default:
        qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": Unknown simulation mode";
        return 1;
    }
}

GenericSimulation *ReberGrammarSimulation::createConfigCopy()
{
    return new ReberGrammarSimulation(_config);
}

void ReberGrammarSimulation::_initialise()
{
}

double ReberGrammarSimulation::_getScore()
{
    double score = 0.0d;

    bool (*reber_function)(QString &s, ReberMode mode, int max_depth);

    if(_config.embedded)
    {
        reber_function = &embedded_reber;
    }
    else
    {
        reber_function = &reber;
    }

    int max_trials = 0;

    switch (_config.mode) {
    case DetectGrammar:
        max_trials = _config.trials_detect;
        break;
    case CreateWords:
        max_trials = _config.trials_create;
        break;
    }

    for(int trial = 0; trial < max_trials; ++trial)
    {
        AbstractNeuralNetwork *network = _network->createConfigCopy();
        network->initialise(_gene);

        QString word;
        QString input_word;
        bool result = false;
        do {
            result = reber_function(word, reber_create_random_word, _config.max_depth);
        } while (!result);

        switch (_config.mode) {
        case DetectGrammar:
            if(qrand()%2)
            {
                // Replace some characters to get an ivalid word
                bool valid;
                QString temp_word;
                do {
                    word.replace(qrand()%word.length(), 1, QChar(getRandomReberChar()));
                    temp_word = word;
                    valid = reber_function(temp_word, reber_verify_word, _config.max_depth);
                } while(valid);
            }

                input_word = word;

                while(input_word.length() != 0)
                {
                    QList<double> input = reberCharToInput(input_word.at(0).toAscii());
                    input_word.remove(0,1);
                    network->processInput(input);
                }
                if((network->getNeuronOutput(0) >= _config.detect_threshold) == reber_function(word, reber_verify_word, _config.max_depth))
                {
                    score += 1.0d;
                }
            break;

        case CreateWords:
            QString::Iterator iter = word.begin();
            QString input_word;
            char c_output = '\1';

            int input_length = (word.length()/2) + qrand()%(word.length()/4)+1;
            for(int i = 0; i < input_length && iter != word.end(); ++i)
            {
                input_word.append(*iter++);
            }
            for(QString::Iterator input_iter = input_word.begin(); input_iter !=input_word.end(); ++input_iter)
            {
                QList<double> input = reberCharToInput((*input_iter).toAscii());
                network->processInput(input);
                c_output = networkToReberOutput(network);
            }
            int current_depth = _config.max_depth-input_word.length();
            bool finished = c_output == '\0';

            if(c_output == '\1')
            {
                qWarning() << "WARNING in " __FILE__ << " " << __LINE__ << ": No input";
                return 0.0d;
            }

            while(!finished && current_depth-- > 0)
            {
                input_word.append(c_output);
                QList<double> input = reberCharToInput(c_output);
                network->processInput(input);
                c_output = networkToReberOutput(network);
                finished = c_output == '\0';
            }

            if(finished && reber_function(input_word, reber_verify_word, _config.max_depth))
            {
                score += 1.0d;
            }
            break;
        }
        delete network;
    }
    return score / max_trials;
}
