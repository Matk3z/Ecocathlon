#include "ctimer.h"

Ctimer::Ctimer(/* args */)
{
}

Ctimer::~Ctimer()
{
}

bool Ctimer::AddTimer(int startTime, int lenght, void (&function)()){

    if((currentIndex = CheckIndex()) == (-1))   return false;

    endTime[currentIndex] = startTime + lenght;
    functions[currentIndex] = function;

    return true;
}

void Ctimer::UpdateTimer(int currentTime){

    for(int i = 0; i < 5;i++){
        if(endTime[i] != 0){
            if(endTime[i] - currentTime <= 0){
                endTime[i] = 0;
                functions[i]();
            }
        }

    }
}

void Ctimer::ResetTimer(){
    for(int i = 0; i < maxtimer; i++){
        endTime[i] = 0;
    }
}

int Ctimer::CheckIndex(){

    for(int i = 0; i < maxtimer; i++){

        if(endTime[i] == 0){
            return i;
        }
    }
    return (-1);
}