#ifndef SJSON_H
#define SJSON_H

#include "configor/json.hpp"
#include <string>
class SJson
{
public:
    SJson();
    SJson(std::string path);
    ~SJson();
private:
    std::string m_file_path;
protected:
    configor::json json_data;
    std::string m_last_error = "none";
public:
    void SetFilePath(std::string path);
    std::string GetFilePath();
    virtual bool ReadFromFile();
    std::string GetLastError();
};



#endif // SJSON_H
