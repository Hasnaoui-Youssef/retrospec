#pragma once
#include"role_graph.h"
#include<unordered_map>
#include <utility>
#include<vector>
#include<string>

using std::unordered_map;
using std::string;
using std::vector;
using std::pair;

class GrantStatement {
    string op;
    vector<pair<string, vector<string>>> privs;
    string db;
    string table;
    vector<string> cols;
    string user;
    string host;
    bool with_grant_option;
public:
    GrantStatement(const string&);
    void logInfo();
    std::string toJson();
};
