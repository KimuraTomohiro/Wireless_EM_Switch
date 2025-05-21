#include <TWELITE>
#include <NWK_SIMPLE>
#include "main.hpp"
#include "lcd.hpp"
#include "LED.hpp"
#include "mode.hpp"
#include "communication.hpp"
#include "Timer_Interrupt.hpp"

//宣言部
extern const uint8_t LED_R = 11;    //ほかファイルから参照できるようにする
extern const uint8_t LED_G = 13;    //ほかファイルから参照できるようにする
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;


lcd lcd_control = lcd();    //LCD制御用のインスタンス

uint8_t rx_status = 0;
uint16_t rx_volt1 = 0;
uint16_t rx_volt2 = 0;
uint32_t rx_timestamp = 0;


/*
この変数でモードの切替を行う。
モード0: セットアップ中

モード1: マスター探索中
    リモコン側から信号を発信して、基板側が返答する形で検索を行う。基板側の返答が取れたらモード2に移行する。

モード2: 通常状態
    マスター側と疎通が取れている状態。定期的（0.5秒毎）にstatusの通信を行い、LCDの表示を更新する。
    3回連続で疎通確認が取れない場合（1.5秒間）、モード1に移行する。

モード3: 緊急停止状態（疎通なし）
    緊急停止ボタンが押された状態。
    モード1中にボタンが押されたときに移行する。疎通が取れるまで、0.2秒毎に緊急停止信号を送信し続ける。
    マスター側からの返答があったときはモード4に移行する。

モード4: 緊急停止状態（疎通あり）
    緊急停止ボタンが押された状態。
    モード2中にボタンが押されたときに移行する。0.5秒毎にstatusの通信を行い、LCDの表示を更新する。
    3回連続で疎通確認が取れない場合（1.5秒間）、モード3に移行する。
    緊急停止ボタンを3秒長押しで緊急停止状態を解除し、モード2に移行する。


*/

void setup() {

    pinMode(LED_R, OUTPUT);    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    the_twelite.app.use<TIMER_INTERRUPT>(); //タイマー割り込みでLCDの更新を行うためのアプリケーションビヘイビアを登録

    Timer0.setup();
    Timer0.begin(10,true,false); //LCDの表示更新用のタイマー0を5Hz、割り込みあり、PWM出力なしでスタート

    Timer1.setup();
    Timer1.begin(2,true,false); //LEDの点滅制御用タイマー1を2Hz、割り込みあり、PWM出力なしでスタート

    Timer2.setup();
    Timer2.begin(2,true,false); //通信用・疎通確認用タイマー2を2Hz、割り込みあり、PWM出力なしでスタート
    
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

    change_mode(0);

    delay(500);

    Serial <<"--- Wireless Emergency Switch ---"<< crlf;
    Serial <<"Nagoya Institute of Technology NITRo & SAZANKA"<< crlf;
    Serial <<"Presented by Kimura Tomohiro"<< crlf;
    Serial <<"https://github.com/KimuraTomohiro/Wireless_EM_Switch"<< crlf;

    change_mode(1);

}

/*** loop procedure (called every event) */
void loop() {





    // sprintf(lcd_data_buf1,"ABCDEFGH");

	// if(digitalRead(EM_SW) == PIN_STATE::HIGH){
    //     sprintf(lcd_data_buf2,"ON");

	// }else{
    //     sprintf(lcd_data_buf2,"OFF");
	// }

}





