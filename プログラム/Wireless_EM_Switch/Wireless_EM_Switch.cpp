#include <TWELITE>
//#include "lcd.h"

const uint8_t LED_R = 11;
const uint8_t LED_G = 13;
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;

int iLedCounter = 0;


/*** the setup procedure (called on boot) */

void setup() {


    delay(50);
    
    bool a = false;
    Wire.begin(WIRE_CONF::WIRE_100KHZ,false);

    lcd::setup();

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    digitalWrite(LED_R, HIGH); // TURN LED_R OFF
    digitalWrite(LED_G, LOW); // TURN LED_G ON

    Serial << "--- Wireless Emergency Switch ---" << crlf;
    Serial << a << crlf;
}

/*** loop procedure (called every event) */
void loop() {



	if(digitalRead(EM_SW) == PIN_STATE::HIGH){
		digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
	}else{
		digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
	}
}


