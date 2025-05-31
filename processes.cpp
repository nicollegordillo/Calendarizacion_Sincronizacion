#include "processes.h"
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDebug>
processes::processes(const QString &filePath) {
    // Fetch Contents
    // qDebug() << "Entered processes constructor";
    QString contents = "";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    } else {
        QTextStream in(&file);
        contents = in.readAll();
    }
    // qDebug() << "Read file contents "<< contents;
    // <PID>, <BT>, <AT>, <Priority>
    // Apply analysis
    QStringList lines = contents.split('\n');

    for (int i = 0; i<lines.length();i++){
        QString line = lines[i];
        QStringList tokens = line.split(',');
        if (tokens.length()!= 4){
            return;
        }
        names.append(tokens[0]);
        // qDebug() << "Name "<< names[i] << " for "<<i ;
        bool ok;
        arrivalTime.append(tokens[1].toInt(&ok));
        if (ok){
            // qDebug() << "AT "<< arrivalTime[i] << " for "<<i ;
        };
        burstTime.append(tokens[2].toInt(&ok));
        if (ok){
            // qDebug() << "BT "<< burstTime[i] << " for "<<i ;
        };
        priority.append(tokens[3].toInt(&ok));
        if (ok){
            // qDebug() << "Priority "<< priority[i] << " for "<<i ;
        };
    }
}
