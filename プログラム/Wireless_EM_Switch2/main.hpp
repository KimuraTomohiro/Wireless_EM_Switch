extern const uint8_t EM_SW;
extern const uint8_t RESET_SW;
extern uint8_t rx_status;
extern uint16_t rx_volt1;
extern uint16_t rx_volt2;
extern uint32_t rx_timestamp;
extern const uint8_t LED_R;    //ほかファイルから参照できるようにする
extern const uint8_t LED_G;    //ほかファイルから参照できるようにする
extern char EM_btn_wait_status;
extern uint32_t after_release_count;

enum CURRENT_MODE{
    NORMAL,
    EMERGENCY_MAIN,
    EMERGENCY_SUB,
    RELEASE
};

extern CURRENT_MODE current_mode;