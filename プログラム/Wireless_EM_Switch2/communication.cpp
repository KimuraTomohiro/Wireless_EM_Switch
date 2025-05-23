#include <TWELITE>
#include <NWK_SIMPLE>
#include "communication.hpp"
#include "Interrupt.hpp"

const uint32_t APP_ID = 0x20297C9;  
const uint8_t CHANNEL = 12;
bool communication_established_check_flg;
uint8_t lqi_data;

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
	1ビット目：出力
	2ビット目：SEの状況	1が出力中、0が不明またはオフ
	3ビット目：HEの状況
	4ビット目：WEの状況

	つまり、0x0Fが出力状態になる

・2,3バイト目
	V1の電圧10ビット
・4,5バイト目
	V2の電圧10ビット

・6,7,8,9バイト目
タイムスタンプ
*/


void vTransmit(uint32_t addr, uint8_t data, uint8_t retry) {

	if (auto&& pkt = the_twelite.network.use<NWK_SIMPLE>().prepare_tx_packet()) {
		// set tx packet behavior
		pkt << tx_addr(addr)  // 0..0xFF (LID 0:parent, FE:child w/ no id, FF:LID broad cast), 0x8XXXXXXX (long address)
			<< tx_retry(retry) // set retry (0x3 send four times in total)
			<< tx_packet_delay(100,200,20); // send packet w/ delay (send first packet with randomized delay from 100 to 200ms, repeat every 20ms)
		// prepare packet payload
		pack_bytes(pkt.get_payload() // set payload data objects.
			, data
		);

		// do transmit 
		pkt.transmit();
		Serial << format("制御用送信データ: %x",data) << crlf;
	}else{
		Serial << format("送信失敗") << crlf;
	}

}


void on_rx_packet(packet_rx& rx, bool_t &handled) {
	// rx >> Serial; // debugging (display longer packet information)

    communication_established_check_flg = true;

	lqi_data = rx.get_lqi();

	expand_bytes(rx.get_payload().begin(), rx.get_payload().end()
				, rx_status       // 1byte of data
    			, rx_volt1        // 2byte of data (0..1023)
        		, rx_volt2        // 2byte of data (0..1023)
                , rx_timestamp    // 4byte of timestamp data
	);

	vTransmit(0x0,0x12);	//親機からステイタスが送られてきたら、疎通確認用信号を送信する
	
}