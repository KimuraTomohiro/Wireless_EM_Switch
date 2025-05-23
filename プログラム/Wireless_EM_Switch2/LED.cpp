#include <TWELITE>
#include "LED.hpp"

char LED_mode = 0;
extern const uint8_t LED_R;
extern const uint8_t LED_G;

bool LED_R_ON = false;
bool LED_G_ON = false;


void Timer_LED_Control(LED_color color,LED_flash flash){ 
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);

    
    switch(flash){
        case OFF:
        break;

        case ON:
            if(color == RED){
                digitalWrite(LED_R, LOW);
            }else{
                digitalWrite(LED_G, LOW);
            }
        break;
        
        case BLINK:
            Timer1.change_hz(2);
            //赤を点滅
            if(color == RED){
                if(LED_R_ON == true){
                    digitalWrite(LED_R,HIGH);
                    LED_R_ON = false;
                }else{
                    digitalWrite(LED_R,LOW);
                    LED_R_ON = true;
                }
            }else{
            //緑を点滅
                if(LED_G_ON == true){
                    digitalWrite(LED_G,HIGH);
                    LED_G_ON = false;
                }else{
                    digitalWrite(LED_G,LOW);
                    LED_G_ON = true;
                }
            }
        break;

        case BLINK_FAST:
            Timer1.change_hz(10);
            //赤を点滅
            if(color == RED){
                if(LED_R_ON == true){
                    digitalWrite(LED_R,HIGH);
                    LED_R_ON = false;
                }else{
                    digitalWrite(LED_R,LOW);
                    LED_R_ON = true;
                }
            }else{
            //緑を点滅
                if(LED_G_ON == true){
                    digitalWrite(LED_G,HIGH);
                    LED_G_ON = false;
                }else{
                    digitalWrite(LED_G,LOW);
                    LED_G_ON = true;
                }
            }
            break;

        
    }

}