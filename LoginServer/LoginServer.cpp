#include "LoginServer.h"
#include "LoginServerMont.h"
#include "CRedisConn.h"

bool LoginServer::Start()
{
	// Connect to Redis
	bool firstConn = true;
	for (uint16 i = 0; i < m_NumOfIOCPWorkers; i++) {
		RedisCpp::CRedisConn* redisConn = new RedisCpp::CRedisConn();	// 형식 지정자가 필요합니다.
		if (redisConn == NULL) {
			cout << "[LoginServer::Start] new RedisCpp::CRedisConn() return NULL" << endl;
			return false;
		}

		if (!redisConn->connect(REDIS_TOKEN_SERVER_IP, REDIS_TOKEN_SERVER_PORT)) {
			cout << "[ChattingServer::Start] new RedisCpp::CRedisConn(); return NULL" << endl;
			return false;
		}

		if (!redisConn->ping()) {
			cout << "[ChattingServer::Start] m_RedisConn->connect(..) return FALSE" << endl;
			return false;
		}

		m_RedisConnPool.Enqueue(redisConn);
	}

	m_ServerMont = new LoginServerMont(this,
		MONT_SERVER_IP, MONT_SERVER_PORT,
		MONT_SERVER_PROTOCOL_CODE,
		MONT_CLIENT_IOCP_CONCURRENT_THRD, MONT_CLIENT_IOCP_WORKER_THRD_CNT,
		MONT_CLIENT_MEM_POOL_UNIT_CNT, MONT_CLIENT_MEM_POOL_UNIT_CAPACITY,
		false, false,
		MONT_CLIENT_MEM_POOL_BUFF_ALLOC_SIZE,
		MONT_CLIENT_RECV_BUFF_SIZE
	);
    
	if (!JNetServer::Start()) {
		return false;
	}

	m_ServerMont->Start();

	m_ServerStart = true;
	return true;
}

void LoginServer::Stop()
{
	if (m_ServerStart) {
		m_ServerStart = false;
		JNetServer::Stop();

		while (m_RedisConnPool.GetSize() > 0) {
			RedisCpp::CRedisConn* redisConn = NULL;
			m_RedisConnPool.Dequeue(redisConn);
			if (redisConn != NULL) {
				delete redisConn;
			}
		}
	}
}

void LoginServer::OnClientJoin(UINT64 sessionID, const SOCKADDR_IN& clientSockAddr)
{
	m_ClientHostAddrMapMtx.lock();
	if (m_ClientHostAddrMap.find(sessionID) != m_ClientHostAddrMap.end()) {
#if defined(ASSERT)
		DebugBreak();
#endif
	}
	else {
		m_ClientHostAddrMap.insert({ sessionID, clientSockAddr });
	}
	m_ClientHostAddrMapMtx.unlock();
}

void LoginServer::OnClientLeave(UINT64 sessionID)
{
	m_ClientHostAddrMapMtx.lock();
	if (m_ClientHostAddrMap.find(sessionID) == m_ClientHostAddrMap.end()) {
#if defined(ASSERT)
		// 로그인 처리 후 클라이언트에서 먼저 끊는 방식이라면, OnClientLeave에서 삭제 처리
		DebugBreak();
#endif
	}
	else {
		m_ClientHostAddrMap.erase(sessionID);
	}
	m_ClientHostAddrMapMtx.unlock();
}

void LoginServer::OnRecv(UINT64 sessionID, JBuffer& recvBuff)
{
	while (recvBuff.GetUseSize() >= sizeof(WORD)) {
		WORD type;
		recvBuff.Peek(&type);

		if (type == en_PACKET_CS_LOGIN_REQ_LOGIN) {
			// 로그인 요청 처리
			stMSG_LOGIN_REQ message;
			recvBuff >> message;
			Proc_LOGIN_REQ(sessionID, message);		// 1. 계정 DB 접근 및 계정 정보 확인
		}
	}
}

