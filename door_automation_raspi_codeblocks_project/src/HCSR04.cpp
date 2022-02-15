#include "HCSR04.h"
#include <iostream>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

//namespaces for time
using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

HCSR04::HCSR04(int PIN_TRIG, int PIN_ECHO)
{
    //set pins und pinmode
    m_PIN_TRIG = PIN_TRIG;
    m_PIN_ECHO = PIN_ECHO;
    pinMode(m_PIN_TRIG, OUTPUT);
    pinMode(m_PIN_ECHO, INPUT);
}

HCSR04::~HCSR04()
{
    //dtor
}

double HCSR04::measure()
{

    //Send trig pulse
        digitalWrite(m_PIN_TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(m_PIN_TRIG, LOW);

        time_t start_time = time(nullptr);//starttime for  the case, no echo was found!

        //Wait for echo start
        while(digitalRead(m_PIN_ECHO) == LOW && difftime(time(nullptr), start_time)<=2);// && difftime(time(nullptr), start_time)<=2 is for the case, no echo was found!

        //Wait for echo end
        long startTime = micros();
        while(digitalRead(m_PIN_ECHO) == HIGH  && difftime(time(nullptr), start_time)<=2);
        long travelTime = micros() - startTime;

        //Get distance in cm
        return travelTime / 58;

}

