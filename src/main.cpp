#include"mysql_analyzer.h"
#include "role_graph.h"
#include"grants_re.h"
#include <fstream>
#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<sstream>


using std::string;
using std::cout;
using std::vector;
using std::unordered_map;

void generateReport(std::unordered_map<std::string, std::vector<GrantStatement>> grantObjects,
        std::string host, std::string user,
        std::string port, std::string fileName="report.json"){

    std::stringstream json;
    json << "{";
    json << "\"DBMS\" : \"" << "MySQL" << "\",";
    json << "\"server\" : \"" << host << "\",";
    json << "\"connectedUser\" : \"" << user << "\",";
    json << "\"port\" : \"" << port << "\",";
    json << "\"userGrants\" : { ";
    bool firstMapEntry = true;
    for(auto& p : grantObjects){
        if(!firstMapEntry){
            json << ",";
        }
        firstMapEntry = false;
        json << "\""<< p.first <<"\" : " << "[";
        bool firstGrantEntry = true;
        for(auto& g : p.second){
            if(!firstGrantEntry){
                json << ",";
            }
            json << g.toJson();
        }
        json << "]";
    }
    json << "}}";
    std::ofstream of(fileName);
    of << json.str();
    of.close();
}

int main(int argc, char* argv[]){
    if(argc < 5){
        cout << "Please provide db credentials : ./{filename} <host> <user> <password> <db> [<graphFileName>]";
    }
    string host = string(argv[1]);
    string user = string(argv[2]);
    string pass = string(argv[3]);
    string db = string(argv[4]);
    string port = "3306";
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
    generateReport(grantObjects,host, user, port, "Report_testing.json");
    for(auto& s : grants){
        if(graph.hasUser(s.first)){
            graph.get_user_grants(s.first, s.second);
            for(auto& grant : s.second){
                grantObjects[s.first].emplace_back(grant);
            }
        }
    }


    return 0;
}
