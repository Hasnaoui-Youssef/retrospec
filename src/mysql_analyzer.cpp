#include"mysql_analyzer.h"
#include <mysql/mysql.h>
#include <mysql/mysql_com.h>
#include <sstream>
#include <stdexcept>
#include<iostream>
#include<fstream>


MySqlAnalyzer::MySqlAnalyzer(const std::string& host, const std::string& user, const std::string& pass, const std::string& db){
    conn = mysql_init(nullptr);
    if(!mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(), db.c_str(),0, nullptr, CLIENT_MULTI_STATEMENTS)){
        throw std::runtime_error("Failed to connect to MySQL server : " + std::string(mysql_error(conn)));
    }
}

MySqlAnalyzer::~MySqlAnalyzer(){
    mysql_close(conn);
}
RoleGraph MySqlAnalyzer::buildRoleGraph(){
    RoleGraph graph;
    std::string query = "select from_user, from_host, to_user, to_host from mysql.role_edges";
    if(mysql_query(conn, query.c_str())){
        throw std::runtime_error("Failed to query role edges : " + std::string(mysql_error(conn)));
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if(!res){
        throw std::runtime_error("Failed to get role edges result : " + std::string(mysql_error(conn)));
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))){
        std::string from = std::string(row[0]);
        std::string to = std::string(row[2]);
        bool isAdmin = (std::string(row[4]) == "N") ? false : true;

        graph.addEdge(from, to, isAdmin);
        graph.addUser(from);
        graph.addUser(to);
    }
    mysql_free_result(res);
    return graph;
}

std::unordered_map<std::string,std::vector<std::string>> MySqlAnalyzer::getGrants(){
    std::unordered_map<std::string, std::vector<std::string>> grants;
    std::ifstream query_file("queries/mysql_grants_re.sql");
    std::stringstream ss;
    ss << query_file.rdbuf();
    std::string query = ss.str();
    if(mysql_query(conn, query.c_str())){
        throw std::runtime_error("Failed to query grants : " + std::string(mysql_error(conn)));
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if(res){
        mysql_free_result(res);
        res = nullptr;
    }

    if(mysql_errno(conn)){
        throw std::runtime_error("Initial statement error : " + std::string(mysql_error(conn)));
    }

    if(mysql_next_result(conn) != 0){
        throw std::runtime_error("No select result : " + std::string(mysql_error(conn)));
    }

    res = mysql_store_result(conn);
    if(!res){
        throw std::runtime_error("Failed to get grants result : " + std::string(mysql_error(conn)));
    }

    MYSQL_ROW row;
    std::string grant_statement;
    std::string user;
    while((row = mysql_fetch_row(res))){
        user = std::string(row[0]);
        grant_statement = std::string(row[5]);
        grants[user].push_back(grant_statement);
    }
    return grants;
}
