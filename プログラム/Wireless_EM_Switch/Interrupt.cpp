#include <TWELITE>
#include "Interrupt.hpp" // ビヘイビア定義ファイル
#include "lcd.hpp"
#include "LED.hpp"
#include "communication.hpp"
#include "mode.hpp"
#include "main.hpp"

extern lcd lcd_control;

//ビヘイビアに関する参考はここ
//https://mwx.twelite.info/api-reference/behavior

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/

MWX_DIO_INT(PIN_DIGITAL::DIO18,uint32_t arg, uint8_t& handled){ //停止ボタンが押されたときに割り込み動作を行う。

    EM_btn_wait_status = 1;
    detachIntDio(EM_SW);    //緊急停止状態に入ったら、ボタンの割り込みは一旦解除する
    if(current_mode == 1){
        change_mode(3);
    }else if(current_mode == 2){
        change_mode(4);
    }
    handled = false;

}

MWX_DIO_EVENT(PIN_DIGITAL::DIO18, uint32_t arg){


}



MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    LCD_Timer_Control();
    //本来は高速で復帰する必要のある割り込み動作にLCDの更新という重い動作を入れるのはよろしくないが、
    // 時間的正確性がそこまで重要視されるものでもないので利便性のためにここで処理する。

    handled = false;
}

MWX_TIMER_EVENT(0, uint32_t arg){

    Serial << format("モード: %d",current_mode) << crlf;


}


MWX_TIMER_INT(1, uint32_t arg, uint8_t& handled){
    Timer_LED_Control();
    handled = true;
}

MWX_TIMER_INT(2, uint32_t arg, uint8_t& handled){   //定期通信を3Hzで行う
    handled = false;
}


MWX_TIMER_EVENT(2, uint32_t arg){

    if(release_signal_send_flg == true){
        vTransmit(0x0,0x02);
        return;
    }


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

        rx_volt1 = 0;
        rx_volt2 = 0;
        rx_timestamp = 0;

        if(current_mode == 2){
            change_mode(1);
        }else if(current_mode == 4){
            change_mode(3);
        }
        
    }else{  //疎通確認が取れた場合、モード1の場合モード2に、モード3の場合モード4に変更する。
        if(current_mode == 1){
            current_mode = 2;
            change_mode(0); //緊急停止解除プロセスのモード0を経由してモード2やモード4に戻る
        }else if(current_mode == 3){
            current_mode = 4;
            change_mode(0);
        }

    }
    communication_established_check_flg = false;

    handled = false;
}



char timer_count = 0;
MWX_TIMER_EVENT(3, uint32_t arg){

if(EM_btn_wait_status == 0){
    //緊急停止動作時、ボタンを長押しで解除できるようにする。
    if(digitalRead(EM_SW) == PIN_STATE::HIGH && (current_mode == 3 || current_mode == 4)){  //緊急停止状態でボタンが押されている時

        sprintf(lcd_data_buf1,"HOLD TO RELEASE");
        lcd_control.setCursor(1,0);
        for(char i=0;i<=timer_count;i++){
            lcd_control.print("##");
        }
        timer_count++;

        if(timer_count > 3){
            lcd_control.clean();
            //lcd_data_buf1に格納する方法はLCDの更新に割り込みを使っているので、他の割り込み中は使えない。

            timer_count = 0;
            if(current_mode == 3){
                current_mode = 1;
                change_mode(0);
            }else if(current_mode == 4){
                current_mode = 2;
                change_mode(0);
            }

        }

        }else if(timer_count != 0){  //ボタンを押したが途中で離した場合もとのモードに戻る。

            timer_count = 0;   
            change_mode(current_mode);

        }

    }

}


/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/