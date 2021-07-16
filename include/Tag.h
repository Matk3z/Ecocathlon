#ifndef TAG_H
#define TAG_H

#include <TimeLib.h>

class Tag
{
private:
    int OrderIndex = 1;
    int secEnd = 0;
    int tagID;
    int tagType;
    int tagData;
    bool QuestionHasStarted;
    bool baliseComplete;
public:


    int secStart = 0;

    int timeTocomplete;

    int GetID(){
        return tagID;
    }
    void SetID(int id){
        tagID = id;
    }
    int GetType(){
        return tagType;
    }
    int GetData(){
        return tagData;
    }
    bool GetHasStarted(){
        return QuestionHasStarted;
    }
    bool GetBaliseCompletionState(){
        return baliseComplete;
    }

    Tag();
    void SetTagInformation(int id, int type, int data);
    void StartQuestion(time_t startTime);
    int GetTimeToComplete(time_t endTime);
    void EndQuestion();
    bool TestQCM(int answerData);
    bool TestOrder(int answerData);
    void BaliseFound();
};

#endif