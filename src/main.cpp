#include "simulation/genericsimulation.h"
#include "network/feedforwardnetwork.h"
#include "network/genericgene.h"
#include "ga/genericgeneticalgorithm.h"

#include <QTime>
#include <QDebug>
#include "ConsoleOutput.h"

int main(int argc, char *argv[])
{
    /*
    qDebug() << FeedForwardNetwork::num_segments(5,1,10,30);
    FeedForwardNetwork testFFN(5,1,10,30);
    GenericGene *testGene = testFFN.getRandomGene();
    testFFN.initialise(testGene);
    GenericSimulation testSimulation;
    testSimulation.initialise(&testFFN);

    qDebug() << "Score:" << testSimulation.getScore();

    delete testGene;
    testGene = NULL;
    */

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    ConsoleOutput *output = new ConsoleOutput;
    FeedForwardNetwork testFFN(5,1,10,30);
    GenericSimulation testSimulation;
    GenericGeneticAlgorithm testGa(&testFFN, &testSimulation);

    QObject::connect(&testGa, SIGNAL(ga_current_round(int,int,double)), output, SLOT(get_ga_progress(int,int,double)));

    testGa.run_ga();

    qWarning() << "+++++++++++++";
    qWarning() << "Best fitness:" << testGa.best_fitness();

    delete output;

    return 0;
}
