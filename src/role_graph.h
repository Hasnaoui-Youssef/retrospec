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
    unordered_map<string, vector<string>> adj_list;
    unordered_map<string, vector<string>> user_grants;
    set<string> users;

    bool hasCycle(const string& node, set<string>& visited, set<string>& recStack, vector<string>& cyclePath);

public:
    void addEdge(const string& from, const string& to, const bool is_admin);
    bool detectCircularDependencies(vector<vector<string>>& cycles);
    void generateViz(const string& filename = "graph");
    void addUser(string& user);
    void setGrants(unordered_map<string, vector<string>>&);
    void get_user_grants(string node, vector<string>& uGrants);
};
