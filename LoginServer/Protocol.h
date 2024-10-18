#pragma once
#include <minwindef.h>
#include "Configuration.h"

#if defined(MOW_LOGIN_SERVER_MODE)
#define MAX_OF_ACCOUNT_ID_LENGTH		20
#define MAX_OF_ACCOUNT_PASSWORD_LENGTH	20
#define TOKEN_LENGTH					20

enum enPacketType {
	LOGIN_PACKET_TYPE = 10000,
	REQ_CREATE_ACCOUNT,
	REPLY_CREATE_ACCOUNT,
	REQ_LOGIN,
	REPLY_LOGIN,

	en_PACKET_SS_MONITOR = 20000,
	en_PACKET_SS_MONITOR_LOGIN,
	en_PACKET_SS_MONITOR_DATA_UPDATE,
	en_PACKET_CS_MONITOR = 25000,
	en_PACKET_CS_MONITOR_TOOL_REQ_LOGIN,
	en_PACKET_CS_MONITOR_TOOL_RES_LOGIN,
	en_PACKET_CS_MONITOR_TOOL_DATA_UPDATE
};

enum enReplyCode {
	CRETAE_ACCOUNT_SUCCESS,
	CRETAE_ACCOUNT_FAILURE,

	LOGIN_SUCCESS,
	LOGIN_FAILURE,
};

#pragma pack(push, 1)
struct stMSG_REQ_CREATE_ACCOUNT {
	UINT16	Type;
	WCHAR	AccountID[MAX_OF_ACCOUNT_ID_LENGTH];
	INT32	AccountIdLen;
	WCHAR	AccountPassword[MAX_OF_ACCOUNT_PASSWORD_LENGTH];
	INT32	AccountPasswordLen;
};
struct stMSG_RES_CREATE_ACCOUNT {
	UINT16	Type;
	UINT16	ReplyCode;
};

struct stMSG_REQ_LOGIN {
	UINT16	Type;
	WCHAR	AccountID[MAX_OF_ACCOUNT_ID_LENGTH];
	INT32	AccountIdLen;
	WCHAR	AccountPassword[MAX_OF_ACCOUNT_PASSWORD_LENGTH];
	INT32	AccountPasswordLen;
};
struct stMSG_RES_LOGIN {
	UINT16	Type;
	UINT16	ReplyCode;
	WCHAR	Token[TOKEN_LENGTH];
	INT32	TokenLength;
	UINT16	AccountNo;
};

struct stMSG_MONITOR_DATA_UPDATE {
	WORD	Type;
	BYTE	DataType;
	int		DataValue;
	int		TimeStamp;

};
#pragma pack(pop)

#else
#pragma pack(push, 1)
struct stMSG_LOGIN_REQ
{
	WORD	Type;

	INT64	AccountNo;
	char	SessionKey[64];
};
struct stMSG_LOGIN_RES
{
	WORD	Type;

	INT64	AccountNo;
	BYTE	Status;				// 0 (���ǿ���) / 1 (����) ...  �ϴ� defines ���

	WCHAR	ID[20];				// ����� ID		. null ����
	WCHAR	Nickname[20];		// ����� �г���	. null ����

	WCHAR	GameServerIP[16];	// ���Ӵ�� ����,ä�� ���� ����
	USHORT	GameServerPort;
	WCHAR	ChatServerIP[16];
	USHORT	ChatServerPort;
};

struct stMSG_MONITOR_DATA_UPDATE {
	WORD	Type;
	BYTE	DataType;
	int		DataValue;
	int		TimeStamp;

};
#pragma pack(pop)

enum en_PACKET_TYPE
{
	//------------------------------------------------------
	// Login Server
	//------------------------------------------------------
	en_PACKET_CS_LOGIN_SERVER = 100,

	//------------------------------------------------------------
	// �α��� ������ Ŭ���̾�Ʈ �α��� ��û
	//
	//	{
	//		WORD	Type
	//
	//		INT64	AccountNo
	//		char	SessionKey[64]
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_LOGIN_REQ_LOGIN,

	//------------------------------------------------------------
	// �α��� �������� Ŭ���̾�Ʈ�� �α��� ����
	//
	//	{
	//		WORD	Type
	//
	//		INT64	AccountNo
	//		BYTE	Status				// 0 (���ǿ���) / 1 (����) ...  �ϴ� defines ���
	//
	//		WCHAR	ID[20]				// ����� ID		. null ����
	//		WCHAR	Nickname[20]		// ����� �г���	. null ����
	//
	//		WCHAR	GameServerIP[16]	// ���Ӵ�� ����,ä�� ���� ����
	//		USHORT	GameServerPort
	//		WCHAR	ChatServerIP[16]
	//		USHORT	ChatServerPort
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_LOGIN_RES_LOGIN,








	//------------------------------------------------------
	// Game Server
	//------------------------------------------------------
	en_PACKET_CS_GAME_SERVER = 1000,




