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
        std::string port, std::string fileName="report"){

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
            firstGrantEntry = false;
            json << g.toJson();
        }
        json << "]";
    }
    json << "}}";
    string extFN = fileName + ".json";
    std::ofstream of(extFN);
    of << json.str();
    of.close();
    string cmd = "node -e \"console.log(JSON.stringify(require('./" + extFN + "'), null, 2))\" > " + fileName+ "_fmt.json";
    system(cmd.c_str());
    cout << "JSON report file generated : " << fileName << "_fmt.json\n";
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
    for(auto& s : grants){
        if(graph.hasUser(s.first)){
            graph.get_user_grants(s.first, s.second);
            for(auto& grant : s.second){
                grantObjects[s.first].emplace_back(grant);
            }
        }
    }
    generateReport(grantObjects,host, user, port, "Report_testing");


    return 0;
}
