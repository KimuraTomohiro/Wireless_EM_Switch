extern const uint8_t EM_SW;
extern const uint8_t RESET_SW;
extern uint8_t rx_status;
extern uint16_t rx_volt1;
extern uint16_t rx_volt2;
extern uint32_t rx_timestamp;
extern const uint8_t LED_R;    //ほかファイルから参照できるようにする
extern const uint8_t LED_G;    //ほかファイルから参照できるようにする
extern char current_mode;
extern char EM_btn_wait_status;

enum CURRENT_MODE{
    NOMAL,
    EMERGENCY
};

extern CURRENT_MODE mode;