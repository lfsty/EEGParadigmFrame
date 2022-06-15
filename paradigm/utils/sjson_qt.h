#ifndef SJSON_QT_H
#define SJSON_QT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QByteArray>
class SJson_Qt
{
public:
    SJson_Qt();
    SJson_Qt(QString file_path);
private:
    QString m_file_path;
    QString m_last_error;
protected:
    QJsonDocument  m_root_Doc;
public:
    void SetFilePath(QString file_path);
    QString GetFilePath();
    bool ReadFromFile();
    QString GetLastError();
    QJsonDocument GetJsonDoc();

};

#endif // SJSON_QT_H
