#include "mutexsync.h"
#include <QDebug>

MutexSync::MutexSync(const QString &procFile, const QString &resFile, const QString &actFile)
    : cycle(-1), maxCycle(0),
    resources(resFile),
    actions(actFile),
    proc(procFile)
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
        if (tipo == "ACQUIRE" || tipo == "RELEASE") {
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

void MutexSync::simulateNext() {
    if (modoExplicito){
        if (finished()) return;
        cycle++;
        ordenProcesados.clear();

        QVector<Action> nuevosPendientes;
        QSet<QString> procesadosEsteCiclo;

        // 1. Procesar RELEASING explícito
        for (const auto& action : actions.actionsList) {
            if (action.cycle != cycle) continue;
            if (action.action.toLower() == "release") {
                QString pid = action.pid;
                QString resource = action.resource;
                QString idAccion = pid + "|" + resource + "|" + action.action + "|" + QString::number(action.cycle);

                if (accionesEjecutadas.contains(idAccion)) continue;
                lockedResources.remove(resource); // liberar explícitamente
                accionesEjecutadas.insert(idAccion);
                procStates[pid] = {"IDLE", resource, "release"};
                procesadosEsteCiclo.insert(pid);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
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

            if (action.action.toLower() == "release") continue; // ya procesado arriba

            QString pid = action.pid;
            QString resource = action.resource;
            QString type = action.action.toLower();
            QString idAccion = pid + "|" + resource + "|" + type + "|" + QString::number(action.cycle);

            if (accionesEjecutadas.contains(idAccion)) continue;
            if (procesadosEsteCiclo.contains(pid)) continue;

            if (!lockedResources.contains(resource)) {
                lockedResources.insert(resource);
                procStates[pid] = {"ACCESSED", resource, type};
                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else {
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

        // DEBUG opcional
        for (auto it = procStates.begin(); it != procStates.end(); ++it) {
            qDebug() << "Ciclo" << cycle
                     << "- PID:" << it.key()
                     << "- Estado:" << it.value().state
                     << "- Recurso:" << it.value().resource
                     << "- Acción:" << it.value().type;
        }

        // 5. Ahora sí, limpiar procStates si ya no están activos
        for (auto it = procStates.begin(); it != procStates.end(); ) {
            if (it.value().state == "ACCESSED" || it.value().state == "WAITING" || it.value().type == "release") {
                it = procStates.erase(it);
            } else {
                ++it;
            }
        }
    }
    else{
        if (finished()) return;
        cycle++;
        ordenProcesados.clear();

        // 1. Liberar recursos usados en el ciclo anterior
        QSet<QString> liberados;
        for (auto it = procStates.begin(); it != procStates.end(); ++it) {
            if (it.value().state == "ACCESSED") {
                liberados.insert(it.value().resource);
            }
        }
        for (const auto& res : liberados) {
            lockedResources.remove(res);
        }

        // 2. Eliminar procesos que accedieron o estaban esperando (para reintentar sus acciones)
        for (auto it = procStates.begin(); it != procStates.end(); ) {
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
            QString type = action.action;
            QString idAccion = pid + "|" + resource + "|" + QString::number(action.cycle);

            if (accionesEjecutadas.contains(idAccion)) continue;
            if (procesadosEsteCiclo.contains(pid)) continue;

            if (!lockedResources.contains(resource)) {
                lockedResources.insert(resource);
                procStates[pid] = {"ACCESSED", resource, type};
                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else {
                nuevosPendientes.append(action);
                procStates[pid] = {"WAITING", resource, type};
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            }
        }

        // 4. Procesar nuevas acciones del ciclo actual
        for (const auto& action : actions.actionsList) {
            if (action.cycle != cycle) continue;

            QString pid = action.pid;
            QString resource = action.resource;
            QString type = action.action;
            QString idAccion = pid + "|" + resource + "|" + type + "|" + QString::number(action.cycle);

            if (accionesEjecutadas.contains(idAccion)) continue;
            if (procesadosEsteCiclo.contains(pid)) continue;

            if (!lockedResources.contains(resource)) {
                lockedResources.insert(resource);
                procStates[pid] = {"ACCESSED", resource, type};
                procesadosEsteCiclo.insert(pid);
                accionesEjecutadas.insert(idAccion);
                if (!ordenProcesados.contains(pid)) ordenProcesados.append(pid);
            } else {
                nuevosPendientes.append(action);
                procStates[pid] = {"WAITING", resource, type};
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

QVector<QString> MutexSync::procesosEnCicloActual() const {
    return ordenProcesados;
}



int MutexSync::currentCycle() const {
    return cycle;
}

bool MutexSync::finished() const {
    return pendingActions.isEmpty() && procStates.isEmpty();
}

QString MutexSync::getStateForProcess(const QString &pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].state;
    }
    return "IDLE";
}

QString MutexSync::getResourceForProcess(const QString& pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].resource;
    }
    return "";
}

QString MutexSync::getActionTypeForProcess(const QString& pid) const {
    if (procStates.contains(pid)) {
        return procStates[pid].type;
    }
    return "";
}



