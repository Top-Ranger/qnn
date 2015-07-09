#include <QCoreApplication>

#include "genericsimulation.h"
#include "Networks/feedforwardnetwork.h"
#include "Networks/genericgene.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << FeedForwardNetwork::num_segments(5,1,0,3);
    GenericGene testGene(FeedForwardNetwork::num_segments(5,1,0,3));
    FeedForwardNetwork testFFN(testGene,5,1,1,3);
    GenericSimulation testSimulation(&testFFN);

    qDebug() << "Score:" << testSimulation.getScore();

    return a.exec();
}
