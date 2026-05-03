#include "algorithm"
#include "cstring"
#include "stdio.h"


class Motion
{
private:
    float X,Y,Z,E= 0;
    float speed =0;
    float аccelerationX = 0;
    float аccelerationY = 0;
    float аccelerationZ = 0;
    float аccelerationE = 0;
    float MaxSpeedX = 0;
    float MaxSpeedY = 0;
    float MaxSpeedZ = 0;
    float MaxSpeedE = 0;

    float startX = 0;
    float startY = 0;
    float startZ = 0;
    float startE = 0;

    float endX = 0;
    float endY = 0;
    float endZ = 0;
    float endE = 0;
public:
    Motion(/* args */) = default;
};



class Sheduler
{
private:
    Motion Moves[10] = {};
    size_t        readPos  = 0;
    size_t        writePos = 0;
public:

    bool isEmpty();
    void Next();
    void NextMove(Motion element){



        memcpy(&(this->Moves[writePos]), &element, sizeof(Motion));
        writePos = (writePos + 1) % sizeof(Moves);
    }

    Sheduler(/* args */){}
    ~Sheduler(){}
};
