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

class LoginServer : public JNetOdbcServer	// -> JNetOdbcServer
{
private:
	bool							m_ServerStart;			// Stop 호출 시 플래그 on, Stop 호출 없이 서버 객체 소멸자 호출 시 Stop 함수 호출(정리 작업)
	uint16							m_NumOfIOCPWorkers;		// IOCP 작업자 스레드 별 Redis 커넥션을 맺기 위해 생성자에서 해당 변수 초기화

	// 클라이언트 세션ID <->  클라이언트 호스트 주소 맵핑
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
	// => 작업자 스레드가 필요 시마다 커넥션 풀로부터 할당 받도록 한다. (멀티 작업자 스레드)

	/*********************************
	* Redis
	*********************************/
	//RedisCpp::CRedisConn*		m_RedisConn;	// 'm_RedisConn' 빨간줄, 불완전한 형식은 사용할 수 없습니다.
	// => 레디스의 커맨드 인자로 전달되는 redisContext는 thread-safe하지 않다. 
	// 따라서 connect로 부터 할당받는 컨텍스트를 여러 개 생성하여 레디스 연결 풀을 만들면 어떨까?
	LockFreeQueue<RedisCpp::CRedisConn*>	m_RedisConnPool;

	/*********************************
	* Monitoring
	*********************************/
	LoginServerMont*	m_ServerMont;		// LoginServer::Start 함수에서 생성 및 Start 호출
public:
	std::atomic<long>	m_AuthTransaction;

private:
	uint64							m_TotalLoginCnt;		// 로그인 요청 메시지 처리 -> 인증 -> 로그인 정상 처리 응답 메시지 송신 후 카운터 증가
	uint64							m_TotalLoginFailCnt;	// 로그인 요청 메시지 처리 -> 인증 실패(DB 조회/계정 획득/Redis 토큰 삽입 실패) 
															// -> 로그인 인증 실패 응답 메시지 송신 후 카운터 증가

public:
	LoginServer(
		int32 dbConnCnt, const WCHAR* odbcConnStr,
		const char* serverIP, uint16 serverPort, uint16 maximumOfConnections,
		PACKET_CODE packetCode_LAN, PACKET_CODE packetCode, PACKET_SYMM_KEY packetSymmetricKey,
		bool recvBufferingMode,
		uint16 maximumOfSessions,
		uint32 numOfIocpConcurrentThrd, uint16 numOfIocpWorkerThrd,
		size_t tlsMemPoolUnitCnt, size_t tlsMemPoolUnitCapacity,
		bool tlsMemPoolMultiReferenceMode, bool tlsMemPoolPlacementNewMode,
		uint32 memPoolBuffAllocSize,
		uint32 sessionRecvBuffSize,
		bool calcTpsThread
	) : JNetOdbcServer(
		dbConnCnt, odbcConnStr,
		serverIP, serverPort, maximumOfConnections,
		packetCode_LAN, packetCode, packetSymmetricKey,
		recvBufferingMode,
		maximumOfSessions,
		numOfIocpConcurrentThrd, numOfIocpWorkerThrd,
		tlsMemPoolUnitCnt, tlsMemPoolUnitCapacity,
		tlsMemPoolMultiReferenceMode, tlsMemPoolPlacementNewMode,
		memPoolBuffAllocSize,
		sessionRecvBuffSize,
		calcTpsThread
	)
	{}

	bool Start();
	void Stop();
	bool ServerStop() { return !m_ServerStart; }

protected:
	// 로그인 서버 접속 클라이언트 연결
	// - LoginServerMont::IncrementSessionCount() 호출
	virtual void OnClientJoin(UINT64 sessionID, const SOCKADDR_IN& clientSockAddr) override;

	// 로그인 서버 접속 클라이언트 연결 종료
	// - DecrementSessionCount
	virtual void OnClientLeave(UINT64 sessionID) override;

	// 로그인 서버 연결 -> 로그인 요청 패킷 전송 (OnClientJoin부터 최초 OnRecv까지의 타임 아웃 발동 필요)
	virtual void OnRecv(UINT64 sessionID, JBuffer& recvBuff);

	// 메시지 처리
	void Proc_LOGIN_REQ(UINT64, stMSG_LOGIN_REQ);
	void Proc_LOGIN_RES(UINT64, INT64 accountNo, BYTE status, const WCHAR* id, const WCHAR* nickName, const WCHAR* gameserverIP, USHORT gameserverPort, const WCHAR* chatserverIP, USHORT chatserverPort);

private:
	// DB 접근
	bool CheckSessionKey(INT64 accountNo, const char* sessionKey);
	bool GetAccountInfo(INT64 accountNo, WCHAR* ID, WCHAR* Nickname);

	// Redis 접근
	bool InsertSessionKeyToRedis(INT64 accountNo, const char* sessionKey);

#if defined(CONNECT_TIMEOUT_CHECK_SET)
	// 타임아웃 체커 스레드 함수
	static UINT __stdcall TimeOutCheckThreadFunc(void* arg);
#endif
};

