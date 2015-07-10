#ifndef CONSOLEOUTPUT_H
#define CONSOLEOUTPUT_H

#include <QObject>
#include <QDebug>

class ConsoleOutput : public QObject
{
    Q_OBJECT

public:
    explicit ConsoleOutput(QObject *parent = 0) :
        QObject(parent)
    {
    }

public slots:
    void get_ga_progress(int current, int max, double best_fitness_value)
    {
        qWarning() << "Round" << current << "of" << max << ": Best fitness:" << best_fitness_value;
    }
};

#endif // CONSOLEOUTPUT_H
