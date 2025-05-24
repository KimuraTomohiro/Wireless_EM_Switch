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
	0ビット目：出力
	1ビット目：SEの状況	1が出力中、0が不明またはオフ
	2ビット目：HEの状況
	3ビット目：WEの状況
	4ビット目：EM解除中かどうか(子機から解除信号を受け取ったら、親機は５秒間解除を告知する信号を出す。子機はこの解除を告知する信号を受け取るまで、緊急停止信号を出し続ける)
	5ビット目：1
	6ビット目：1
	7ビット目：1

	つまり、0xEFが出力状態になる

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
		Serial << format("Send Data: 0x%x",data) << crlf;
	}else{
		Serial << format("Send Faild") << crlf;
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