#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <HomerMain.h>

using namespace std;

class HomerDao {
public:
  ~HomerDao();
  void connect(string& connection);

private:
  SQLHENV mEnv;
  SQLHDBC mConn;
  void checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type);
};

HomerDao::~HomerDao() {
  SQLFreeHandle(SQL_HANDLE_ENV, mEnv);
  SQLFreeHandle(SQL_HANDLE_ENV, mConn);
}

void HomerDao::connect(string& connection) {
  SQLRETURN err;
  err = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mEnv);
  checkError(err, mEnv, SQL_HANDLE_ENV);
   
  err = SQLSetEnvAttr(mEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
  checkError(err, mEnv, SQL_HANDLE_ENV);

  err = SQLAllocHandle(SQL_HANDLE_DBC, mEnv, &mConn);
  checkError(err, mConn, SQL_HANDLE_DBC);

  err = SQLDriverConnect(mConn, NULL, (SQLCHAR*)connection.c_str(), SQL_NTS,
          NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
  checkError(err, mConn, SQL_HANDLE_DBC);
}

void HomerDao::checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type) {
   SQLINTEGER i = 0;
   SQLINTEGER native;
   SQLCHAR state[ 7 ];
   SQLCHAR text[256];
   SQLSMALLINT len;
   SQLRETURN ret;

   if (!SQL_SUCCEEDED(err)) {
     ret = SQLGetDiagRec(type, handle, 1, state, &native, text, sizeof(text), &len);
     if (SQL_SUCCEEDED(ret)) {
       throw new string((char *)text);
     } else {
       throw new string("SQL error");
     }
   }
}

int main(int argc, char *argv[]) {
  HomerDao dao;
  string conn("DATABASE=homer_db;SERVER=localhost;UID=root;DRIVER=MySQL;READONLY=0;");
  try {
    dao.connect(conn);
  } catch (string* e) {
    std::cout << e->c_str();
  }
}

