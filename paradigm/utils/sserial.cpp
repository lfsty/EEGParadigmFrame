#include "sserial.h"
#include <QMessageBox>
SSerial::SSerial()
{

}

SSerial::~SSerial()
{
    CloseSerial();
}

void SSerial::SetBaudRate(int baudrate)
{
    switch(baudrate)
    {
        case 1200:
            serial.setBaudRate(QSerialPort::Baud1200);
            break;
        case 2400:
            serial.setBaudRate(QSerialPort::Baud2400);
            break;
        case 4800:
            serial.setBaudRate(QSerialPort::Baud4800);
            break;
        case 9600:
            serial.setBaudRate(QSerialPort::Baud9600);
            break;
        case 19200:
            serial.setBaudRate(QSerialPort::Baud19200);
            break;
        case 38400:
            serial.setBaudRate(QSerialPort::Baud38400);
            break;
        case 57600:
            serial.setBaudRate(QSerialPort::Baud57600);
            break;
        case 1115200:
            serial.setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            serial.setBaudRate(QSerialPort::Baud115200);
            break;
    }
}

void SSerial::SetDataBits(int databits)
{
    switch(databits)
    {
        case 5:
            serial.setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serial.setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serial.setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serial.setDataBits(QSerialPort::Data8);
            break;
        default:
            serial.setDataBits(QSerialPort::Data8);
            break;
    }

}

void SSerial::SetParity(QString parity)
{
    if(parity == "No")
    {
        serial.setParity(QSerialPort::NoParity);
    }
    else if(parity == "Even")
    {
        serial.setParity(QSerialPort::EvenParity);
    }
    else if(parity == "Odd")
    {
        serial.setParity(QSerialPort::OddParity);
    }
    else if(parity == "Mark")
    {
        serial.setParity(QSerialPort::MarkParity);
    }
    else if(parity == "Space")
    {
        serial.setParity(QSerialPort::SpaceParity);
    }
    else
    {
        serial.setParity(QSerialPort::NoParity);
    }
}

void SSerial::SetStopBits(int stopbits)
{
    switch(stopbits)
    {
        case 1:
            serial.setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serial.setStopBits(QSerialPort::TwoStop);
            break;
        default:
            serial.setStopBits(QSerialPort::OneStop);
            break;
    }
}

void SSerial::SetFlowControl(QString flowcontrol)
{
    if(flowcontrol == "No")
    {
        serial.setFlowControl(QSerialPort::NoFlowControl);
    }
    else if(flowcontrol == "Hardware")
    {
        serial.setFlowControl(QSerialPort::HardwareControl);
    }
    else if(flowcontrol == "Software")
    {
        serial.setFlowControl(QSerialPort::SoftwareControl);
    }
    else
    {
        serial.setFlowControl(QSerialPort::NoFlowControl);
    }
}

void SSerial::SetPortName(QString portname)
{
    serial.setPortName(portname);
}

void SSerial::SetPreFix(QByteArray prefix_bytearray)
{
    m_prefix_bytearray = prefix_bytearray;
}

bool SSerial::OpenSerial()
{
    if(serial.open(QIODevice::ReadWrite))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SSerial::CloseSerial()
{
    serial.clear();
    serial.close();
    serial.deleteLater();
}

void SSerial::WriteByteArray(QByteArray data)
{
    serial.write(m_prefix_bytearray + data);
}
