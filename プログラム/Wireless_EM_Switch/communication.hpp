
extern const uint32_t APP_ID;
extern const uint8_t CHANNEL;
extern bool communication_established;

void vTransmit(uint32_t addr, uint8_t data);
void on_rx_packet(packet_rx& rx, bool_t &handled);