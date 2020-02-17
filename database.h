#include <mysql.h>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <cstdlib>


#ifndef DATABASE_H
#define DATABASE_H
/*
 * datebase name  =  fd_user
 * table name  = student_info
 *
 * int std_num
 * char * name
 * char * phone_num
 * char * image_path
 * date date
 * char * YN
 *
 * std_num starts at 0 and is continuous.
 * if YN == 0   doesn't send message
 */




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


    void create_profile(std::string name, std::string phonenum, std::string image_path, std::string YN);

};
#endif // DATABASE_H
