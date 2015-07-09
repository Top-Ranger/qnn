#include "simulation/genericsimulation.h"
#include "network/feedforwardnetwork.h"
#include "network/genericgene.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << FeedForwardNetwork::num_segments(5,1,10,30);
    FeedForwardNetwork testFFN(5,1,10,30);
    GenericGene *testGene = testFFN.getRandomGene();
    testFFN.initialise(testGene);
    GenericSimulation testSimulation(&testFFN);

    qDebug() << "Score:" << testSimulation.getScore();

    delete testGene;
    testGene = NULL;

    return 0;
}
