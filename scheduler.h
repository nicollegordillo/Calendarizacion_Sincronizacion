#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QVector>
#include <QString>
#include <processes.h>

class scheduler
{
public:
    scheduler(const QString  &, int);
    void calcNext();
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
    QVector<QString> stack;
    QVector<QString> timeline;
    processes snapshot;
};

#endif // SCHEDULER_H
