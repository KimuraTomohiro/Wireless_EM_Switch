#include <TWELITE>
#include "Timer_Interrupt.hpp" // ビヘイビア定義ファイル
#include "lcd.hpp"

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME TIMER_INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/

MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    handled = false;
}

MWX_TIMER_EVENT(0, uint32_t arg){
    LCD_Timer_Control();
}

/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/