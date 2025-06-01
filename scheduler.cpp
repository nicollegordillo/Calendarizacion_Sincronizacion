#include "scheduler.h"
#include <climits>
#include <QDebug>
scheduler::scheduler(const QString &procPath, int met, int q)
    :  finished(false),queue(),timeline(), snapshot(procPath) {
    t = 0;
    if (met >= 0 & met <=4){
        method = met;
    } else{
        method = 0;
    }
    quantum = q;
}

QString scheduler::getExcecutedName(){
    return snapshot.names[timeline[t-1]];
}

QString scheduler::getColorByName(QString &name){
    int id = 0;
    for (int i = 0; i<snapshot.names.length(); i++){
        if (snapshot.names[i] == name){
            id = i;
            break;
        }
    }
    return snapshot.hexColor[id];
}

void scheduler::calculateNext(){
    if(method == 0){
        nextFIFO();
        return;
    }
    if(method == 1){
        nextSJF();
        return;
    }
    if(method == 2){
        nextSRT();
        return;
    }
    if(method == 3){
        nextRR();
        return;
    }
    if(method == 4){
        nextPS();
        return;
    }
}

void scheduler::nextPS(){
    // 0. Stop if finished
    bool bursts_left = false;
    for (int i = 0; i<snapshot.names.length(); i++){
        if (snapshot.burstTime[i]>0){
            bursts_left = true;
        }
    }

    if (!bursts_left){
        finished = true;
        return;
    }
    // 1. Agregar procesos nuevos
    // qDebug() << "T" << t;
    for (int i = 0; i<snapshot.names.length(); i++){
        if(snapshot.arrivalTime[i]==t){
            // qDebug() << "At" << t << "Came " << snapshot.names[i];
            queue.append(i);
        }
    }

    int min = INT_MAX;
    int min_id = 0;
    // Find next process
    for (int i = 0; i<queue.length(); i++){
        if (snapshot.priority[queue[i]] < min){
            min = snapshot.priority[queue[i]];
            min_id = queue[i];
        }
    }

    // Reduce burst time by 1 (time increment)
    snapshot.burstTime[min_id]-=1;

    // Eliminate from queue if burst time is 0
    if (snapshot.burstTime[min_id] == 0){
        queue.erase(std::remove_if(queue.begin(), queue.end(),
                                [min_id](int value) { return value == min_id; }),
                queue.end());
    }
    timeline.append(min_id);
    t++;
}

void scheduler::nextFIFO() {
    // 0. Verificar si ya no hay procesos pendientes
    bool bursts_left = false;
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.burstTime[i] > 0) {
            bursts_left = true;
            break;
        }
    }

    if (!bursts_left) {
        finished = true;
        return;
    }

    // 1. Agregar procesos nuevos que hayan llegado en el tiempo actual
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.arrivalTime[i] == t) {
            queue.append(i);
        }
    }

    // 2. Ejecutar el proceso al frente de la cola
    if (!queue.isEmpty()) {
        int current = queue.front();
        snapshot.burstTime[current] -= 1;
        timeline.append(current);

        // Si terminó su ráfaga, quitarlo de la cola
        if (snapshot.burstTime[current] == 0) {
            queue.removeFirst();
        }
    } else {
        // CPU ociosa
        timeline.append(-1);
    }

    t++;
}

void scheduler::nextSJF(){

}

void scheduler::nextSRT(){
    // 0. Verificar si hay procesos pendientes
    bool bursts_left = false;
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.burstTime[i] > 0) {
            bursts_left = true;
            break;
        }
    }

    if (!bursts_left) {
        finished = true;
        return;
    }

    // 1. Agregar procesos nuevos
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.arrivalTime[i] == t) {
            queue.append(i);
        }
    }

    // 2. Buscar el proceso con menor burst restante
    int min_time = INT_MAX;
    int selected = -1;
    for (int i = 0; i < queue.size(); i++) {
        int idx = queue[i];
        if (snapshot.burstTime[idx] < min_time) {
            min_time = snapshot.burstTime[idx];
            selected = idx;
        }
    }

    // 3. Ejecutar un ciclo de tiempo
    if (selected != -1) {
        snapshot.burstTime[selected] -= 1;
        timeline.append(selected);

        // Si terminó, quitarlo de la cola
        if (snapshot.burstTime[selected] == 0) {
            queue.erase(std::remove(queue.begin(), queue.end(), selected), queue.end());
        }
    } else {
        // No hay proceso disponible, añadir "idle"
        timeline.append(-1); // -1 puede representar CPU ociosa si lo manejas
    }

    t++;

}

void scheduler::nextRR(){
    // 0. Verificar si hay procesos pendientes
    bool bursts_left = false;
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.burstTime[i] > 0) {
            bursts_left = true;
            break;
        }
    }

    if (!bursts_left) {
        finished = true;
        return;
    }

    // 1. Agregar procesos nuevos
    for (int i = 0; i < snapshot.names.length(); i++) {
        if (snapshot.arrivalTime[i] == t) {
            queue.append(i);
        }
    }

    static int current_quantum = 0;
    static int current_process = -1;

    // 2. Si no hay proceso activo o terminó/cuántum expiró, elegir el siguiente
    if (current_process == -1 || current_quantum == 0 || snapshot.burstTime[current_process] == 0) {
        // Si terminó, eliminarlo de la cola
        if (current_process != -1 && snapshot.burstTime[current_process] == 0) {
            queue.erase(std::remove(queue.begin(), queue.end(), current_process), queue.end());
        } else if (current_process != -1 && current_quantum == 0) {
            // Si se agotó el quantum, moverlo al final de la cola
            queue.erase(std::remove(queue.begin(), queue.end(), current_process), queue.end());
            queue.append(current_process);
        }

        // Seleccionar siguiente proceso
        if (!queue.isEmpty()) {
            current_process = queue.front();
            current_quantum = quantum;
        } else {
            current_process = -1;
        }
    }

    // 3. Ejecutar el proceso
    if (current_process != -1) {
        snapshot.burstTime[current_process] -= 1;
        current_quantum -= 1;
        timeline.append(current_process);
    } else {
        // CPU ociosa
        timeline.append(-1);
    }

    t++;

}


