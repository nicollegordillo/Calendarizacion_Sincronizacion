#include "resources.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Resources::Resources(const QString &filePath) {
    loadResources(filePath);
}

void Resources::loadResources(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo de recursos:" << filePath;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList tokens = line.split(',');
        if (tokens.size() != 2) continue;

        QString resourceName = tokens[0].trimmed();
        bool ok;
        int count = tokens[1].trimmed().toInt(&ok);
        if (ok) {
            resourceCount[resourceName] = count;
        }
    }
}