void LoginServer::Proc_LOGIN_REQ(UINT64 sessionID, stMSG_LOGIN_REQ message)
{
	stMSG_LOGIN_RES resMessage;
	memset(&resMessage, 0, sizeof(resMessage));
	resMessage.AccountNo = message.AccountNo;
	resMessage.Status = dfLOGIN_STATUS_OK;

	// DB 조회
	if (!CheckSessionKey(message.AccountNo, message.SessionKey)) {
		resMessage.Status = dfLOGIN_STATUS_FAIL;
		InterlockedIncrement64((int64*)&m_TotalLoginFailCnt);
#if defined(ASSERT)
		DebugBreak();
#endif
	}
	else {
		// Account 정보 획득(stMSG_LOGIN_RES 메시지 활용)
		if (!GetAccountInfo(message.AccountNo, resMessage.ID, resMessage.Nickname/*, resMessage.GameServerIP, resMessage.GameServerPort, resMessage.ChatServerIP, resMessage.ChatServerPort*/)) {
			resMessage.Status = dfLOGIN_STATUS_FAIL;
			InterlockedIncrement64((int64*)&m_TotalLoginFailCnt);
#if defined(ASSERT)
			DebugBreak();
#endif
		}
		else {
			// 4. Redis에 토큰 삽입
			if (!InsertSessionKeyToRedis(message.AccountNo, message.SessionKey)) {
				resMessage.Status = dfLOGIN_STATUS_FAIL;
				InterlockedIncrement64((int64*)&m_TotalLoginFailCnt);
#if defined(ASSERT)
				DebugBreak();
#endif
			}
		}
	}

	// 5. 서버 IP/Port 설정
	m_ClientHostAddrMapMtx.lock();
	auto iter = m_ClientHostAddrMap.find(sessionID);
	if (iter == m_ClientHostAddrMap.end()) {
#if defined(ASSERT)
		DebugBreak();
#endif
	}
	SOCKADDR_IN clientAddr = iter->second;
	m_ClientHostAddrMapMtx.unlock();

	char clientIP[16] = { 0, };
	IN_ADDR_TO_STRING(clientAddr.sin_addr, clientIP);

	if (memcmp(clientIP, m_Client_CLASS1, 16) == 0) {
		memcpy(resMessage.GameServerIP, L"10.0.1.1", sizeof(resMessage.GameServerIP));
		resMessage.GameServerPort = ECHO_GAME_SERVER_POPT;
		memcpy(resMessage.ChatServerIP, L"10.0.1.1", sizeof(resMessage.ChatServerIP));
		resMessage.ChatServerPort = CHATTING_SERVER_POPT;
	}
	else if (memcmp(clientIP, m_Client_CLASS2, 16) == 0) {
		memcpy(resMessage.GameServerIP, L"10.0.2.1", sizeof(resMessage.GameServerIP));
		resMessage.GameServerPort = ECHO_GAME_SERVER_POPT;
		memcpy(resMessage.ChatServerIP, L"10.0.2.1", sizeof(resMessage.ChatServerIP));
		resMessage.ChatServerPort = CHATTING_SERVER_POPT;
	}
	else {
		memcpy(resMessage.GameServerIP, L"127.0.0.1", sizeof(resMessage.GameServerIP));
		resMessage.GameServerPort = ECHO_GAME_SERVER_POPT;
		memcpy(resMessage.ChatServerIP, L"127.0.0.1", sizeof(resMessage.ChatServerIP));
		resMessage.ChatServerPort = CHATTING_SERVER_POPT;
	}


	// 6. 클라이언트에 토큰 전송 (WSASend)
	Proc_LOGIN_RES(sessionID, resMessage.AccountNo, 
		resMessage.Status, resMessage.ID, resMessage.Nickname, 
		resMessage.GameServerIP, resMessage.GameServerPort, 
		resMessage.ChatServerIP, resMessage.ChatServerPort
	);
}

void LoginServer::Proc_LOGIN_RES(UINT64 sessionID, INT64 accountNo, BYTE status, const WCHAR* id, const WCHAR* nickName, const WCHAR* gameserverIP, USHORT gameserverPort, const WCHAR* chatserverIP, USHORT chatserverPort)
{
	JBuffer* reply = AllocSerialSendBuff(sizeof(WORD) + sizeof(INT64) + sizeof(BYTE) + sizeof(WCHAR[20]) + sizeof(WCHAR[20]) + sizeof(WCHAR[16]) + sizeof(USHORT) + sizeof(WCHAR[16]) + sizeof(USHORT));
	(*reply) << (WORD)en_PACKET_CS_LOGIN_RES_LOGIN << accountNo << status;
	reply->Enqueue((BYTE*)id, sizeof(WCHAR[20]));
	reply->Enqueue((BYTE*)nickName, sizeof(WCHAR[20]));
	reply->Enqueue((BYTE*)gameserverIP, sizeof(WCHAR[16]));
	(*reply) << gameserverPort;
	reply->Enqueue((BYTE*)chatserverIP, sizeof(WCHAR[16]));
	(*reply) << chatserverPort;

	SendPacketBlocking(sessionID, reply);

	++m_AuthTransaction;
}

