#include <TWELITE>
#include "LED.hpp"

char LED_mode = 0;
extern const uint8_t LED_R;
extern const uint8_t LED_G;

bool LED_R_ON = false;
bool LED_G_ON = false;

void Timer_LED_Control(){
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);

    switch(LED_mode){
        case 0:
            digitalWrite(LED_R, HIGH);//消灯
            digitalWrite(LED_G, HIGH);
            break;
        
        case 1:
            digitalWrite(LED_R, LOW);//赤を点灯
            digitalWrite(LED_G, HIGH);
            break;

        case 2:
            digitalWrite(LED_R, HIGH);
            digitalWrite(LED_G, LOW);//緑を点灯
            break;

        case 3:
        case 4:
            digitalWrite(LED_G, HIGH);
            
            //赤を点滅
            if(LED_R_ON){
                digitalWrite(LED_R,HIGH);
                LED_R_ON = false;
            }else{
                digitalWrite(LED_R,LOW);
                LED_R_ON = true;
            }

            break;

    }

}