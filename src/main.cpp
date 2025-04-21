#include"mysql_analyzer.h"
#include "role_graph.h"
#include<iostream>
#include<string>


using std::string;
using std::cout;

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
    for(auto& s : grants){
        graph.get_user_grants(s.first, s.second);
        for(auto& grant : s.second){
            cout << grant << "\n";
        }
    }
    return 0;
}
