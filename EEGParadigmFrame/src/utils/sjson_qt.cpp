#include "sjson_qt.h"

SJson_Qt::SJson_Qt()
{

}

SJson_Qt::SJson_Qt(QString file_path)
{
    SetFilePath(file_path);
}

void SJson_Qt::SetFilePath(QString file_path)
{
    m_file_path = file_path;
}

bool SJson_Qt::ReadFromFile()
{
    QFile json_file(m_file_path);
    if(!json_file.open(QIODevice::ReadOnly))
    {
        m_last_error = "json文件打开出错";
        return false;
    }
    const QByteArray allData = json_file.readAll();
    json_file.close();

    QJsonParseError jsonError;
    m_root_Doc = QJsonDocument::fromJson(allData, &jsonError);

    if(jsonError.error != QJsonParseError::NoError)
    {
        m_last_error =  "json解析出错:" + jsonError.errorString();
        return false;
    }
    return true;
}

QString SJson_Qt::GetLastError()
{
    return m_last_error;
}

QJsonDocument SJson_Qt::GetJsonDoc()
{
    return m_root_Doc;
}
