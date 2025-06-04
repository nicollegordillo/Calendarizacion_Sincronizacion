#include "semaphore.h"
#include <QDebug>


Semaphore::Semaphore(const QString &procFile, const QString &resFile, const QString &actFile)
    : cycle(-1), maxCycle(0),
    resources(resFile),
    actions(actFile),
    proc(procFile),
    has_resource()
{
    pendingActions = actions.actionsList;

    // 1. Identificar procesos activos (que aparecen en el archivo de acciones)
    QSet<QString> procesosActivos;
    for (const auto& a : actions.actionsList) {
        procesosActivos.insert(a.pid);
        if (a.cycle > maxCycle)
            maxCycle = a.cycle;
    }
    // Detectar el modo automáticamente
    modoExplicito = false; // valor por defecto
    for (const Action& action : pendingActions) {
        QString tipo = action.action.toUpper();
        if (tipo == "ACQUIRE" || tipo == "SIGNAL") {
            modoExplicito = true;
            break;
        }
    }

    // 2. Registrar solo los procesos activos
    for (int i = 0; i < proc.names.size(); ++i) {
        QString pid = proc.names[i];
        if (procesosActivos.contains(pid)) {
            processes[pid] = proc.hexColor[i];
            procStates[pid] = {"IDLE", "", ""};
        }
    }
}

