#include "unzip.h"
#include <QDebug>
#include <QTemporaryDir>
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QFile>
bool unzip::UnZip(QString dest, QString src)
{
    QTemporaryDir temp_dir;
    if(temp_dir.isValid()){
        init(temp_dir.path()+"/");
    }else{
        return false;
    }

    QProcess p(0);
    QStringList args = QStringList();
    args.append("x");
    args.append(src);
    args.append("-o"+dest);//-o和目标目录必须连在一起
    args.append("-aoa");//不提示,直接覆盖同名文件
    p.start(temp_dir.path() + "/7z.exe",args);
    p.waitForStarted();
    p.waitForFinished();

    return true;
}

void unzip::init(QString bin_path)
{
    //添加依赖
    QStringList depends;
    depends.append("7z.exe");
    depends.append("7z.dll");

    for(QString file : depends){
        QString local_file = bin_path+file;
        if(QDir().exists(file) == false){
            QString qrc_file = ":7z/bin/"+file;
            QFile::copy(qrc_file,local_file);
        }
    }
}
