#include <database.h>
#include <cstdlib>

db::db(){
    SERVER = "localhost";
    USER = "root";
    PASSWORD = "intel1234";
    DATABASE = "mysql";
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

    mysql_real_connect(conn, SERVER, USER, PASSWORD, NULL, NULL, NULL, 0);
    if(conn)
        ;
    else{
        std::cout << "Connection Failed\n";
        return -1;
    }
    mysql_query(conn, "CREATE DATABASE fd_user default CHARACTER SET UTF8;");
    mysql_query(conn, "use fd_user");
    mysql_query(conn, "CREATE TABLE student_info(std_num INT PRIMARY KEY,name VARCHAR(20) NOT NULL,phone_num VARCHAR(20) NOT NULL,image_path VARCHAR(100) NOT NULL, date DATE,YN VARCHAR(5) NOT NULL)");
    return 0;
}

void db::close_db(){
    mysql_free_result(res);
    mysql_close(conn);
}

void db::create_profile(std::string name, std::string phonenum, std::string image_path, std::string YN){
    std::string query = "INSERT INTO student_info VALUES(" + db::size() + ", \"" + name + "\", \"" + phonenum + "\", \"" + image_path + "\", NULL" + ",\"" + YN + "\")";

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

vector<vector<string>> db::getData(int n, int m){
    /* select column[m]      or      *
     * from student_info
     * where std_num = n     or      x
     * order by std_num
     */
    unsigned long b = std::atoi(db::size().c_str());
    vector<vector<string>> data(b);


    string query = "";
    string column[6] = {"std_num", "name", "phone_num", "image_path", "IFNULL(date, \"NULL\")", "YN"};

    if(m > 5 || m < -1 || n < -1){
        return data;
    }
    if(m >= 0){
        query += ("SELECT " + column[m] + " FROM student_info ");
    }
    else{
        query += "SELECT std_num, name, phone_num, image_path, IFNULL(date, \"NULL\"), YN FROM student_info ";
    }

    if(n >= 0){
        query += ("WHERE std_num = " + std::to_string(n) + " ");
    }

    query += "ORDER BY std_num";



    mysql_query(conn, "use fd_user");
    mysql_query(conn, query.c_str());
    res = mysql_store_result(conn);

    fields = mysql_num_fields(res);
    int j = 0;
    while((row = mysql_fetch_row(res))!= NULL){
        for(int i = 0; i < fields; i++){
            data[j].push_back(row[i]);
        }
        j++;
    }

    cout << query << endl;
    return data;
}

void db::delData(int n){
    string del_query = "";
    string sort_query = "";
    mysql_query(conn, "use fd_user");

    del_query += ("DELETE FROM student_info WHERE std_num = " + std::to_string(n));
    mysql_query(conn, del_query.c_str());

    sort_query += ("UPDATE student_info SET std_num = std_num - 1 WHERE std_num > " + std::to_string(n));
    mysql_query(conn, sort_query.c_str());
}

void db::updateData(int n, int m, string data){
    /* UPDATE student_info
     * SET column[m] = data
     * WHERE std_num = n
     */
    string column[5] = {"std_num", "name", "phone_num", "image_path", "date"};
    string query = "UPDATE student_info SET " + column[m] + " = \"" + data + "\" WHERE std_num = " + to_string(n);
    mysql_query(conn, "use fd_user");
    mysql_query(conn, query.c_str());


    cout << query << endl;
}








































