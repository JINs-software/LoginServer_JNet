#pragma once

//#define ASSERT

#define CONNECT_TO_MONITORING_SERVER

//#define	CONNECT_TIMEOUT_CHECK_SET

//#define DELEY_TIME_CHECK

#define RECV_BUFFERING_MODE					false
#define	DB_CONNECTION_ERROR_FILE_LOGGING	true

/*********************************************
* �α��� ���� ���� ��(������ �μ�)
**********************************************/
/// @def �α��� ���� DB Connection ����
#define LOGIN_SERVER_NUM_OF_DB_CONN			100	
/// @def ODBC Connection String
#define ODBC_CONNECTION_STRING				L"Driver={MySQL ODBC 9.1 ANSI Driver};Server=127.0.0.1;User=root;Password=607281;Option=3;"
											// L"Driver={ODBC Driver 18 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=TestDB;Trusted_Connection=Yes;"
/// @def �α��� ���� ���ε� IP
#define LOGIN_SERVER_IP						NULL
/// @def �α��� ���� ���ε� ��Ʈ ��ȣ
#define LOGIN_SERVER_PORT					12110
/// @def ���� �ִ� ���뷮
#define MAX_CLIENT_CONNECTION				18000
/// @def �α��� ���� �������� �ڵ� (�α��� �������� ��� �ڵ� ��)
#define LOGIN_SERVER_PROTOCOL_CODE			119
/// @def �α��� ���� ��Ŷ �ڵ� (��Ŷ En/Decode �� ���Ǵ� ����-Ŭ�� ��ĪŰ)
#define LOGIN_SERVER_PACKET_CODE			50
/// @def �α��� ���� IOCP 'Concurrent thread' �μ�
#define NUM_OF_IOCP_CONCURRENT_THREAD		2		
/// @def �α��� ���� IOCP �۾��� ������ ����
#define NUM_OFIOCP_WORKER_THREAD			100
/// @def TLS �޸� Ǯ(��ü: �۽� ����ȭ ����) �� �ʱ� ��ü �Ҵ� ����
#define NUM_OF_TLSMEMPOOL_INIT_MEM_UNIT		1000
/// @def TLS �޸� Ǯ( "" ) �Ҵ� ���� ��ü �ִ� ����(����)
#define NUM_OF_TLSMEMPOOL_CAPACITY			1000
/// @def �α��� ���� �۽� ����ȭ ���� ũ��
#define LOGIN_SERVER_SERIAL_BUFFER_SIZE		300
/// @def �α��� ���� ���� ���� ũ��
#define	LOGIN_SERVER_RECV_BUFFER_SIZE		1000
/// @def TPS(Transaction Per Second) ���� �÷���
#define LOGIN_SERVER_CALC_TPS_THREAD		false


/*********************************************
* �α��� ���� ���� ���� ��
**********************************************/
#define CONNECT_LOGIN_REQ_TIMEOUT_SEC		3
/// @def Redis ��ū ���� IP
#define REDIS_TOKEN_SERVER_IP				"127.0.0.1"
/// @def Reds ��ū ���� ��Ʈ ��ȣ
#define REDIS_TOKEN_SERVER_PORT				6379
#define ECHO_GAME_SERVER_IP_WSTR			L"127.0.0.1"
#define ECHO_GAME_SERVER_POPT				12120
#define CHATTING_SERVER_IP_WSTR				L"127.0.0.1"
#define CHATTING_SERVER_POPT				12130


/*******************************************************************************
* �α��� ����͸� Ŭ���̾�Ʈ(����͸� ���� ���� Ŭ���̾�Ʈ) ���� ��(������ �μ�)
********************************************************************************/
/// @def ����͸� ���� IP
#define MONT_SERVER_IP						"127.0.0.1"
/// @def ����͸� ���� ��Ʈ ��ȣ
#define	MONT_SERVER_PORT					12121
/// @def ����͸� �������� �ڵ�
#define MONT_SERVER_PROTOCOL_CODE			109
/// @def ����͸� �������� ��Ŷ En/Decode ��Ī Ű
//#define MONT_SERVER_PACKET_KEY				30		// LAN ���� �� �������� -> En/Decode X
/// @def IOCP 'Concurrent Thread' �μ�
#define MONT_CLIENT_IOCP_CONCURRENT_THRD		0
/// @def IOCP �۾��� ������ ����
#define MONT_CLIENT_IOCP_WORKER_THRD_CNT		1
/// @def �۽� ����ȭ ���� �ʱ� �Ҵ� ���� (TLS �޸� Ǯ)
#define MONT_CLIENT_MEM_POOL_UNIT_CNT			0
/// @def �۽� ����ȭ ���� �ִ� �Ҵ� ���� ��ü ����
#define MONT_CLIENT_MEM_POOL_UNIT_CAPACITY		10
/// @def �۽� ����ȭ ���� �⺻ ũ��
#define MONT_CLIENT_MEM_POOL_BUFF_ALLOC_SIZE	200
/// @def ����͸� Ŭ���̾�Ʈ ���� ���� ũ��
#define MONT_CLIENT_RECV_BUFF_SIZE				100