#pragma once
#include "JNetCore.h"
#include "PerformanceCounter.h"
#include "LoginServer.h"

class LoginServerMont : public JNetClient
{
private:
	struct stMontData {
		int dataValue = 0;
		int timeStamp = 0;
	};
	std::map<BYTE, stMontData> m_MontDataMap;

private:
	HANDLE						m_MontThread;
	bool						m_MontServerConnected = false;

	LoginServer* m_LoginServer;
	PerformanceCounter* m_PerfCounter;


public:
	LoginServerMont(LoginServer* loginserver,
		const char* serverIP, uint16 serverPort,
		BYTE packetCode_LAN,
		uint32 numOfIocpConcurrentThrd, uint16 numOfIocpWorkerThrd,
		size_t tlsMemPoolUnitCnt, size_t tlsMemPoolUnitCapacity,
		uint32 memPoolBuffAllocSize,
		uint32 sessionRecvBuffSize)
		: m_LoginServer(loginserver),
		JNetClient(
			serverIP, serverPort,
			packetCode_LAN,
			numOfIocpConcurrentThrd, numOfIocpWorkerThrd,
			tlsMemPoolUnitCnt, tlsMemPoolUnitCapacity,
			memPoolBuffAllocSize,
			sessionRecvBuffSize,
			false
		) {}

	bool Start() {
		if (!JNetClient::Start(false)) {
			return false;
		}

		m_MontThread = (HANDLE)_beginthreadex(NULL, 0, PerformanceMontFunc, this, 0, NULL);
		return true;
	}

private:
	virtual void OnConnectionToServer() override;
	virtual void OnDisconnectionFromServer() override;
	virtual void OnRecv(JBuffer& recvBuff) override { /*모니터링 서버로부터 수신 받을 패킷 없음*/ }

private:
	static UINT __stdcall PerformanceMontFunc(void* arg);

	void ResetPerfCount();
	void SendPerfCountToMontServer();
};

