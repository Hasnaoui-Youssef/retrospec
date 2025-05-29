#include"grants_re.h"
#include <fstream>
#include<sstream>
#include<iostream>
#include<cctype>
#include<algorithm>
#include <stdexcept>

using std::ofstream;

GrantStatement::GrantStatement(const string& statement){
    string str = statement;

    str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
    str = str.substr(0, str.find("WITH GRANT OPTION")); // Afterwards we have to take this into account
    std::istringstream iss(str);
    string token;
    iss >> this->op;
    std::transform(this->op.begin(), this->op.end(), this->op.begin(), ::toupper);
    string privileges;
    size_t ONpos = str.find("ON");
    if ( ONpos != string::npos ){
        privileges = str.substr(this->op.length() + 1, ONpos - this->op.length() - 1);
    }
    vector<string> privTokens;
    std::istringstream pIss(privileges);
    while (std::getline(pIss, token, ',')) {
        privTokens.push_back(token);
    }
    for(auto& t : privTokens){
        t.erase(std::remove_if(t.begin(), t.end(), ::isspace), t.end());
        size_t openPpos = t.find('(');
        if( openPpos != string::npos ){
            string privName = t.substr(0, openPpos);
            string cols = t.substr(openPpos + 1, t.find(')') - openPpos - 1);
            vector<string> c;
            std::istringstream prIss(cols);
            while( std::getline(prIss, token, ',')){
                c.push_back(token);
            }
            this->privs.emplace_back(privName, c);
        }else {
            this->privs.emplace_back(t, vector<string>());
        }
    }

    size_t toPos = str.find("TO");
    if(toPos == string::npos){
        throw std::runtime_error("No TO clause");
    }
    string target = str.substr(ONpos + 3, toPos - ONpos - 3);
    target.erase(std::remove(target.begin(), target.end(), '`'), target.end());
    size_t dotPos = target.find('.');
    if(dotPos != string::npos){
        this->db = target.substr(0, dotPos);
        this->table = target.substr(dotPos + 1);
    }else{
        this->table = target;
        this->db = "";
    }
    string userHost = str.substr(toPos + 3);
    userHost.erase(std::remove(userHost.begin(), userHost.end(), '\''), userHost.end());
    size_t atPos = userHost.find('@');
    if(atPos == string::npos){
        throw std::runtime_error("No @ defining user and host");
    }
    this->user = userHost.substr(0, atPos);
    this->host = userHost.substr(atPos + 1);
}

void GrantStatement::logInfo(){
    std::cout << "\n==========================================================================================\n";
    std::cout << "Privileges : \n";
    std::cout << "Type : " << this->op << "\n";
    for(auto& p : this->privs){
        std::cout << "\n**********************************\n";
        std::cout << "---Name : " << p.first << "\n---Cols : ";
        if(p.second.size() == 0){
            std::cout << "ALL";
        }else{
            for (auto& c : p.second){
                std::cout << c << " ";
            }
        }
        std::cout << "\n**********************************\n";
    }
    std::cout << "Database : " << this->db << "\n";
    std::cout << "Table : " << this->table << "\n";
    std::cout << "User : " << this->user << "\n";
    std::cout << "Host : " << this->host << "\n";
    std::cout << "\n==========================================================================================\n";
}


std::string GrantStatement::toJson(){
    std::stringstream json;
    json << "{";
    json << "\"type\" : \"" << this->op << "\",";
    json << "\"privs\" : [ ";
    for(auto& p : this->privs){
        json << "{";
        json << "\"name\" : \"" << p.first << "\"," ;
        json << "\"cols\" : [";
        if(p.second.size() == 0){
            json << "\"ALL\"";
        }else{
            for (auto& c : p.second){
                json << "\"" << c << ((c == p.second.back()) ? "\"" : "\",");
            }
        }
        json << "]";
        json << (p.first == this->privs.back().first ? "" : "");
    }
    json << "],";
    json << "\"database\" : \"" << this->db << "\",";
    json << "\"table\" : \"" << this->table << "\",";
    json << "\"user\" : \"" << this->user << "\",";
    json << "\"host\" : \"" << this->host << "\"";
    json << "}";
    return json.str();
}
