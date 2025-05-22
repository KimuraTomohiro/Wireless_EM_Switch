#include <TWELITE>
#include "Interrupt.hpp" // ビヘイビア定義ファイル
#include "lcd.hpp"
#include "LED.hpp"
#include "communication.hpp"
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

    handled = true;

}



MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    LCD_Timer_Control();
    //本来は高速で復帰する必要のある割り込み動作にLCDの更新という重い動作を入れるのはよろしくないが、
    // 時間的正確性がそこまで重要視されるものでもないので利便性のためにここで処理する。

    handled = true;
}


MWX_TIMER_INT(1, uint32_t arg, uint8_t& handled){
    Timer_LED_Control();
    handled = true;
}


/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/