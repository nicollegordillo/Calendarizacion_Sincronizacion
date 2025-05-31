#include "scheduler.h"
#include <climits>
#include <QDebug>
scheduler::scheduler(const QString &procPath, int met)
    :  finished(false),queue(),timeline(), snapshot(procPath) {
    t = 0;
    if (met >= 0 & met <=4){
        method = met;
    } else{
        method = 0;
    }
}

QString scheduler::getExcecutedName(){
    return snapshot.names[timeline[t-1]];
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

void scheduler::nextFIFO(){

}

void scheduler::nextSJF(){

}

void scheduler::nextSRT(){

}

void scheduler::nextRR(){

}


