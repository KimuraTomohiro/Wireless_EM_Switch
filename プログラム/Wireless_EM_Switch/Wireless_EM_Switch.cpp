#include <TWELITE>
#include "lcd.hpp"
#include "Timer_Interrupt.hpp"

lcd lcd_control = lcd();    //LCD制御用のインスタンス

const uint8_t LED_R = 11;
const uint8_t LED_G = 13;
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;


extern char lcd_data_buf1[10];  //LCDディスプレイ一行目用のデータバッファ（実際には8までしか使わない）
extern char lcd_data_buf2[10];  //LCDディスプレイ一行目用のデータバッファ（実際には8までしか使わない）



void setup() {
    the_twelite.app.use<TIMER_INTERRUPT>(); //タイマー割り込みでLCDの更新を行うためのアプリケーションビヘイビアを登録

    Timer0.setup();
    Timer0.begin(10,true,false); //タイマー0を5Hz、割り込みあり、PWM出力なしでスタート
    
    Wire.begin(WIRE_CONF::WIRE_400KHZ,false);
    lcd_control.setup();

    pinMode(LED_R, OUTPUT);    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    

    digitalWrite(LED_R, HIGH); // TURN LED_R OFF
    digitalWrite(LED_G, LOW); // TURN LED_G ON


    Serial <<"--- Wireless Emergency Switch ---"<< crlf;
    Serial <<"Nagoya Institute of Technology NITRo & SAZANKA"<< crlf;
    Serial <<"Presented by Kimura Tomohiro"<< crlf;
    Serial <<"https://github.com/KimuraTomohiro/Wireless_EM_Switch"<< crlf;


}

/*** loop procedure (called every event) */
void loop() {

    sprintf(lcd_data_buf1,"ABCDEFGH");

	if(digitalRead(EM_SW) == PIN_STATE::HIGH){
		digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
        sprintf(lcd_data_buf2,"ON");

	}else{
		digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
        sprintf(lcd_data_buf2,"OFF");
	}

}





