#include <mysql.h>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <cstdlib>


#ifndef DATABASE_H
#define DATABASE_H

using namespace std;
class db
{
private:
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int fields;
    char* SERVER;
    char* USER;
    char* PASSWORD;
    char* DATABASE;
public:
    db();
    db(char* server, char* user, char* password, char* database);
    ~db();
    int connect_db();
    void close_db();

    //goodgodd
    std::string size();
    vector<vector<string>> getData(int n, int m);
    void delData(int n);
    void updateData(int n, int m, string data);


    void create_profile(std::string name, std::string phonenum, std::string image_path);

};
#endif // DATABASE_H
