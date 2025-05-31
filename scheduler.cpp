#include "scheduler.h"

scheduler::scheduler(const QString &procPath, int met)
    :  t(0), stack(),timeline(), snapshot(procPath) {
    if (met >= 0 & met <=4){
        method = met;
    } else{
        method = 0;
    }
}

void scheduler::calcNext(){
    // poner sus ifs para sus metodos jsjs
    if (method == 4){
        nextPS();
    }
    t++;
}

void scheduler::nextPS(){

}

void scheduler::nextFIFO(){

}

void scheduler::nextSJF(){

}

void scheduler::nextSRT(){

}

void scheduler::nextRR(){

}