bool LoginServer::CheckSessionKey(INT64 accountNo, const char* sessionKey)
{
	/***************************************
	* DB 커넥션 타임아웃에 대비한 코드로 변경
	* *************************************/
	bool ret;
	SQLLEN sqlLen = 0;

	JNetDBConn* dbConn;
	bool dbProcSuccess = false;
	while (!dbProcSuccess) {
		// 1. DB 커넥션 할당
		while ((dbConn = HoldDBConnection()) == NULL);	// DBConnection 획득까지 polling

		// 2. 이전 바인딩 해제
		UnBind(dbConn);

		// 3. 첫 번째 파라미터로 계정 번호 바인딩
		if (BindParameter(dbConn, 1, &accountNo)) {
			// 4. 쿼리 실행
			if (!ExecQuery(dbConn, Query_AccountNo)) {
				FreeDBConnection(dbConn, true, true);
				continue;
			}
			else {
				if (GetRowCount(dbConn) > 0) { ret = true; }
				else { ret = false; }
				dbProcSuccess = true;
			}
		}

		FreeDBConnection(dbConn);
	}

	return ret;
}

bool LoginServer::GetAccountInfo(INT64 accountNo, WCHAR* ID, WCHAR* Nickname)
{
	bool ret;

	// 계정 정보와 상태를 가져오는 SQL 쿼리
	JNetDBConn* dbConn;
	bool dbProcSuccess = false;
	while (!dbProcSuccess) {
		while ((dbConn = HoldDBConnection()) == NULL);	// DBConnection 획득까지 polling

		// 이전 바인딩 해제
		UnBind(dbConn);

		// 계정 번호를 파라미터로 바인딩
		if (BindParameter(dbConn, 1, &accountNo)) {
			// 쿼리(계정 정보와 상태를 가져오는 SQL 쿼리)
			if (!ExecQuery(dbConn, Query_AccountInfo)) {
				FreeDBConnection(dbConn, true, true);
				continue;
			}
			else {
				// 결과를 저장할 변수들 선언
				WCHAR userid[20];  // 사용자 ID
				WCHAR usernick[20];  // 사용자 닉네임
				int status;  // 상태

				// 결과 열을 바인딩
				BindColumn(dbConn, 1, userid, sizeof(userid), NULL);
				BindColumn(dbConn, 2, usernick, sizeof(usernick), NULL);
				BindColumn(dbConn, 3, &status);

				// 결과를 페치하고 응답 구조체에 값 설정
				if (!dbConn->Fetch()) { ret = false; }
				else {
					ret = true;
					memcpy(ID, userid, sizeof(userid));
					memcpy(Nickname, usernick, sizeof(usernick));
				} 
				dbProcSuccess = true;
			}
		}

		FreeDBConnection(dbConn);
	}

	return ret;
}

bool LoginServer::InsertSessionKeyToRedis(INT64 accountNo, const char* sessionKey)
{
	bool ret;
	std::string accountNoStr = to_string(accountNo);
	std::string sessionKeyStr(sessionKey, sizeof(stMSG_LOGIN_REQ::SessionKey));

	RedisCpp::CRedisConn* redisConn = NULL;
	while (true) {	// redisConnect 획득까지 폴링
		m_RedisConnPool.Dequeue(redisConn);
		if (redisConn != NULL) {
			break;
		}
	}

	uint32 retval;
	if (!redisConn->set(accountNoStr, sessionKeyStr, retval)) { ret = false; }
	else { ret = true; }

	m_RedisConnPool.Enqueue(redisConn);
	return ret;
}

#if defined(CONNECT_TIMEOUT_CHECK_SET)
UINT __stdcall LoginServer::TimeOutCheckThreadFunc(void* arg)
{
	return 0;
}
#endif
