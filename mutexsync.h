#ifndef MUTEXSYNC_H
#define MUTEXSYNC_H

#include <QString>
#include <QMap>
#include <QSet>

// Primero incluimos los headers de los tipos que vamos a usar.
// Nos aseguramos de que, efectivamente, el compilador vea "processes" antes de usarlo:
#include "processes.h"
#include "resources.h"
#include "actions.h"

// (Opcional, para “reforzar” que existe la clase processes, en caso
// de que el include tenga algún conflicto:)
// class processes;

class MutexSync {
public:
    MutexSync(const QString &procFile,
              const QString &resFile,
              const QString &actFile);

    void simulateNext();
    int currentCycle() const;
    bool finished() const;

    QString getStateForProcess(const QString &pid) const;
    QMap<QString, QString> processes; // pid → color
    QString getResourceForProcess(const QString& pid) const;
    QString getActionTypeForProcess(const QString& pid) const;


private:
    int cycle;
    int maxCycle;
    QVector<Action> pendingActions;
    QSet<QString> accionesEjecutadas;


    Resources resources;
    Actions actions;

    // Aquí “proc” es una instancia de tu clase processes:
    Processes proc;

    struct ProcState {
        QString state;    // "WAITING", "ACCESSED", "IDLE"
        QString resource; // recurso que está usando o intentando usar
        QString type;

        ProcState(const QString& s = "IDLE", const QString& r = "", const QString& t = "")
            : state(s), resource(r), type(t) {}

    };

    QMap<QString, ProcState> procStates; // estado de cada proceso
    QSet<QString> lockedResources;       // recursos bloqueados
};

#endif // MUTEXSYNC_H


