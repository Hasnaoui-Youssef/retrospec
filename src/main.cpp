#include"mysql_analyzer.h"
#include "role_graph.h"
#include"grants_re.h"
#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>


using std::string;
using std::cout;
using std::vector;
using std::unordered_map;

int main(int argc, char* argv[]){
    if(argc < 5){
        cout << "Please provide db credentials : ./{filename} <host> <user> <password> <db> [<graphFileName>]";
    }
    string host = string(argv[1]);
    string user = string(argv[2]);
    string pass = string(argv[3]);
    string db = string(argv[4]);
    MySqlAnalyzer analyzer(host, user, pass, db);
    RoleGraph graph = analyzer.buildRoleGraph();
    auto grants = analyzer.getGrants();
    if(argc >= 6){
        string fileName = string(argv[5]);
        graph.generateViz(fileName);
    }else{
        graph.generateViz();
    }
    graph.setGrants(grants);
    unordered_map<string, vector<GrantStatement>> grantObjects;
    for(auto& s : grants){
        if(graph.hasUser(s.first)){
            graph.get_user_grants(s.first, s.second);
            for(auto& grant : s.second){
                grantObjects[s.first].emplace_back(grant);
            }
        }
    }
    for(auto& p : grantObjects){
        cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
        cout << "Grants for user : " << p.first << "\n";
        cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
        for(auto& g : p.second){
            g.logInfo();
        }
    }
    return 0;
}
