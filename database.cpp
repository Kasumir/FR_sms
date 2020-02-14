#include <database.h>

db::db(){
    SERVER = "localhost";
    USER = "root";
    PASSWORD = "intel1234";
    DATABASE = "fd_user";
    conn = NULL;
    res = NULL;
    row = NULL;
    fields = 0;
}

db::db(char* server, char* user, char* password, char* database){
    SERVER = server;
    USER = user;
    PASSWORD = password;
    DATABASE = database;
    conn = NULL;
    res = NULL;
    row = NULL;
    fields = 0;
}

db::~db(){
    db::close_db();
}

int db::connect_db(){
    conn = mysql_init(NULL);
    if(!conn){
        std::cout << "Mysql Initialization Failed";
        return -1;
    }
    mysql_real_connect(conn, SERVER, USER, PASSWORD, DATABASE, NULL, NULL, 0);
    if(conn)
        ;
    else{
        std::cout << "Connection Failed\n";
        return -1;
    }
    return 0;
}

void db::close_db(){
    mysql_free_result(res);
    mysql_close(conn);
}

std::vector<std::string> db::get_image_path(){
    std::vector<std::string> v;
    mysql_query(conn, "use fd_user");
    mysql_query(conn, "SELECT image_path FROM student_info ORDER BY std_num");
        res = mysql_store_result(conn);

        fields = mysql_num_fields(res);
        while((row = mysql_fetch_row(res))!= NULL){
            for(int i = 0; i < fields; i++)
                v.push_back(row[i]);
        }
    return v;
}

std::string db::get_image_path(std::string str){
    std::string s = "SELECT image_path FROM student_info WHERE std_num = " + str;
    mysql_query(conn, "use fd_user");
    mysql_query(conn, s.c_str());

    res = mysql_store_result(conn);
    fields = mysql_num_fields(res);
    row = mysql_fetch_row(res);
    s = row[0];
    return s;
}

std::vector<std::string> db::get_phone_num(){
    std::vector<std::string> v;
    mysql_query(conn, "use fd_user");
    mysql_query(conn, "SELECT phone_num FROM student_info ORDER BY std_num");
        res = mysql_store_result(conn);

        fields = mysql_num_fields(res);
        while((row = mysql_fetch_row(res))!= NULL){
            for(int i = 0; i < fields; i++)
                v.push_back(row[i]);
        }
    return v;
}

std::string db::get_phone_num(std::string str){
    std::string s = "SELECT phone_num FROM student_info WHERE std_num = " + str;
    mysql_query(conn, "use fd_user");
    mysql_query(conn, s.c_str());

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    s = row[0];
    return s;
}

void db::create_profile(std::string name, std::string phonenum, std::string image_path){
    std::string query = "INSERT INTO student_info VALUES(" + db::size() + ", \"" + name + "\", \"" + phonenum + "\", \"" + image_path + "\", NULL)";

    mysql_query(conn, "use fd_user");
    mysql_query(conn, query.c_str());
}

std::string db::size(){
    mysql_query(conn, "use fd_user");
    mysql_query(conn, "SELECT count(*) FROM student_info");
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    std::string str = row[0];
    return str;
}
