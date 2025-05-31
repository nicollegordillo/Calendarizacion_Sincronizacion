#ifndef PROCESSES_H
#define PROCESSES_H

#include "qcontainerfwd.h"
#include <QVector>
#include <QString>
#include <QColor>

class processes
{
public:
    processes(const QString &);
    QVector<QString> names;
    QVector<int> arrivalTime;
    QVector<int> burstTime;
    QVector<int> priority;
    QVector<QString> hexColor;
    QString genColor(int, int);
};

#endif // PROCESSES_H
