#ifndef UNZIP_H
#define UNZIP_H

#include <QString>

class unzip
{
public:
    static bool UnZip(QString dest,QString src);
private:
    static void init(QString bin_path);
};

#endif // UNZIP_H
