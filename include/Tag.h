#ifndef TAG_H
#define TAG_H

#include <TimeLib.h>

class Tag
{
private:
    int OrderIndex = 1;
public:
    int tagID;
    int tagType;
    int tagData;
    bool baliseComplete = false;
    bool QuestionHasStarted;

    time_t QuestionStartTime;
    int timeTocomplete;


    void StartQuestion(time_t startTime);
    void EndQuestion();
    bool TestQCM(int answerData);
    bool TestOrder(int answerData);
    void BaliseFound();
};

#endif