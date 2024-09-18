#include "LoginServerMont.h"

void LoginServerMont::OnConnectionToServer()
{
	JBuffer* loginPacket = AllocSerialSendBuff(sizeof(WORD) + sizeof(int));
	(*loginPacket) << (WORD)en_PACKET_SS_MONITOR_LOGIN << (int)dfSERVER_LOGIN_SERVER;
	if (!SendPacket(loginPacket)) FreeSerialBuff(loginPacket);
	else m_MontServerConnected = true;
}

void LoginServerMont::OnDisconnectionFromServer()
{
	m_MontServerConnected = false;
}

UINT __stdcall LoginServerMont::PerformanceMontFunc(void* arg)
{
	LoginServerMont* mont = reinterpret_cast<LoginServerMont*>(arg);
	mont->AllocTlsMemPool();

	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_SERVER_RUN , {0} });
	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_SERVER_CPU , {0} });
	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM , {0} });
	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_SESSION , {0} });
	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_AUTH_TPS , {0} });
	mont->m_MontDataMap.insert({ dfMONITOR_DATA_TYPE_LOGIN_PACKET_POOL , {0} });

	mont->m_PerfCounter = new PerformanceCounter();
	mont->m_PerfCounter->SetCpuUsageCounter();
	mont->m_PerfCounter->SetProcessCounter(dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM, dfQUERY_PROCESS_USER_VMEMORY_USAGE, L"ChattingServer");

	while (!mont->m_LoginServer->ServerStop()) {
		if (!mont->m_MontServerConnected) {
			mont->ResetPerfCount();
			mont->SendPerfCountToMontServer();
		}
		else {
			if (mont->ConnectToServer()) mont->m_MontServerConnected = true;
		}
		Sleep(1000);
	}

	return 0;
}

void LoginServerMont::ResetPerfCount()
{
	int authTps = m_LoginServer->m_AuthTransaction;
	m_LoginServer->m_AuthTransaction = 0;

	time_t now = time(NULL);

	m_PerfCounter->ResetPerfCounterItems();
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_RUN].dataValue = 1;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_RUN].timeStamp = now;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_CPU].dataValue = m_PerfCounter->ProcessTotal();
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_CPU].timeStamp = now;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM].dataValue = m_PerfCounter->GetPerfCounterItem(dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM);
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM].dataValue /= (1024 * 1024);	// 서버 메모리 사용량 MB
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM].timeStamp = now;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SESSION].dataValue = m_LoginServer->GetCurrentSessions();		// GetSessionCount
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_SESSION].timeStamp = now;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_AUTH_TPS].dataValue = authTps;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_AUTH_TPS].timeStamp = now;
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_PACKET_POOL].dataValue = m_LoginServer->GetCurrentAllocatedMemUnitCnt();
	m_MontDataMap[dfMONITOR_DATA_TYPE_LOGIN_PACKET_POOL].timeStamp = now;
}

void LoginServerMont::SendPerfCountToMontServer()
{
	for (const auto& it : m_MontDataMap) {
		BYTE counterType = it.first;
		const stMontData& montData = it.second;
		JBuffer* perfMsg = AllocSerialSendBuff(sizeof(stMSG_MONITOR_DATA_UPDATE));

		(*perfMsg) << (WORD)en_PACKET_SS_MONITOR_DATA_UPDATE;
		(*perfMsg) << counterType << montData.dataValue << montData.timeStamp;
		if (!SendPacket(perfMsg)) {
			FreeSerialBuff(perfMsg);
		}
	}
}
