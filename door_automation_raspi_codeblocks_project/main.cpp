#include <iostream>     // console outputs
#include <ctime>       // time for delays, ...
#include <wiringPi.h>   // gpio control



#include "HCSR04.h"
#include "INA219.h"
#include "ServoPWM.h"


#define COPY_RIGHT "\n\n\tDOOR AUTOMATION\n\t(c)2021 Daniel-T1G3R\n\tt1g3r.dev@gmail.com\n\n"

//define inputs
#define PIN_HC_00_TRIG  04
#define PIN_HC_00_ECHO  05

#define PIN_HC_01_TRIG  06
#define PIN_HC_01_ECHO  10

#define PIN_S_00        11
#define PIN_S_01        31

#define PIN_INA_00_SDA  08
#define PIN_INA_00_SCL  09

//define outputs
#define PIN_LED_R       27
#define PIN_LED_G       28
#define PIN_LED_B       29

#define PIN_PWM_SERVO   1



//define namespce for cpp std for basic functions like iostream
using namespace std;

//state is used to define in which state (open, closed, init, the program is!.
int state = 0;
/*
* States
* 0 = init
* 1 = closed
* 2 = open
* -1 = overcurrent
*
*
*/

//input uebergabewerte  von getInputs();
bool HC_00  = false;
bool HC_01  = false;
bool INA_00 = false;
bool S_00   = false;
bool S_01   = false;

//output uebergabewerte von setOutputs();
bool servo  = false;
bool LED_R  = false;
bool LED_G  = false;
bool LED_B  = false;

//time until hold open
time_t time_releasepoint;
//globale pointer zum zugriff auf die instanzen der Sensoren die in initSensors() erstellst worden sind
INA219* INA_PTR;

HCSR04* HC_00_PTR;
HCSR04* HC_01_PTR;

ServoPWM* PWM_PTR;

//deklaration der Funktionen
bool initSensors();
bool getInputs();
bool stateMachine();
bool setOutputs();


int main()
{
    cout << COPY_RIGHT << endl << "Initializing" << endl;

    initSensors();

    while(1) //dauerschleife druchleuft alle 100-200 ms und scannt die sensoren -> setzt die Aktoren
    {
        getInputs();

        stateMachine();


        setOutputs();

        delay(100);
    }

    return 0;
}

bool initSensors() //initialisieren der Sensoren und Aktoren
{
    //initialisieren der wiringPI Library für gpio zugriff
    wiringPiSetup();

    //Buttons
    pinMode(PIN_S_00, INPUT);
    pinMode(PIN_S_01, INPUT);

    //LEDs
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);

    //HCSR04s & INA219 oben
    float SHUNT_OHMS = 0.1;
    float MAX_EXPECTED_AMPS = 3.2;
    static INA219 INA_OBJ(SHUNT_OHMS, MAX_EXPECTED_AMPS);
    INA_OBJ.configure(RANGE_16V, GAIN_8_320MV, ADC_12BIT, ADC_12BIT);
    INA_PTR = &INA_OBJ;


    static HCSR04 HC_00_OBJ(PIN_HC_00_TRIG, PIN_HC_00_ECHO);
    HC_00_PTR = &HC_00_OBJ;

    static HCSR04 HC_01_OBJ(PIN_HC_01_TRIG, PIN_HC_01_ECHO);
    HC_01_PTR = &HC_01_OBJ;

        //servo
    static ServoPWM PWM_OBJ(PIN_PWM_SERVO);
    PWM_PTR = &PWM_OBJ;


}

bool getInputs()
{
//inputs einlesen, verarbeiten und entsprechend in die globalen uebergabevariablen uebergeben
    float distance0=0;
    float distance1=0;
    float current = 0;


    S_00 = digitalRead(PIN_S_00);
    S_01 = digitalRead(PIN_S_01);

    distance0 = HC_00_PTR->measure();
    distance1 = HC_01_PTR->measure();

    current = INA_PTR->current();

    cout << "S_00:" << S_00 << endl;
    cout << "S_01:" << S_01 << endl;
    cout << "distance0:" << distance0 << endl;
    cout << "distance1:" << distance1 << endl;
    cout << "current:" << current << endl;


    if(distance0 <= 50.0) // wenn abstand zu sensor kleiner 1m auslösen
        HC_00 = 1;
    else
        HC_00 = 0;

    if(distance1<= 50.0) // wenn abstand zu sensor kleiner 1m auslösen
        HC_01 = 1;
    else
        HC_01 = 0;


    if(current >=2.0)
        INA_00 = 1;
    else
        INA_00 = 0;




}

bool stateMachine()
{
    if(INA_00) state=-1; //Switch to error case!

    switch(state)
    {
        case 0:
        {
            //init State
            time_t result = time(nullptr);
            cout << "Running since: " <<asctime(localtime(&result)) << endl;
            state=1;

            servo  = false;
            LED_R  = true;
            LED_G  = true;
            LED_B  = true;


            break;
        }
        case 1:

            if(HC_00||HC_01)
            {
                                                // 30 sec offenhalten
                time_releasepoint = time(nullptr) + 30;
                state = 2;
            }
            if(S_00||S_01)
            {
                                                // 5 min offenhalten
                time_releasepoint = time(nullptr) + 60*5;
                delay(1000);
                state =  2;
            }


            servo  = false;
            LED_R  = false;
            LED_G  = false;
            LED_B  = true;

            break;
        case 2:

            if(S_00||S_01) //wen button gedrueckt sofort wieder schliessen
            {
                state =  1;
            }
            //wenn zeit abgelaufen wieder schließen
            if(time(nullptr)>= time_releasepoint) state=1;


            servo  = true;
            LED_R  = false;
            LED_G  = true;
            LED_B  = false;

            break;
        case -1:
        default:
            cout << "\n" << time(nullptr) << ":Error - INA219\n";
            LED_R = true;
            LED_G = false;
            LED_B = false;
            servo = !servo;  //weg von dem was er bisdahin gemacht hat -> damit er nichts kaputt macht
            setOutputs();//warte fehlerzeit ab
            delay(60000*5);//5min
             break;
    }
    //Ausgabe der Errechneten werte
    cout << "\n" << time(nullptr) <<endl;
    cout << "Servo:" << servo << endl;
    cout << "LED_R:" << LED_R << endl;
    cout << "LED_G:" << LED_G << endl;
    cout << "LED_B:" << LED_B << endl;

}

bool setOutputs()
{
    //Setzen der LEDs und des PWM
    digitalWrite(PIN_LED_R, LED_R);
    digitalWrite(PIN_LED_G, LED_G);
    digitalWrite(PIN_LED_B, LED_B);
    PWM_PTR->update(servo);
}
