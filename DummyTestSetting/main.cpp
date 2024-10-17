#include "JNetDB.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace jnet;

//const WCHAR* ConnectionString = L"Driver={MySQL ODBC 8.4 ANSI Driver};Server=127.0.0.1;Database=logdb;User=mainserver;Password=607281;Option=3;";
const WCHAR* ConnectionString = L"Driver={MySQL ODBC 9.1 ANSI Driver};Server=127.0.0.1;User=root;Password=607281;Option=3;";

const wstring InitFilePath = L".\\sql\\_accountdb.sql";
const wstring DummyDataFilePath = L".\\sql\\_Dummy_InsertQuery.sql";

// SQL 파일에서 내용을 읽는 함수
std::wstring ReadSQLFromFile(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (!file) {
        std::wcerr << L"Failed to open SQL file: " << filePath << std::endl;
        return L"";
    }
    std::wstringstream queryStream;
    queryStream << file.rdbuf();
    return queryStream.str();
}

// 세미콜론으로 구분된 쿼리들을 분리하는 함수
std::wstring Trim(const std::wstring& str) {
    size_t start = str.find_first_not_of(L" \t\n\r");
    size_t end = str.find_last_not_of(L" \t\n\r");
    if (start == std::wstring::npos || end == std::wstring::npos) {
        return L"";  // 모두 공백인 경우 빈 문자열 반환
    }
    return str.substr(start, end - start + 1);
}
std::vector<std::wstring> SplitQueries(const std::wstring& sqlContent) {
    std::vector<std::wstring> queries;
    std::wstringstream ss(sqlContent);
    std::wstring query;

    while (std::getline(ss, query, L';')) {
        std::wstring trimmedQuery = Trim(query);
        if (!trimmedQuery.empty()) {
            queries.push_back(trimmedQuery + L";");  // 세미콜론 포함
        }
    }

    return queries;
}

int main() {
	JNetDBConnPool dbConnPool(true);
    if (!dbConnPool.Connect(1, ConnectionString)) {
        cout << "Failed to make connection to db" << endl;
        return 0;
    }

    JNetDBConn* dbConn = dbConnPool.Pop();

    wstring initSqlContent = ReadSQLFromFile(InitFilePath);
    vector<wstring> initQueries = SplitQueries(initSqlContent);
    for (const auto& query : initQueries) {
        if (!dbConn->Execute(query.c_str())) {
            wcout << "[Fail]: " << query << endl;
        }
    }
    
    wstring insertDummyData = ReadSQLFromFile(DummyDataFilePath);
    vector<wstring> insertQueries = SplitQueries(insertDummyData);
    for (const auto& query : insertQueries) {
        if (!dbConn->Execute(query.c_str())) {
            wcout << "[Fail]: " << query << endl;
        }
    }
}