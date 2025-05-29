#pragma once
#include "role_graph.h"
#include<string>

using std::string;

class SGDB_Analyzer {
public:
    std::string username;
    std::string password;
    std::string host;
    std::string db;
    int port;
    virtual void connect();
    virtual void disconnect();
    SGDB_Analyzer() = delete;
    SGDB_Analyzer(std::string username,std::string password,std::string host,std::string db,int port);
    virtual RoleGraph buildRoleGraph() = 0;
    virtual void generateReport(std::string fileName);
};
