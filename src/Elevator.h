#ifndef ELEVATOR_H
#define ELEVATOR_H
#include <iostream>

class Elevator {
    public:
        Elevator();
        ~Elevator();
    private:
        int doorState;
        int peopleCount;
};
#endif
