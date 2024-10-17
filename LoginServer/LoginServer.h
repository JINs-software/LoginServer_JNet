#pragma once
#include "JNetCore.h"
#include "PerformanceCounter.h"
#include "Configuration.h"
#include "Protocol.h"

using namespace jnet;
class LoginServerMont;
namespace RedisCpp {
	class CRedisConn;
}

class LoginServer : public JNetOdbcServer	
{
private:
	bool							m_ServerStart;			// Stop ȣ�� �� �÷��� on, Stop ȣ�� ���� ���� ��ü �Ҹ��� ȣ�� �� Stop �Լ� ȣ��(���� �۾�)
	uint16							m_NumOfIOCPWorkers;		// IOCP �۾��� ������ �� Redis Ŀ�ؼ��� �α� ���� �����ڿ��� �ش� ���� �ʱ�ȭ

	// Ŭ���̾�Ʈ ����ID <->  Ŭ���̾�Ʈ ȣ��Ʈ �ּ� ����
	std::map<UINT64, SOCKADDR_IN>	m_ClientHostAddrMap;
	std::mutex						m_ClientHostAddrMapMtx;

	const char m_Client_CLASS1[16] = "10.0.1.2";
	const char m_Client_CLASS2[16] = "10.0.2.2";

	const WCHAR* Query_AccountNo = L"SELECT accountno FROM accountdb.sessionkey WHERE accountno = ? AND sessionkey IS NULL";
	const WCHAR* Query_AccountInfo = L"SELECT a.userid, a.usernick, s.status FROM accountdb.account a JOIN accountdb.status s ON a.accountno = s.accountno WHERE a.accountno = ?";

	/*********************************
	* DB
	*********************************/
	//DBConnection*				m_DBConn;
	// => �۾��� �����尡 �ʿ� �ø��� Ŀ�ؼ� Ǯ�κ��� �Ҵ� �޵��� �Ѵ�. (��Ƽ �۾��� ������)

	/*********************************
	* Redis
	*********************************/
	//RedisCpp::CRedisConn*		m_RedisConn;	// 'm_RedisConn' ������, �ҿ����� ������ ����� �� �����ϴ�.
	// => ������ Ŀ�ǵ� ���ڷ� ���޵Ǵ� redisContext�� thread-safe���� �ʴ�. 
	// ���� connect�� ���� �Ҵ�޴� ���ؽ�Ʈ�� ���� �� �����Ͽ� ���� ���� Ǯ�� ����� ���?
	LockFreeQueue<RedisCpp::CRedisConn*>	m_RedisConnPool;

	/*********************************
	* Monitoring
	*********************************/
	LoginServerMont*	m_ServerMont;		// LoginServer::Start �Լ����� ���� �� Start ȣ��
public:
	std::atomic<long>	m_AuthTransaction;

private:
	uint64							m_TotalLoginCnt;		// �α��� ��û �޽��� ó�� -> ���� -> �α��� ���� ó�� ���� �޽��� �۽� �� ī���� ����
	uint64							m_TotalLoginFailCnt;	// �α��� ��û �޽��� ó�� -> ���� ����(DB ��ȸ/���� ȹ��/Redis ��ū ���� ����) 
															// -> �α��� ���� ���� ���� �޽��� �۽� �� ī���� ����

public:
	LoginServer(
		int32 dbConnCnt, const WCHAR* odbcConnStr,
		const char* serverIP, uint16 serverPort, uint16 maximumOfConnections,
		PACKET_CODE packetCode_LAN, PACKET_CODE packetCode, PACKET_SYMM_KEY packetSymmetricKey,
		bool recvBufferingMode,
		uint16 maximumOfSessions,
		uint32 numOfIocpConcurrentThrd, uint16 numOfIocpWorkerThrd,
		size_t tlsMemPoolUnitCnt, size_t tlsMemPoolUnitCapacity,
		uint32 memPoolBuffAllocSize,
		uint32 sessionRecvBuffSize,
		bool calcTpsThread
	);

	bool Start();
	void Stop();
	bool ServerStop() { return !m_ServerStart; }

private:
	/// @brief �α��� ���� ���� Ŭ���̾�Ʈ ����
	// - call LoginServerMont::IncrementSessionCount()
	virtual void OnClientJoin(UINT64 sessionID, const SOCKADDR_IN& clientSockAddr) override;
	/// @brief �α��� ���� ���� Ŭ���̾�Ʈ ���� ����
	// - call LoginServerMont::DecrementSessionCount() 
	virtual void OnClientLeave(UINT64 sessionID) override;
	/// @brief �α��� ��û ��Ŷ ���� ó�� -> Proc_LOGIN_REQ(..) 
	/// @todo �α��� ���� ����(OnClientJoint) ~ �α��� ��û ��Ŷ ����(OnRecv)���� �ð� ���� �� �ʿ� �� Ÿ�� �ƿ� �ߵ� ���
	virtual void OnRecv(UINT64 sessionID, JBuffer& recvBuff);

	/// @brief �α��� ��û �޽��� ó��, (1) DB ��ȸ �� (2) ��ū ���� �׸��� (3) ��ū ���� �۾��� ���� ������� ���� (�ټ��� IOCP �۾��� ������)
	void Proc_LOGIN_REQ(UINT64, stMSG_LOGIN_REQ);
	/// @brief �α��� ��û�� ���� ����
	void Send_LOGIN_RES(UINT64, INT64 accountNo, BYTE status, const WCHAR* id, const WCHAR* nickName, const WCHAR* gameserverIP, USHORT gameserverPort, const WCHAR* chatserverIP, USHORT chatserverPort);

private:
	// DB ����
	bool CheckSessionKey(INT64 accountNo/*, const char* sessionKey*/);
	bool GetAccountInfo(INT64 accountNo, WCHAR* ID, WCHAR* Nickname);

	// Redis ����
	bool InsertSessionKeyToRedis(INT64 accountNo, const char* sessionKey);

#if defined(CONNECT_TIMEOUT_CHECK_SET)
	// Ÿ�Ӿƿ� üĿ ������ �Լ�
	static UINT __stdcall TimeOutCheckThreadFunc(void* arg);
#endif
};

