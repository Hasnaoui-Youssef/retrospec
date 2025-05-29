#pragma once

#include<string>
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<set>

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::set;



class RoleGraph{
    std::unordered_map<std::string, std::vector<std::string>> adj_list;
    std::unordered_map<std::string, std::vector<std::string>> user_grants;
    std::set<std::string> users;

    bool hasCycle(const string& node, set<string>& visited, set<string>& recStack, vector<string>& cyclePath);

public:
    void addEdge(const string& from, const string& to, const bool is_admin);
    bool detectCircularDependencies(vector<vector<string>>& cycles);
    bool hasUser(string user);
    void generateViz(const string& filename = "graph");
    void addUser(string& user);
    void setGrants(unordered_map<string, vector<string>>&);
    void get_user_grants(string node, vector<string>& uGrants);
};
