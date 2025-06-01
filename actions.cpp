#include "actions.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Actions::Actions(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo de acciones:" << filePath;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList tokens = line.split(',');
        if (tokens.size() != 4) continue;

        Action a;
        a.pid = tokens[0].trimmed();
        a.action = tokens[1].trimmed().toUpper(); // "READ" or "WRITE"
        a.resource = tokens[2].trimmed();
        a.cycle = tokens[3].trimmed().toInt();

        actionsList.append(a);
    }
}