	/*  ���� ��ū DB ������� �̻��
	*
	*
		////////////////////////////////////////////////////////
		//
		//   Server & Server Protocol  / LAN ����� �⺻���� ������ ���� ����.
		//
		////////////////////////////////////////////////////////
		en_PACKET_SS_LAN						= 10000,
		//------------------------------------------------------
		// GameServer & LoginServer & ChatServer Protocol
		//------------------------------------------------------

		//------------------------------------------------------------
		// �ٸ� ������ �α��� ������ �α���.
		// �̴� ������ ������, �׳� �α��� ��.
		//
		//	{
		//		WORD	Type
		//
		//		BYTE	ServerType			// dfSERVER_TYPE_GAME / dfSERVER_TYPE_CHAT
		//
		//		WCHAR	ServerName[32]		// �ش� ������ �̸�.
		//	}
		//
		//------------------------------------------------------------
		en_PACKET_SS_LOGINSERVER_LOGIN,



		//------------------------------------------------------------
		// �α��μ������� ����.ä�� ������ ���ο� Ŭ���̾�Ʈ ������ �˸�.
		//
		//	{
		//		WORD	Type
		//
		//		INT64	AccountNo
		//		CHAR	SessionKey[64]
		//	}
		//
		//------------------------------------------------------------
		// en_PACKET_SS_NEW_CLIENT_LOGIN,	// �ű� �������� ����Ű ������Ŷ�� ��û,���䱸���� ���� 2017.01.05


		//------------------------------------------------------------
		// �α��μ������� ����.ä�� ������ ���ο� Ŭ���̾�Ʈ ������ �˸�.
		//
		// �������� Parameter �� ����Ű ������ ���� ������ Ȯ���� ���� � ��. �̴� ���� ������� �ٽ� �ް� ��.
		// ä�ü����� ���Ӽ����� Parameter �� ���� ó���� �ʿ� ������ �״�� Res �� ������� �մϴ�.
		//
		//	{
		//		WORD	Type
		//
		//		INT64	AccountNo
		//		CHAR	SessionKey[64]
		//		INT64	Parameter
		//	}
		//
		//------------------------------------------------------------
		en_PACKET_SS_REQ_NEW_CLIENT_LOGIN,

		//------------------------------------------------------------
		// ����.ä�� ������ ���ο� Ŭ���̾�Ʈ ������Ŷ ���Ű���� ������.
		// ���Ӽ�����, ä�ü����� ��Ŷ�� ������ ������, �α��μ����� Ÿ ������ ���� �� CHAT,GAME ������ �����ϹǷ�
		// �̸� ����ؼ� �˾Ƽ� ���� �ϵ��� ��.
		//
		// �÷��̾��� ���� �α��� �Ϸ�� �� ��Ŷ�� Chat,Game ���ʿ��� �� �޾��� ������.
		//
		// ������ �� Parameter �� �̹� ����Ű ������ ���� ������ �� �ִ� Ư�� ��
		// ClientID �� ����, ���� ī������ ���� ��� ����.
		//
		// �α��μ����� ���Ӱ� �������� �ݺ��ϴ� ��� ������ ���������� ���� ������ ���� ��������
		// �����Ͽ� �ٸ� ����Ű�� ��� ���� ������ ����.
		//
		//	{
		//		WORD	Type
		//
		//		INT64	AccountNo
		//		INT64	Parameter
		//	}
		//
		//------------------------------------------------------------
		en_PACKET_SS_RES_NEW_CLIENT_LOGIN,

		*/

		//------------------------------------------------------
		// Monitor Server Protocol
		//------------------------------------------------------


		////////////////////////////////////////////////////////
		//
		//   MonitorServer & MoniterTool Protocol / ������ ���� ����.
		//
		////////////////////////////////////////////////////////

		//------------------------------------------------------
		// Monitor Server  Protocol
		//------------------------------------------------------
	en_PACKET_SS_MONITOR = 20000,
	//------------------------------------------------------
	// Server -> Monitor Protocol
	//------------------------------------------------------
	//------------------------------------------------------------
	// LoginServer, GameServer , ChatServer  �� ����͸� ������ �α��� ��
	//
	// 
	//	{
	//		WORD	Type
	//
	//		int		ServerNo		//  �� �������� ���� ��ȣ�� �ο��Ͽ� ���
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_SS_MONITOR_LOGIN,

	//------------------------------------------------------------
	// ������ ����͸������� ������ ����
	// �� ������ �ڽ��� ����͸����� ��ġ�� 1�ʸ��� ����͸� ������ ����.
	//
	// ������ �ٿ� �� ��Ÿ ������ ����͸� �����Ͱ� ���޵��� ���ҋ��� ����Ͽ� TimeStamp �� �����Ѵ�.
	// �̴� ����͸� Ŭ���̾�Ʈ���� ���,�� ����Ѵ�.
	// 
	//	{
	//		WORD	Type
	//
	//		BYTE	DataType				// ����͸� ������ Type �ϴ� Define ��.
	//		int		DataValue				// �ش� ������ ��ġ.
	//		int		TimeStamp				// �ش� �����͸� ���� �ð� TIMESTAMP  (time() �Լ�)
	//										// ���� time �Լ��� time_t Ÿ�Ժ����̳� 64bit �� ���񽺷����
	//										// int �� ĳ�����Ͽ� ����. �׷��� 2038�� ������ ��밡��
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_SS_MONITOR_DATA_UPDATE,


