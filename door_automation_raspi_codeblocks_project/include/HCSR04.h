#ifndef HCSR04_H
#define HCSR04_H

#include <wiringPi.h>
#include <ctime>


class HCSR04
{
    public:
        //set pins/pinmodes
        HCSR04(int PIN_TRIG, int PIN_ECHO);
        virtual ~HCSR04();
        //get distance in cm
        double measure();

    protected:

    private:
        //pins os this instance stored
        int m_PIN_TRIG;
        int m_PIN_ECHO;
};

#endif // HCSR04_H
