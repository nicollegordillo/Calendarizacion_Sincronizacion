#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QVector>
#include <QString>
#include "processes.h"

class scheduler
{
public:
    scheduler(const QString  &, int, int);
    QString getExcecutedName();
    QString getColorByName(QString &);
    QString nombre;
    void calculateNext();
    void nextFIFO();
    void nextSJF();
    void nextSRT();
    void nextRR();
    void nextPS();
    int method;
        // 0 = FIFO
        // 1 = SJB
        // 2 = STR
        // 3 = RR
        // 4 = Priority / Preemtive Scheduling

    int t;
    int quantum;
    bool finished;
    bool initializedSJF = false;
    QVector<int> queue; // Vector of process index in Processes lists
    QVector<int> timeline;
    QVector<int> originalBurstTime;
    Processes snapshot;
};

#endif // SCHEDULER_H
