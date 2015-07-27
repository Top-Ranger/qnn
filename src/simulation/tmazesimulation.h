#ifndef TMAZESIMULATION_H
#define TMAZESIMULATION_H

#include <qnn-global.h>

#include "genericsimulation.h"
#include <QList>

class QNNSHARED_EXPORT TMazeSimulation : public GenericSimulation
{
public:
    static QList<double> generateStandardTMaze();
    static bool standardG1Correct(QList<double> list);

    struct config {
        int trials;
        int max_timesteps;
        QList<double> (*generateTMaze)();
        bool (*G1Correct)(QList<double> list);

        config() :
            trials(24),
            max_timesteps(500),
            generateTMaze(&generateStandardTMaze),
            G1Correct(&standardG1Correct)
        {
        }
    };

    TMazeSimulation(config config = config());
    ~TMazeSimulation();

    int needInputLength();
    int needOutputLength();

    GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    config _config;
};

#endif // TMAZESIMULATION_H
