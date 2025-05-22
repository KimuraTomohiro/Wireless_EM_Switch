
extern const uint32_t APP_ID;
extern const uint8_t CHANNEL;
extern bool communication_established_check_flg;
extern uint8_t rx_status;
extern uint16_t rx_volt1;
extern uint16_t rx_volt2;
extern uint32_t rx_timestamp;

void vTransmit(uint32_t addr, uint8_t data, uint8_t retry = 3);
void on_rx_packet(packet_rx& rx, bool_t &handled);