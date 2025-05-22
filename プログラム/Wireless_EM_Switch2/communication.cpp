#include <TWELITE>
#include <NWK_SIMPLE>
#include "communication.hpp"
#include "Interrupt.hpp"

const uint32_t APP_ID = 0x20297C9;  
const uint8_t CHANNEL = 12;
bool communication_established_check_flg;

/*
送受信データの詳細(子機側)
・1バイト目
以下のコマンドを使用

親機宛
	0x10: 緊急停止指示
	0x11: 出力再開指示
	0x12: 疎通確認用信号(何もしない)

・2,3,4,5バイト目
タイムスタンプ
*/

/*
送受信データの詳細(親機側)
・1バイト目
以下のコマンドを使用

ブロードキャスト
	0x00: 緊急停止動作中
	0x01: 出力中・確認出力待機中

・2,3バイト目
	V1の電圧10ビット
・4,5バイト目
	V2の電圧10ビット

・6,7,8,9バイト目
タイムスタンプ
*/




const char* Serial_text[] = {
    "Stop",
    "Check",
    "Start"
};

void vTransmit(uint32_t addr, uint8_t data, uint8_t retry) {

	if (auto&& pkt = the_twelite.network.use<NWK_SIMPLE>().prepare_tx_packet()) {
		// set tx packet behavior
		pkt << tx_addr(addr)  // 0..0xFF (LID 0:parent, FE:child w/ no id, FF:LID broad cast), 0x8XXXXXXX (long address)
			<< tx_retry(retry) // set retry (0x3 send four times in total)
			<< tx_packet_delay(0,10,5); // send packet w/ delay (send first packet with randomized delay from 100 to 200ms, repeat every 20ms)
		// prepare packet payload
		pack_bytes(pkt.get_payload() // set payload data objects.
			, data
            , uint32_t(millis())
		);
	Serial << format("制御用送信データ: %x %s",data,Serial_text[data]) << crlf;
		// do transmit 
		pkt.transmit();
	}else{
		Serial << format("Send failed") << crlf;
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