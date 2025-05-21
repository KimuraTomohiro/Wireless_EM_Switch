#include <TWELITE>
#include "Interrupt.hpp" // ビヘイビア定義ファイル
#include "lcd.hpp"
#include "LED.hpp"
#include "communication.hpp"
#include "mode.hpp"

//ビヘイビアに関する参考はここ
//https://mwx.twelite.info/api-reference/behavior

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/

MWX_DIO_INT(PIN_DIGITAL::DIO18,uint32_t arg, uint8_t& handled){ //停止ボタンが押されたときに割り込み動作を行う。


    handled = false;

}

MWX_DIO_EVENT(PIN_DIGITAL::DIO18, uint32_t arg){

    if(current_mode == 1){
        change_mode(3);
    }else if(current_mode == 2){
        change_mode(4);
    }

}



MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    handled = false;
}

MWX_TIMER_EVENT(0, uint32_t arg){
    LCD_Timer_Control();
}

MWX_TIMER_INT(1, uint32_t arg, uint8_t& handled){
    Timer_LED_Control();
    handled = true;
}

MWX_TIMER_INT(2, uint32_t arg, uint8_t& handled){
    handled = false;
}


MWX_TIMER_EVENT(2, uint32_t arg){   //定期通信を3Hzで行う

    switch(current_mode){
        
        case 1:
        case 2:
            vTransmit(0x0, 0x01);
        break;

        case 3:
        case 4:
            vTransmit(0x0, 0x00);
        break;

    }
}

MWX_TIMER_INT(3, uint32_t arg, uint8_t& handled){
    //通信の疎通確認を行う。このブロックがタイマ2の1秒毎に呼ばれ、その間に通信があればcommunication_establishedがtrueになるので疎通確認が取れる。
    //その間に通信がない場合、通信が途絶したとみなしてモード2からモード1に戻る。モード4にいたらモード3に戻る。
    if(communication_established_check_flg == false){ 
        if(current_mode == 2){
            change_mode(1);
        }else if(current_mode == 4){
            change_mode(3);
        }
        
    }else{  //疎通確認が取れた場合、モード1の場合モード2に、モード3の場合モード4に変更する。
        if(current_mode == 1){
            change_mode(2);
        }else if(current_mode == 3){
            change_mode(4);
        }

    }
    communication_established_check_flg = false;

    handled = false;
}

/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/