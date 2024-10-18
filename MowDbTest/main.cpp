#include "JNetDB.h"
#include <iostream>

using namespace jnet;
using namespace std;

#define ODBC_CONNECTION_STRING				L"Driver={MySQL ODBC 9.1 ANSI Driver};Server=127.0.0.1;User=root;Password=607281;Option=3;"

const WCHAR* Query_AccountId = L"SELECT accountId FROM mowaccountdb.account WHERE accountid = ?";
const WCHAR* Query_AccountPassword = L"SELECT accountpassword FROM mowaccountdb.account WHERE accountid = ?";
const WCHAR* Query_InsertNewAccount = L"INSERT mowaccountdb.account (accountid, accountpassword) VALUES(?, ?)";

int main() {
	JNetDBConnPool dbConnPool(true);
	if (!dbConnPool.Connect(1, ODBC_CONNECTION_STRING)) {
		cout << "Fail Conn" << endl;
	}

	wchar_t accountID[20] = L"HUR_JIN";
	wchar_t accountPassword[20] = L"12345";

	JNetDBConn* dbConn = dbConnPool.Pop();
	dbConn->Unbind();

	if (!dbConn->BindParam(1, accountID)) {
		DebugBreak();
	}
	if (!dbConn->Execute(Query_AccountId)) {
		DebugBreak();
	}
	cout << dbConn->GetRowCount() << endl;

	// ---------------------

	dbConn->Unbind();
	
	if (!dbConn->BindParam(1, accountID) || !dbConn->BindParam(2, accountPassword)) {
		DebugBreak();
	}
	if (!dbConn->Execute(Query_InsertNewAccount)) {
		DebugBreak();
	}
	cout << dbConn->GetRowCount() << endl;

	// ---------------------

	dbConn->Unbind();
	if (!dbConn->BindParam(1, accountID)) {
		DebugBreak();
	}
	if (!dbConn->Execute(Query_AccountPassword)) {
		DebugBreak();
	}

	WCHAR password[20];
	if (!dbConn->BindCol(1, password, sizeof(password), NULL)) {
		DebugBreak();
	}
	if (!dbConn->Fetch()) {
		DebugBreak();
	}
	wcout << password << endl;


	// --------------------
	
	return 0;
}
