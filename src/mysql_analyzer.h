#include<mysql/mysql.h>
#include"role_graph.h"
#include<string>
#include<vector>
#include<unordered_map>

using std::unordered_map;
using std::vector;


class MySqlAnalyzer{
    MYSQL* conn;
public:
    MySqlAnalyzer(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
    ~MySqlAnalyzer();
    RoleGraph buildRoleGraph();
    unordered_map<string,vector<std::string>> getGrants();
};
