#include <TWELITE>
#include <NWK_SIMPLE>
#include "main.hpp"
#include "lcd.hpp"
#include "LED.hpp"
#include "communication.hpp"
#include "Interrupt.hpp"


const uint8_t LED_R = 11;    //ほかファイルから参照できるようにする
const uint8_t LED_G = 13;    //ほかファイルから参照できるようにする
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;

extern bool release_flg;
extern uint8_t timer3_count;




lcd lcd_control = lcd();    //LCD制御用のインスタンス


uint8_t rx_status = 0;
uint16_t rx_volt1 = 0;
uint16_t rx_volt2 = 0;
uint32_t rx_timestamp = 0;
uint32_t after_release_count = 10;

CURRENT_MODE current_mode = NORMAL;

void setup() {

    the_twelite.app.use<INTERRUPT>(); //割り込みのアプリケーションビヘイビアを登録
    pinMode(LED_R, OUTPUT_INIT_HIGH);    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる
    pinMode(LED_G, OUTPUT_INIT_HIGH);
    pinMode(EM_SW,INPUT_PULLDOWN);
    pinMode(RESET_SW,INPUT_PULLDOWN);

    attachIntDio(EM_SW,PIN_INT_MODE::RISING);
    
    Timer0.setup();
    Timer0.begin(3,true,false); //LCDの表示更新用のタイマー0を3Hz、割り込みあり、PWM出力なしでスタート

    Timer1.setup();
    Timer1.begin(2,true,false); //LEDの点滅制御用タイマー1を2Hz、割り込みあり、PWM出力なしでスタート

    Timer2.setup();
    Timer2.begin(2,true,false); //通信用のタイマー2を2Hz、割り込みあり、PWM出力なしでスタート

    Timer3.setup(); //長押しの確認用


    
    Wire.begin(WIRE_CONF::WIRE_400KHZ,false);
    lcd_control.setup();

    	// the twelite main class
	the_twelite
		<< TWENET::appid(APP_ID)    // set application ID (identify network group)
		<< TWENET::channel(CHANNEL) // set channel (pysical channel)
		<< TWENET::rx_when_idle();  // open receive circuit (if not set, it can't listen packts from others)

	// Register Network
	auto&& nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
	nwksmpl << NWK_SIMPLE::logical_id(0xFE) // set Logical ID. (0xFE means a child device with no ID)
	        << NWK_SIMPLE::repeat_max(3);   // can repeat a packet up to three times. (being kind of a router)

	the_twelite.begin(); // start twelite!

    delay(500);

    Serial <<"--- Wireless Emergency Switch ---"<< crlf;
    Serial <<"Nagoya Institute of Technology NITRo & SAZANKA"<< crlf;
    Serial <<"Presented by Kimura Tomohiro"<< crlf;
    Serial <<"https://github.com/KimuraTomohiro/Wireless_EM_Switch"<< crlf;

}


uint8_t switch_stable_count = 0;
bool timer_change_flg = false;

void loop() {

    if((current_mode == EMERGENCY_MAIN) && (digitalRead(EM_SW) == PIN_STATE::HIGH)){
        timer_change_flg = true;
        Timer0.end();
        Timer3.begin(4,true,false);

    }else if((current_mode == RELEASE) && (digitalRead(EM_SW) == PIN_STATE::LOW)){//RELEASEに入ってからボタンを離した場合元のモードに戻る
        current_mode == EMERGENCY_MAIN;

    }else{
        if(timer_change_flg == true){ //タイマが切り替わった最初の一回のみ実行する
            lcd_control.clean();
        }

        Timer0.begin(3,true,false);
        timer_change_flg = false;
        Timer3.end();
        timer3_count = 0;
    }

}





