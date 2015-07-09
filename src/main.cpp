#include <QCoreApplication>

#include "genericsimulation.h"
#include "Networks/feedforwardnetwork.h"
#include "Networks/genericgene.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << FeedForwardNetwork::num_segments(5,1,10,30);
    GenericGene testGene(FeedForwardNetwork::num_segments(5,1,10,30));
    FeedForwardNetwork testFFN(testGene,5,1,10,30);
    GenericSimulation testSimulation(&testFFN);

    qDebug() << "Score:" << testSimulation.getScore();

    return a.exec();
}
