#ifndef ACTIONS_H
#define ACTIONS_H

#include <QString>
#include <QVector>

struct Action {
    QString pid;
    QString action;   // e.g. "READ", "WRITE"
    QString resource;
    int cycle;
};

class Actions {
public:
    Actions(const QString &filePath);
    QVector<Action> actionsList;
};

#endif // ACTIONS_H
