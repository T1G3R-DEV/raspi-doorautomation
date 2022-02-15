#include "ServoPWM.h"
#include "wiringPi.h"
#include <iostream>

ServoPWM::ServoPWM(int pin)
{
    //Set pins and pinmode
    m_pin = pin;
    pinMode(m_pin, PWM_OUTPUT);
    //setup PWM
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange (2000);
    pwmSetClock (192);
    pwmWrite(m_pin,90);

}
void ServoPWM::update(bool s_open)
{

    //std::cout << "PWM-Update !\n";


    //store current state
    m_state = s_open;
    // set pwm depending if open/closed expected
    if(s_open)
    {
        pwmWrite(m_pin,90);
    }
    else
    {
        pwmWrite(m_pin,200);
    }


}

ServoPWM::~ServoPWM()
{
    //dtor
}
