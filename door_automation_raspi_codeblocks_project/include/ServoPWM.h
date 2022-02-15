#ifndef SERVOPWM_H
#define SERVOPWM_H


class ServoPWM
{
    public:
        //set pins and pinmode =init
        ServoPWM(int pin);
        virtual ~ServoPWM();
        //to update pwm state (0/1)
        void update(bool s_open);

    protected:

    private:
        //stored pins for the instance
        bool m_state;
        bool m_pin;


};

#endif // SERVOPWM_H
