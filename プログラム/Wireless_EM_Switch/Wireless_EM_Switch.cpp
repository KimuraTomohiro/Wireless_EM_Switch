#include <TWELITE>
#include "lcd.hpp"

lcd lcd_control = lcd();

const uint8_t LED_R = 11;
const uint8_t LED_G = 13;
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;


extern char lcd_data_buf1[10];
extern char lcd_data_buf2[10];

void setup() {
    Timer0.setup();
    Timer0.begin(1,true,false);
    Timer0.change_hz(0,50);
    
    Wire.begin(WIRE_CONF::WIRE_400KHZ,false);
    lcd_control.setup();

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる

    digitalWrite(LED_R, HIGH); // TURN LED_R OFF
    digitalWrite(LED_G, LOW); // TURN LED_G ON

    Serial << "--- Wireless Emergency Switch ---" << crlf;


}


/*** loop procedure (called every event) */
void loop() {

    int vol = 12;
    sprintf(lcd_data_buf2,"Vol:%d",vol);

	if(digitalRead(EM_SW) == PIN_STATE::HIGH){
		digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
        sprintf(lcd_data_buf1,"ON");

	}else{
		digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
        sprintf(lcd_data_buf1,"OFF");
	}

    if (Timer0.available()){
        LCD_Timer_Control();
    }

}





