#pragma once
#include<vector>
#include<string>

class Table_Schema{
    std::string name;
    std::string schema;
    std::vector<std::string> cols;
public:
    void setName(const std::string& name);
    void setSchema(const std::string& schema);
    void setCols(const std::vector<std::string>& cols);
    std::string& getName(){ return name; }
    std::string& getSchema(){ return schema; }
    std::vector<std::string>& getCols(){ return cols; }
};
