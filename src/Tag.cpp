#include <Tag.h>
#include <TimeLib.h>


Tag::Tag(){
    tagID = 0;
    tagType = 0;
    tagData = 0;
    timeTocomplete = 0;

}

void Tag::StartQuestion(time_t startTime)
{

    if(QuestionHasStarted){
        return;
    }

    OrderIndex = 1;
    QuestionHasStarted = true;
    secStart = minute(startTime) * 60 + second(startTime);
}

int Tag::GetTimeToComplete(time_t endTime){
    secEnd = minute(endTime) * 60 + second(endTime);
    timeTocomplete = secEnd - secStart;
    return timeTocomplete;
}

void Tag::EndQuestion(){
    QuestionHasStarted = false;
}

bool Tag::TestQCM(int answerData)
{
    if(!QuestionHasStarted){
        return false;
    }
    if(answerData == 1){
        EndQuestion();
        baliseComplete = true;
        return true;
    }

    return false;
}

bool Tag::TestOrder(int answerData){

    if(!QuestionHasStarted){
        return false;
    }

    if(answerData == OrderIndex){
        if(OrderIndex == 4){
            baliseComplete = true;
            EndQuestion();
            return true;
        }
        OrderIndex++;

        return true;
    }
    else{
        return false;
    }

}

void Tag::BaliseFound(){
    baliseComplete = true;
}

