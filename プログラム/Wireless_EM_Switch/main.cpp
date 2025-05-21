#include <TWELITE>
#include <NWK_SIMPLE>
#include "main.hpp"
#include "lcd.hpp"
#include "LED.hpp"
#include "mode.hpp"
#include "communication.hpp"
#include "Interrupt.hpp"


const uint8_t LED_R = 11;    //ほかファイルから参照できるようにする
const uint8_t LED_G = 13;    //ほかファイルから参照できるようにする
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;


lcd lcd_control = lcd();    //LCD制御用のインスタンス


uint8_t rx_status = 0;
uint16_t rx_volt1 = 0;
uint16_t rx_volt2 = 0;
uint32_t rx_timestamp = 0;

bool first_loop = true;


/*
この変数でモードの切替を行う。
モード0: 緊急復帰動作

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
    the_twelite.app.use<INTERRUPT>(); //割り込みのアプリケーションビヘイビアを登録


    pinMode(LED_R, OUTPUT_INIT_HIGH);    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる
    pinMode(LED_G, OUTPUT_INIT_HIGH);
    pinMode(EM_SW,INPUT);
    attachIntDio(EM_SW,PIN_INT_MODE::RISING);


    Timer0.setup();
    Timer0.begin(3,true,false); //LCDの表示更新用のタイマー0を3Hz、割り込みあり、PWM出力なしでスタート

    Timer1.setup();
    Timer1.begin(2,true,false); //LEDの点滅制御用タイマー1を2Hz、割り込みあり、PWM出力なしでスタート

    Timer2.setup();
    Timer2.begin(3,true,false); //通信用タイマー2を2Hz、割り込みあり、PWM出力なしでスタート

    Timer3.setup();
    Timer3.begin(1,true,false); //疎通確認用タイマー3を1Hz、割り込みあり、PWM出力なしでスタート

    Timer4.setup(); //緊急停止解除信号のタイムアウト測定用タイマ

    
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

    change_mode(1);
}


uint8_t timer4_count = 0;
/*** loop procedure (called every event) */
void loop() {

    if(Timer4.available()){ 
        //モード0のみ時間で遷移するモードだが、関数内でループを回すとメインループに戻らずイベントが処理されない。
        //そのためここに処理を記述する。
        timer4_count ++;

        if(timer4_count > 5){
            if(communication_established_check_flg == true){
                sprintf(lcd_data_buf1,"Complete");
            }else{
                sprintf(lcd_data_buf1,"Timeout");
            }
            release_signal_send_flg = false;

        while(digitalRead(EM_SW) == PIN_STATE::HIGH){
            delay(100); //緊急停止からの復帰など、ボタンが押された状態の時に放されるまで待つ
        }
            delay(2000);
            Timer1.change_hz(2);
            change_mode(next_mode);
            timer4_count = 0;
            Timer4.end();

        }
    }

    //モード1とモード3の場合疎通が取れていないので一方的な送信のみになり、LCDの表示内容は変化しない。
    //しかしモード2とモード4の場合受信した情報から電圧情報を取得し表示に反映させる必要があるので、ここでその作業を行う。
    if(current_mode == 2 || current_mode == 4){   

    }

}





