#pragma once
#include"role_graph.h"
#include<unordered_map>
#include<vector>
#include<string>

using std::unordered_map;
using std::string;
using std::vector;

class UserGrants{
    unordered_map<string, vector<string>> grant_map;
};
