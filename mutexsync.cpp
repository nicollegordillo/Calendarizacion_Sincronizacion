#include "mutexsync.h"
#include <QDebug>

MutexSync::MutexSync(const QString &procFile, const QString &resFile, const QString &actFile)
    : cycle(0), maxCycle(0),
    resources(resFile),
    actions(actFile),
    proc(procFile) // Aquí sí o sí debes inicializar 'proc'
{
    // Ya dentro de la clase, puedes usar 'proc' directamente
    for (int i = 0; i < proc.names.size(); ++i) {
        QString pid = proc.names[i];
        processes[pid] = proc.hexColor[i];
        procStates[pid] = {"IDLE", ""};
    }

    // Calcular el ciclo máximo según las acciones
    for (const auto &action : actions.actionsList) {
        if (action.cycle > maxCycle) {
            maxCycle = action.cycle;
        }
    }
}


void MutexSync::simulateNext() {
    if (cycle > maxCycle) return;

    // Procesar acciones del ciclo actual
    for (const auto &a : actions.actionsList) {
        if (a.cycle == cycle) {
            QString pid = a.pid;
            QString resource = a.resource;

            if (lockedResources.contains(resource)) {
                // Recurso bloqueado
                procStates[pid] = {"WAITING", resource};
            } else {
                // Recurso libre: accede y lo bloquea
                lockedResources.insert(resource);
                procStates[pid] = {"ACCESSED", resource};
            }
        }
    }

    // Liberar recursos que se accedieron en el ciclo anterior
    for (auto it = procStates.begin(); it != procStates.end(); ++it) {
        if (it.value().state == "ACCESSED") {
            lockedResources.remove(it.value().resource);
            it.value() = {"IDLE", ""};
        } else if (it.value().state == "WAITING") {
            it.value().state = "IDLE"; // volver a intentar en el siguiente ciclo
        }
    }

    cycle++;
}

int MutexSync::currentCycle() const {
    return cycle;
}

bool MutexSync::finished() const {
    return cycle > maxCycle;
}

QString MutexSync::getStateForProcess(const QString &pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].state;
    }
    return "IDLE";
}

