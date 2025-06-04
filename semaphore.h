#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <QString>
#include <QMap>
#include <QSet>

// Primero incluimos los headers de los tipos que vamos a usar.
// Nos aseguramos de que, efectivamente, el compilador vea "processes" antes de usarlo:
#include "processes.h"
#include "resources.h"
#include "actions.h"


class Semaphore
{
public:
    Semaphore(const QString &procFile,
              const QString &resFile,
              const QString &actFile);

    void simulateNext();
    int currentCycle() const;
    bool finished() const;
    QVector<QString> procesosEnCicloActual() const;


    QString getStateForProcess(const QString &pid) const;
    QMap<QString, QString> processes; // pid → color
    QString getResourceForProcess(const QString& pid) const;
    QString getActionTypeForProcess(const QString& pid) const;
    QMap<QString, QMap<int, QString>> timelineStates;     // PID -> ciclo -> estado
    QMap<QString, QMap<int, QString>> timelineResources;  // PID -> ciclo -> recurso
    QMap<QString, QMap<int, QString>> timelineActions;    // PID -> ciclo -> tipo de acción
private:
    int cycle;
    int maxCycle;
    bool modoExplicito;  // true si es acquire/release, false si es read/write
    QVector<Action> pendingActions;
    QSet<QString> accionesEjecutadas;
    Resources resources;
    Actions actions;
    QVector<QString> ordenProcesados;

    // Aquí “proc” es una instancia de tu clase processes:
    Processes proc;

    struct ProcState {
        QString state;    // "WAITING", "ACCESSED", "IDLE"
        QString resource; // recurso que está usando o intentando usar
        QString type;
        int sem_counter;

        ProcState(const QString& s = "IDLE", const QString& r = "", const QString& t = "", int c = 1)
            : state(s), resource(r), type(t), sem_counter(c){}

    };

    QMap<QString, ProcState> procStates; // estado de cada proceso
    QSet<QString> lockedResources;       // recursos bloqueados
};


#endif // SEMAPHORE_H
