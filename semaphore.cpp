#include "semaphore.h"
#include <QDebug>


Semaphore::Semaphore(const QString &procFile, const QString &resFile, const QString &actFile)
    : cycle(-1), maxCycle(0),
    resources(resFile),
    actions(actFile),
    proc(procFile)
{
    pendingActions = actions.actionsList;

    for (int i = 0; i < proc.names.size(); ++i) {
        QString pid = proc.names[i];
        processes[pid] = proc.hexColor[i];
        procStates[pid] = {"IDLE", "", "", 1};
    }

    for (const auto& a : actions.actionsList) {
        if (a.cycle > maxCycle)
            maxCycle = a.cycle;
    }
}

void Semaphore::simulateNext() {
    if (finished()) return;
    cycle++;

    // 1. Liberar recursos usados en el ciclo anterior
    QSet<QString> liberados;
    for (auto it = procStates.begin(); it != procStates.end(); ++it) {
        const QString& state = it.value().state;
        const QString& res = it.value().resource;
        if (it.value().state == "ACCESSED") {
            qDebug() <<res<<"Was signaled, sem found at" <<resources.resourceCount[res];
            resources.resourceCount[res]+= 1;
            int sem_count = resources.resourceCount[res];
            qDebug() <<res<<"now"<<sem_count;
            if (sem_count>=0){
                liberados.insert(it.value().resource);
            }
        }
    }
    for (const auto& res : liberados) {
        lockedResources.remove(res);
    }

    // 2. Eliminar procesos que accedieron o estaban esperando (para reintentar sus acciones)
    for (auto it = procStates.begin(); it != procStates.end(); ) {
        const QString& state = it.value().state;
        if (it.value().state == "ACCESSED" || it.value().state == "WAITING") {
            it = procStates.erase(it);
        } else {
            ++it;
        }
    }

    // 3. Procesar primero acciones en espera (pendientes de ciclos anteriores)
    QVector<Action> nuevosPendientes;
    QSet<QString> procesadosEsteCiclo;

    for (const auto& action : pendingActions) {
        if (action.cycle > cycle) {
            nuevosPendientes.append(action); // todavía no es su turno
            continue;
        }

        QString pid = action.pid;
        QString resource = action.resource;
        int sem_count = resources.resourceCount[resource];
        QString type = action.action;
        QString idAccion = pid + "|" + resource + "|" + QString::number(action.cycle);

        if (accionesEjecutadas.contains(idAccion)) continue;
        if (procesadosEsteCiclo.contains(pid)) continue;

        if (sem_count>0) {
            qDebug() << pid << "Accessed "<< resource << "found sem at " << sem_count << "now its" << sem_count-1;
            lockedResources.insert(resource);
            resources.resourceCount[resource]-=1;
            procStates[pid] = {"ACCESSED", resource, type, sem_count-1};
            procesadosEsteCiclo.insert(pid);
            accionesEjecutadas.insert(idAccion);
        } else{
            qDebug() << pid << "waiting for "<< resource << "found sem at " << sem_count;
            nuevosPendientes.append(action);
            procStates[pid] = {"WAITING", resource, type, sem_count};
        }
    }

    // 4. Procesar nuevas acciones del ciclo actual
    for (const auto& action : actions.actionsList) {
        if (action.cycle != cycle) continue;

        QString pid = action.pid;
        QString resource = action.resource;
        int sem_count = resources.resourceCount[resource];
        QString type = action.action;
        QString idAccion = pid + "|" + resource + "|" + type + "|" + QString::number(action.cycle);

        if (accionesEjecutadas.contains(idAccion)) continue;
        if (procesadosEsteCiclo.contains(pid)) continue;

        if (sem_count>0) {
            qDebug() << pid << "Accessed "<< resource << "found sem at " << sem_count << "now its" << sem_count-1;
            resources.resourceCount[pid]-=1;
            lockedResources.insert(resource);
            procStates[pid] = {"ACCESSED", resource, type, sem_count-1};
            procesadosEsteCiclo.insert(pid);
            accionesEjecutadas.insert(idAccion);
        } else {
            qDebug() << pid << "waiting for "<< resource << "found sem at " << sem_count;
            nuevosPendientes.append(action);
            procStates[pid] = {"WAITING", resource, type, sem_count};
        }
    }

    // 5. Guardar las acciones pendientes que siguen sin ejecutarse
    pendingActions = nuevosPendientes;

    for (auto it = procStates.begin(); it != procStates.end(); ++it) {
        qDebug() << "Ciclo" << cycle
                 << "- PID:" << it.key()
                 << "- Estado:" << it.value().state
                 << "- Recurso:" << it.value().resource << "Sem:" << it.value().sem_counter
                 << "- Acción:" << it.value().type;
    }
}

int Semaphore::currentCycle() const {
    return cycle;
}

bool Semaphore::finished() const {
    return pendingActions.isEmpty() && procStates.isEmpty();
}

QString Semaphore::getStateForProcess(const QString &pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].state;
    }
    return "IDLE";
}

QString Semaphore::getResourceForProcess(const QString& pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].resource;
    }
    return "";
}

QString Semaphore::getActionTypeForProcess(const QString& pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].type;
    }
    return "";
}



