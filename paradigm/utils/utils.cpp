#include "utils.h"
char charToHex(char H, char L)
{
    if((H >= '0') && (H <= '9'))
    {
        H -= '0';
    }
    else if((H >= 'A') && (H <= 'F'))
    {
        H = H - 'A' + 10;
    }
    else
    {
        H = '*';
    }

    if((L >= '0') && (L <= '9'))
    {
        L -= '0';
    }
    else if((L >= 'A') && (L <= 'F'))
    {
        L = L - 'A' + 10;
    }
    else
    {
        L = '*';
    }
    return ((H & 0x0F) << 4) | (L & 0x0F);
}

QByteArray hexStr_To_Hexchar(QString data)
{
    QByteArray hexArray;

    data.remove(' ');    //删除字符串中的空格
    data = data.toUpper();
    int Lenth = data.length(); //获取长度
    if(Lenth % 2 == 1)
    {
        data += "0";
        Lenth++;
    }

    for (int i = 0; i < Lenth; i += 2)
    {
        hexArray.push_back(charToHex(data[i].toLatin1(), data[i + 1].toLatin1()));
    }
    return hexArray;
}
