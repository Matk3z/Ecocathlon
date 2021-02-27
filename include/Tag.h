#ifndef TAG_H
#define TAG_H

#include <TimeLib.h>

class Tag
{
private:
    int OrderIndex = 1;
    int secEnd = 0;
public:
    int tagID;
    int tagType;
    int tagData;
    bool baliseComplete = false;
    bool QuestionHasStarted;
    int secStart = 0;

    int timeTocomplete;

    Tag();
    void StartQuestion(time_t startTime);
    int GetTimeToComplete(time_t endTime);
    void EndQuestion();
    bool TestQCM(int answerData);
    bool TestOrder(int answerData);
    void BaliseFound();
};

#endif