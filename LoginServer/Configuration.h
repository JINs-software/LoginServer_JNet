#pragma once

//#define ASSERT

#define CONNECT_TO_MONITORING_SERVER

//#define	CONNECT_TIMEOUT_CHECK_SET

//#define DELEY_TIME_CHECK

#define RECV_BUFFERING_MODE					false
#define	DB_CONNECTION_ERROR_FILE_LOGGING	true

/*********************************************
* 로그인 서버 설정 값(생성자 인수)
**********************************************/
/// @def 로그인 서버 DB Connection 갯수
#define LOGIN_SERVER_NUM_OF_DB_CONN			100	
/// @def ODBC Connection String
#define ODBC_CONNECTION_STRING				L"Driver={MySQL ODBC 9.1 ANSI Driver};Server=127.0.0.1;User=root;Password=607281;Option=3;"
											// L"Driver={ODBC Driver 18 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=TestDB;Trusted_Connection=Yes;"
/// @def 로그인 서버 바인딩 IP
#define LOGIN_SERVER_IP						NULL
/// @def 로그인 서버 바인딩 포트 번호
#define LOGIN_SERVER_PORT					12110
/// @def 연결 최대 수용량
#define MAX_CLIENT_CONNECTION				18000
/// @def 로그인 서버 프로토콜 코드 (로그인 프로토콜 헤더 코드 부)
#define LOGIN_SERVER_PROTOCOL_CODE			119
/// @def 로그인 서버 패킷 코드 (패킷 En/Decode 시 사용되는 서버-클라 대칭키)
#define LOGIN_SERVER_PACKET_CODE			50
/// @def 로그인 서버 IOCP 'Concurrent thread' 인수
#define NUM_OF_IOCP_CONCURRENT_THREAD		2		
/// @def 로그인 서버 IOCP 작업자 스레드 갯수
#define NUM_OFIOCP_WORKER_THREAD			100
/// @def TLS 메모리 풀(객체: 송신 직렬화 버퍼) 내 초기 객체 할당 갯수
#define NUM_OF_TLSMEMPOOL_INIT_MEM_UNIT		1000
/// @def TLS 메모리 풀( "" ) 할당 가능 객체 최대 갯수(제한)
#define NUM_OF_TLSMEMPOOL_CAPACITY			1000
/// @def 로그인 서버 송신 직렬화 버퍼 크기
#define LOGIN_SERVER_SERIAL_BUFFER_SIZE		300
/// @def 로그인 세션 수신 버퍼 크기
#define	LOGIN_SERVER_RECV_BUFFER_SIZE		1000
/// @def TPS(Transaction Per Second) 측정 플래그
#define LOGIN_SERVER_CALC_TPS_THREAD		false


/*********************************************
* 로그인 서버 내부 설정 값
**********************************************/
#define CONNECT_LOGIN_REQ_TIMEOUT_SEC		3
/// @def Redis 토큰 서버 IP
#define REDIS_TOKEN_SERVER_IP				"127.0.0.1"
/// @def Reds 토큰 서버 포트 번호
#define REDIS_TOKEN_SERVER_PORT				6379
#define ECHO_GAME_SERVER_IP_WSTR			L"127.0.0.1"
#define ECHO_GAME_SERVER_POPT				12120
#define CHATTING_SERVER_IP_WSTR				L"127.0.0.1"
#define CHATTING_SERVER_POPT				12130


/*******************************************************************************
* 로그인 모니터링 클라이언트(모니터링 서버 기준 클라이언트) 설정 값(생성자 인수)
********************************************************************************/
/// @def 모니터링 서버 IP
#define MONT_SERVER_IP						"127.0.0.1"
/// @def 모니터링 서버 포트 버호
#define	MONT_SERVER_PORT					12121
/// @def 모니터링 프로토콜 코드
#define MONT_SERVER_PROTOCOL_CODE			109
/// @def 모니터링 프로토콜 패킷 En/Decode 대칭 키
//#define MONT_SERVER_PACKET_KEY				30		// LAN 구간 내 프로토콜 -> En/Decode X
/// @def IOCP 'Concurrent Thread' 인수
#define MONT_CLIENT_IOCP_CONCURRENT_THRD		0
/// @def IOCP 작업자 스레드 갯수
#define MONT_CLIENT_IOCP_WORKER_THRD_CNT		1
/// @def 송신 직렬화 버퍼 초기 할당 갯수 (TLS 메모리 풀)
#define MONT_CLIENT_MEM_POOL_UNIT_CNT			0
/// @def 송신 직렬화 버퍼 최대 할당 가능 객체 갯수
#define MONT_CLIENT_MEM_POOL_UNIT_CAPACITY		10
/// @def 송신 직렬화 버퍼 기본 크기
#define MONT_CLIENT_MEM_POOL_BUFF_ALLOC_SIZE	200
/// @def 모니터링 클라이언트 수신 버퍼 크기
#define MONT_CLIENT_RECV_BUFF_SIZE				100