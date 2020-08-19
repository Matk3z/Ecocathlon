#ifndef CTIMER_H
#define CTIMER_H

class Ctimer
{
private:
    /* data */
    int maxtimer = 10;
    int endTime[10];
    void (*functions[10])();
    int currentIndex = 0;
    int CheckIndex();

public:

    Ctimer(/* args */);
    bool AddTimer(int startTime, int lenght, void (&function)());
    void UpdateTimer(int currentTime);
    void ResetTimer();
    ~Ctimer();
};


#endif