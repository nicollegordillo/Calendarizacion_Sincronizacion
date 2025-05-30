#ifndef PROCESSES_H
#define PROCESSES_H

#include "qcontainerfwd.h"
#include <QVector>
#include <QString>
class processes
{
public:
    processes(const QString &);
    QVector<QString> names;
    QVector<int> arrivalTime;
    QVector<int> burstTime;
    QVector<int> priority;
};

#endif // PROCESSES_H