void Semaphore::simulateNext() {
    if (modoExplicito){
        if (finished()) return;
        cycle++;
        ordenProcesados.clear();

        QVector<Action> nuevosPendientes;
        QSet<QString> procesadosEsteCiclo;

        // 1. Liberar recursos usados en el ciclo anterior
        for (const auto& action : actions.actionsList) {
            if (action.cycle != cycle) continue;
            if (action.action.toLower() == "signal") {
                QString pid = action.pid;
                QString resource = action.resource;
                QString idAccion = pid + "|" + resource + "|" + action.action + "|" + QString::number(action.cycle);

                if (accionesEjecutadas.contains(idAccion)) continue;

                qDebug() <<resource<<"Was signaled, sem found at" <<resources.resourceCount[resource];
                resources.resourceCount[resource]+= 1;
                int sem_count = resources.resourceCount[resource];
                qDebug() <<resource<<"now"<<sem_count;

                if (sem_count>=0){  // liberar explícitamente si semaforo lo indica
                    QPair<QString, QString> key(pid, resource);
                    lockedResources.remove(resource);
                    accionesEjecutadas.insert(idAccion);
                    if (has_resource[key]){
                        has_resource[key] = false;
                    }
                    procStates[pid].state = "IDLE";
                    procStates[pid].resource = resource;
                    procStates[pid].type = "signal";
                    procesadosEsteCiclo.insert(pid);
                    if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
                }
            }
        }

        // 2. Procesar pendientes y nuevas acciones de tipo acquire/read/write
        QVector<Action> accionesActuales = actions.actionsList;
        accionesActuales += pendingActions;

        for (const auto& action : accionesActuales) {
            if (action.cycle > cycle) {
                nuevosPendientes.append(action);
                continue;
            }

            if (action.action.toLower() == "signal") continue; // ya procesado arriba

            QString pid = action.pid;
            QString resource = action.resource;
            QString type = action.action.toLower();
            QString idAccion = pid + "|" + resource + "|" + type + "|" + QString::number(action.cycle);
            int sem_count = resources.resourceCount[resource];

            if (accionesEjecutadas.contains(idAccion)) continue;
            if (procesadosEsteCiclo.contains(pid)) continue;
            QPair<QString, QString> key(pid, resource);

            qDebug() << cycle << pid << "has" << has_resource << "to" << resource;
            if (sem_count>0 |  has_resource[key]) {

                if (! has_resource[key]){
                    qDebug() << pid << "Accessed "<< resource << "found sem at " << sem_count << "now its" << sem_count-1;
                    lockedResources.insert(resource);
                    resources.resourceCount[resource]-=1;

                } else {
                    qDebug() << pid << "Accessed "<< resource << "(it already had access)" << sem_count;
                }
                procStates[pid].state = "ACCESSED";
                procStates[pid].resource = resource;
                procStates[pid].type = type;
                has_resource[key] = true;
                qDebug() << cycle << pid << "has" << has_resource[key] << "to" << resource;
                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else {
                qDebug() << pid << "waiting for "<< resource << "found sem at " << sem_count;
                nuevosPendientes.append(action);
                procStates[pid] = {"WAITING", resource, type};
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            }
        }

        // 3. Guardar pendientes
        pendingActions = nuevosPendientes;
        // 4. Guardar estados en timeline
        for (const auto& pid : processes.keys()) {
            if (procStates.contains(pid)) {
                timelineStates[pid][cycle] = procStates[pid].state;
                timelineResources[pid][cycle] = procStates[pid].resource;
                timelineActions[pid][cycle] = procStates[pid].type;
            } else {
                timelineStates[pid][cycle] = "IDLE";
                timelineResources[pid][cycle] = "";
                timelineActions[pid][cycle] = "";
            }
        }

        for (auto it = procStates.begin(); it != procStates.end(); ++it) {
            qDebug() << "Ciclo" << cycle
                     << "- PID:" << it.key()
                     << "- Estado:" << it.value().state
                     << "- Recurso:" << it.value().resource
                     << "- Acción:" << it.value().type;
        }

        // 5. Ahora sí, limpiar procStates si ya no están activos
        for (auto it = procStates.begin(); it != procStates.end(); ) {
            if (it.value().state == "ACCESSED" || it.value().state == "WAITING" || it.value().type == "signal") {
                it = procStates.erase(it);
            } else {
                ++it;
            }
        }
    } else {
        qDebug() << "Modo No Explicito";
        if (finished()) return;
        cycle++;
        ordenProcesados.clear();
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
            if (it.value().state == "ACCESSED"
                || it.value().state == "WAITING")  {
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

                procStates[pid].state = "ACCESSED";
                procStates[pid].resource = resource;
                procStates[pid].type = type;
                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                 if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else{
                qDebug() << pid << "waiting for "<< resource << "found sem at " << sem_count;
                nuevosPendientes.append(action);
                procStates[pid].state = "WAITING";
                procStates[pid].resource = resource;
                procStates[pid].type = type;
                 if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
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

                procStates[pid].state = "ACCESSED";
                procStates[pid].resource = resource;
                procStates[pid].type = type;


                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else {
                qDebug() << pid << "waiting for "<< resource << "found sem at " << sem_count;
                nuevosPendientes.append(action);

                procStates[pid].state = "WAITING";
                procStates[pid].resource = resource;
                procStates[pid].type = type;

                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            }
        }

        // 5. Guardar las acciones pendientes que siguen sin ejecutarse
        pendingActions = nuevosPendientes;

        for (auto it = procStates.begin(); it != procStates.end(); ++it) {
            qDebug() << "Ciclo" << cycle
                     << "- PID:" << it.key()
                     << "- Estado:" << it.value().state
                     << "- Recurso:" << it.value().resource
                     << "- Acción:" << it.value().type;
        }

        // 6. Guardar información del ciclo actual para cada proceso activo
        for (const auto& pid : processes.keys()) {
            if (procStates.contains(pid)) {
                timelineStates[pid][cycle] = procStates[pid].state;
                timelineResources[pid][cycle] = procStates[pid].resource;
                timelineActions[pid][cycle] = procStates[pid].type;
            } else {
                timelineStates[pid][cycle] = "IDLE";
                timelineResources[pid][cycle] = "";
                timelineActions[pid][cycle] = "";
            }
        }
    }

}

QVector<QString> Semaphore::procesosEnCicloActual() const {
    return ordenProcesados;
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



