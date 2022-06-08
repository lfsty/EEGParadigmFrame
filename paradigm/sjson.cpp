#include "sjson.h"
#include <fstream>
SJson::SJson()
{

}

SJson::SJson(std::string path)
{
    SetFilePath(path);
}

void SJson::SetFilePath(std::string path)
{
    m_file_path = path;
}

std::string SJson::GetFilePath()
{
    return m_file_path;
}

bool SJson::ReadFromFile()
{
    std::ifstream ifs;
    ifs.open(GetFilePath());
    //打开文件失败
    if(!ifs.is_open())
    {
        m_last_error = "open file error";
        return false;
    }

    //尝试解析json
    try
    {
        ifs >> json_data;
    }
    catch (...)
    {
        //解析json出错
        m_last_error = "parse json error";
        return false;
    }

    m_last_error = "none";
    return true;
}
