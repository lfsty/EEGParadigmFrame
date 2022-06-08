#ifndef SSERIAL_H
#define SSERIAL_H


#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QByteArray>
class SSerial
{
public:
    SSerial();
    ~SSerial();
public:
    void SetBaudRate(int baudrate);
    void SetDataBits(int databits);
    void SetParity(QString parity);
    void SetStopBits(int stopbits);
    void SetFlowControl(QString flowcontrol);
    void SetPortName(QString portname);
    void SetPreFix(QByteArray prefix_bytearray);
public:
    bool OpenSerial();
    void CloseSerial();
    void WriteByteArray(QByteArray data);
    bool IsOpen()
    {
        return serial.isOpen();
    }
private:
    QByteArray m_prefix_bytearray;
    QSerialPort serial;
};

#endif // SSERIAL_H
