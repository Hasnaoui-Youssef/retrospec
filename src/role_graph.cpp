#include"role_graph.h"
#include<algorithm>
#include <cstdlib>
#include <deque>
#include <fstream>
#include<iostream>
using std::ofstream;
using std::cout;


void RoleGraph::addEdge(const string& from, const string& to, bool isAdmin){
    adj_list[to].push_back(from);
}


void RoleGraph::addUser(string& user){
    users.insert(user);
}


bool RoleGraph::hasCycle(const string& node, set<string>& visited, set<string>& recStack, vector<string>& cyclePath){
    if(recStack.find(node) != recStack.end()){
        cyclePath.push_back(node);
        return true;
    }

    if (visited.find(node) != visited.end()){
        return false;
    }

    visited.insert(node);
    recStack.insert(node);
    cyclePath.push_back(node);
    for(const auto& neigh : adj_list[node]){
        if(hasCycle(neigh, visited, recStack, cyclePath)){
            return true;
        }
    }

    recStack.erase(node);
    cyclePath.pop_back();
    return false;
}

bool RoleGraph::detectCircularDependencies(vector<vector<string>>& cycles){
    set<string> visited;
    bool hasCycle = false;
    for (const auto& entry : adj_list){
        const string& node = entry.first;
        if(visited.find(node) != visited.end()) continue;
        set<string> recStack;
        vector<string> currPath;
        if(this->hasCycle(node, visited, recStack, currPath)){
            string lastNode = currPath.back();
            auto cycleStart = find(currPath.begin(), currPath.end(), lastNode);
            vector<string> cycle(cycleStart, currPath.end());
            cycles.push_back(cycle);
            hasCycle = true;
        }
    }
    return hasCycle;
}

void RoleGraph::generateViz(const string& filename){
    ofstream dotFile(filename + ".gv");
    dotFile << "digraph G {\n";
    dotFile << "    rankdir=LR;\n";
    dotFile << "    node [shape=box, style=filled, fillcolor=whitesmoke];\n";
    for(const auto& entry : adj_list) dotFile << "    \"" << entry.first << "\";\n";
    for(const auto & entry : adj_list){
        for(const auto& neigh : entry.second){
            dotFile << "    \"" << entry.first << "\" -> \"" << neigh << "\";\n";
        }
    }
    dotFile << "}\n";
    dotFile.close();
    cout << "Graphviz file generated : " << filename << ".gv";
    string command =  "dot -Tpng " + filename + ".gv -o "+ filename + ".png";
    cout << "Running \"" << command << "\"\n";
    system(command.c_str());
}


void RoleGraph::get_user_grants(string node, vector<string>& uGrants){
    if(users.find(node) == users.end()) return;
    cout << "Grants for user : " << node << "\n";
    set<string> visited;
    std::deque<string> q;
    q.push_back(node);
    while(!q.empty()){
        string n = q.front();
        q.pop_front();
        if(visited.find(n) != visited.end()){
            cout << "Node already visited!\n";
            continue;
        }
        visited.insert(n);
        if(adj_list.find(n) != adj_list.end()){
            for(int i = 0; i < adj_list[n].size(); ++i) q.push_back(adj_list[n][i]);
        }
        if(n == node) continue;
        if(user_grants.find(n) != user_grants.end()){
            for(int i = 0; i < user_grants[n].size(); ++i) uGrants.push_back(user_grants[n][i]);
        }
    }
}

//This function assumes we have a coherent  adjacency list
void RoleGraph::setGrants(unordered_map<string, vector<string>>& g){
    for(auto& elem : g){
        if(users.find(elem.first) != users.end()){
            user_grants[elem.first] = elem.second;
        }
    }
}