	en_PACKET_CS_MONITOR = 25000,
	//------------------------------------------------------
	// Monitor -> Monitor Tool Protocol  (Client <-> Server ��������)
	//------------------------------------------------------
	//------------------------------------------------------------
	// ����͸� Ŭ���̾�Ʈ(��) �� ����͸� ������ �α��� ��û
	//
	//	{
	//		WORD	Type
	//
	//		char	LoginSessionKey[32]		// �α��� ���� Ű. (�̴� ����͸� ������ ���������� ����)
	//										// �� ����͸� ���� ���� Ű�� ������ ���;� ��
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_MONITOR_TOOL_REQ_LOGIN,

	//------------------------------------------------------------
	// ����͸� Ŭ���̾�Ʈ(��) ����͸� ������ �α��� ����
	//
	//	{
	//		WORD	Type
	//
	//		BYTE	Status					// �α��� ��� 0 / 1 / 2 ... �ϴ� Define
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_MONITOR_TOOL_RES_LOGIN,

	//------------------------------------------------------------
	// ����͸� ������ ����͸� Ŭ���̾�Ʈ(��) ���� ����͸� ������ ����
	// 
	// ���� ����͸� ����� ��� ���̹Ƿ�, ����͸� ������ ��� ����͸� Ŭ���̾�Ʈ����
	// �����Ǵ� ��� �����͸� �ٷ� ���۽��� �ش�.
	// 
	//
	// �����͸� �����ϱ� ���ؼ��� �ʴ����� ��� �����͸� ��� 30~40���� ����͸� �����͸� �ϳ��� ��Ŷ���� ����°�
	// ������  �������� ������ ������ �����Ƿ� �׳� ������ ����͸� �����͸� ���������� ����ó�� �Ѵ�.
	//
	//	{
	//		WORD	Type
	//		
	//		BYTE	ServerNo				// ���� No
	//		BYTE	DataType				// ����͸� ������ Type �ϴ� Define ��.
	//		int		DataValue				// �ش� ������ ��ġ.
	//		int		TimeStamp				// �ش� �����͸� ���� �ð� TIMESTAMP  (time() �Լ�)
	//										// ���� time �Լ��� time_t Ÿ�Ժ����̳� 64bit �� ���񽺷����
	//										// int �� ĳ�����Ͽ� ����. �׷��� 2038�� ������ ��밡��
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_MONITOR_TOOL_DATA_UPDATE

};

enum en_PACKET_CS_LOGIN_RES_LOGIN
{
	dfLOGIN_STATUS_NONE = -1,		// ����������
	dfLOGIN_STATUS_FAIL = 0,		// ���ǿ���
	dfLOGIN_STATUS_OK = 1,		// ����
	dfLOGIN_STATUS_GAME = 2,		// ������
	dfLOGIN_STATUS_ACCOUNT_MISS = 3,		// account ���̺� AccountNo ����
	dfLOGIN_STATUS_SESSION_MISS = 4,		// Session ���̺� AccountNo ����
	dfLOGIN_STATUS_STATUS_MISS = 5,		// Status ���̺� AccountNo ����
	dfLOGIN_STATUS_NOSERVER = 6,		// �������� ������ ����.
};

enum en_PACKET_SS_LOGINSERVER_LOGIN
{
	dfSERVER_TYPE_GAME = 1,
	dfSERVER_TYPE_CHAT = 2,
	dfSERVER_TYPE_MONITOR = 3,
};

enum en_PACKET_SS_HEARTBEAT
{
	dfTHREAD_TYPE_WORKER = 1,
	dfTHREAD_TYPE_DB = 2,
	dfTHREAD_TYPE_GAME = 3,
};
#endif

enum en_PACKET_SS_MONITOR_DATA_UPDATE
{
	dfMONITOR_DATA_TYPE_LOGIN_SERVER_RUN = 1,		// �α��μ��� ���࿩�� ON / OFF
	dfMONITOR_DATA_TYPE_LOGIN_SERVER_CPU = 2,		// �α��μ��� CPU ����
	dfMONITOR_DATA_TYPE_LOGIN_SERVER_MEM = 3,		// �α��μ��� �޸� ��� MByte
	dfMONITOR_DATA_TYPE_LOGIN_SESSION = 4,			// �α��μ��� ���� �� (���ؼ� ��)		// LoginServerMont::GetNumOfLoginServerSessions
	dfMONITOR_DATA_TYPE_LOGIN_AUTH_TPS = 5,			// �α��μ��� ���� ó�� �ʴ� Ƚ��		// LoginServerMont::GetLoginServerAuthTPS
	dfMONITOR_DATA_TYPE_LOGIN_PACKET_POOL = 6,		// �α��μ��� ��ŶǮ ��뷮				// JClient::GetAllocMemPoolUsageUnitCnt
};

enum en_SERVER_TYPE {
	dfSERVER_LOGIN_SERVER = 0,
	dfSERVER_ECHO_GAME_SERVER,
	dfSERVER_CHAT_SERVER,
	dfSERVER_SYSTEM
};