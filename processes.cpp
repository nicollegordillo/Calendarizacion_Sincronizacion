#include "processes.h"
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRandomGenerator>

Processes::Processes(const QString &filePath) {
    // Fetch Contents
    qDebug() << "Entered processes constructor";
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
    int total = lines.length();

    for (int i = 0; i<total;i++){
        QString line = lines[i];
        QStringList tokens = line.split(',');
        if (tokens.length()!= 4){
            return;
        }
        names.append(tokens[0]);
        // qDebug() << "Name "<< names[i] << " for "<<i ;
        bool ok;
        burstTime.append(tokens[1].toInt(&ok));
        if (ok){
            // qDebug() << "AT "<< arrivalTime[i] << " for "<<i ;
        };
        arrivalTime.append(tokens[2].toInt(&ok));
        if (ok){
            // qDebug() << "BT "<< burstTime[i] << " for "<<i ;
        };
        priority.append(tokens[3].toInt(&ok));
        if (ok){
            // qDebug() << "Priority "<< priority[i] << " for "<<i ;
        };
        QString color = genColor(i+1, total);
         qDebug() << "Color" << color;
        hexColor.append(color);
    }
    qDebug() << "Exited processes constructor";

}

QString Processes::genColor(int index, int total) {
    // Espaciado uniforme sobre todo el espectro (360° HSV)
    int hue = (index * 137) % 360;

    // Para evitar agrupaciones similares, variar saturación o valor un poco
    int sat = 200 + ((index * 37) % 56);  // entre 200 y 255
    int val = 220 + ((index * 53) % 36);  // entre 220 y 255

    QColor color = QColor::fromHsv(hue, sat, val);
    return color.name();
}

