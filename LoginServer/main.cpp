#include "LoginServer.h"
#include <conio.h>

int main() {
	LoginServer loginserver(
		LOGIN_SERVER_NUM_OF_DB_CONN, ODBC_CONNECTION_STRING,
		LOGIN_SERVER_IP, LOGIN_SERVER_PORT, MAX_CLIENT_CONNECTION,
		MONT_SERVER_PROTOCOL_CODE, MONT_SERVER_PROTOCOL_CODE, LOGIN_SERVER_PACKET_CODE,
		false,
		MAX_CLIENT_CONNECTION,
		NUM_OF_IOCP_CONCURRENT_THREAD, NUM_OFIOCP_WORKER_THREAD,
		NUM_OF_TLSMEMPOOL_INIT_MEM_UNIT, NUM_OF_TLSMEMPOOL_CAPACITY,
		false, false,
		LOGIN_SERVER_SERIAL_BUFFER_SIZE,
		LOGIN_SERVER_RECV_BUFFER_SIZE,
		LOGIN_SERVER_CALC_TPS_THREAD
	);

	if (!loginserver.Start()) {
		cout << "loginserver.Start returns false!" << endl;
		return 0;
	}

	char ctr;
	clock_t ct = 0;
	while (true) {
		if (_kbhit()) {
			ctr = _getch();
			if (ctr == 'q' || ctr == 'Q') {
				break;
			}
		}
		loginserver.PrintServerInfoOnConsole();
		Sleep(1000);
	}

	loginserver.Stop();
}