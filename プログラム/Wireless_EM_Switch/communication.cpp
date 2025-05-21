#include <TWELITE>
#include <NWK_SIMPLE>
#include "communication.hpp"
#include "Interrupt.hpp"

const uint32_t APP_ID = 0x20297C9;  
const uint8_t CHANNEL = 12;
bool communication_established_check_flg;

/*
送受信データの詳細
・1バイト目
以下のコマンドを使用

子機から親機に0x00を送信：出力オフの指示（緊急停止）
子機から親機に0x01を送信：出力をそのまま維持(主に疎通確認用)
子機から親機に0x02を送信：出力を再開する指示

親機から子機に0x03を送信：出力オンの状態提示（通常動作時）
親機から子機に0x04を送信：出力オフの状態提示（緊急停止時）

・2,3バイト目
ADC1の出力を16ビットで格納(ADCは10bit出力)
子機から送信する際は0xFFFF固定

・4,5バイト目
ADC2の出力を16ビットで格納
子機から送信する際は0xFFFF固定

・6,7,8,9バイト目
タイムスタンプ

*/

const char* Serial_text[] = {
    "緊急停止",
    "疎通確認",
    "出力再開"
};






void vTransmit(uint32_t addr, uint8_t data) {

	if (auto&& pkt = the_twelite.network.use<NWK_SIMPLE>().prepare_tx_packet()) {
		// set tx packet behavior
		pkt << tx_addr(addr)  // 0..0xFF (LID 0:parent, FE:child w/ no id, FF:LID broad cast), 0x8XXXXXXX (long address)
			<< tx_retry(0x3) // set retry (0x3 send four times in total)
			<< tx_packet_delay(0,10,5); // send packet w/ delay (send first packet with randomized delay from 100 to 200ms, repeat every 20ms)
		// prepare packet payload
		pack_bytes(pkt.get_payload() // set payload data objects.
			, data
            , uint16_t(0xFFFF)
            , uint16_t(0xFFFF)
            , uint32_t(millis())
		);
	Serial << format("制御用送信データ: %x %s",data,Serial_text[data]) << crlf;
		// do transmit 
		pkt.transmit();
	}else{
		Serial << format("送信失敗") << crlf;
	}

}


void on_rx_packet(packet_rx& rx, bool_t &handled) {
	// rx >> Serial; // debugging (display longer packet information)

    communication_established_check_flg = true;

	expand_bytes(rx.get_payload().begin(), rx.get_payload().end()
				, rx_status       // 1byte of data
    			, rx_volt1        // 2byte of data (0..1023)
        		, rx_volt2        // 2byte of data (0..1023)
                , rx_timestamp    // 4byte of timestamp data
	);
	
}