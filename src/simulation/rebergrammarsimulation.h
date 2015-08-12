#ifndef REBERGRAMMARSIMULATION_H
#define REBERGRAMMARSIMULATION_H

#include <qnn-global.h>

#include "genericsimulation.h"

class QNNSHARED_EXPORT ReberGrammarSimulation : public GenericSimulation
{
public:

    enum SimulationMode {
        DetectGrammar,
        CreateWords
    };

    struct config {
        SimulationMode mode;
        bool embedded;
        int trials_detect;
        int trials_create;
        double detect_threshold;
        int max_depth;

        config() :
            mode(DetectGrammar),
            embedded(false),
            trials_detect(500),
            trials_create(50),
            detect_threshold(0.50d),
            max_depth(50)
        {
        }
    };

    ReberGrammarSimulation(config config = config());
    ~ReberGrammarSimulation();

    int needInputLength();
    int needOutputLength();

    GenericSimulation *createConfigCopy();

protected:
    void _initialise();
    double _getScore();

    config _config;
};

#endif // REBERGRAMMARSIMULATION_H
