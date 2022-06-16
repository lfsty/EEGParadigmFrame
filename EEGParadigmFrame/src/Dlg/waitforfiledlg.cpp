#include "waitforfiledlg.h"

WaitForFileDlg::WaitForFileDlg(QWidget *parent)
    : QDialog{parent}
{
//    setGeometry(QRect(0, 0, 800, 600));
    m_sdisplay_wait_for_server = new SDisplay(SDisplay::DisPlayType::Text, this);
    //设置内容
    m_sdisplay_wait_for_server->setText("服务器已连接，等待服务器下发范式");

    //设置定位
//    m_sdisplay_wait_for_server->setGeometry(rect());
    m_sdisplay_wait_for_server->setAlignment(Qt::AlignCenter);
    //设置字体格式
    QFont ft;
    ft.setPointSize(20);
    ft.setBold(true);
    m_sdisplay_wait_for_server->setFont(ft);
}

WaitForFileDlg::~WaitForFileDlg()
{
    for(auto iter = m_out_file_map.begin(); iter != m_out_file_map.end(); iter++)
    {
        if(iter.value()->isOpen())
        {
            iter.value()->close();
        }
        iter.value()->deleteLater();
    }

}

void WaitForFileDlg::SetTcpClient(QTcpSocket *tcp_client)
{
    m_tcp_client = tcp_client;
    connect(m_tcp_client, &QTcpSocket::readyRead, this, &WaitForFileDlg::onTcpReadData);
}

void WaitForFileDlg::SetRootPath(QString root_path)
{
    m_root_path = root_path;
}

QString WaitForFileDlg::GetFilePath()
{
    return m_file_path;
}

void WaitForFileDlg::onTcpReadData()
{
    static QByteArray  tcp_buffer;

    tcp_buffer.append(m_tcp_client->readAll());

    while(tcp_buffer.size() >= sizeof(msg_base))
    {
        msg_base _msg_base;
        memcpy(&_msg_base, tcp_buffer, sizeof(msg_base));

        switch(_msg_base.m_type)
        {
            case info_type::file_begin:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_begin))
                    {
                        return;
                    }
                    msg_file_begin _msg_file_begin;
                    memcpy(&_msg_file_begin, tcp_buffer.data(), sizeof(msg_file_begin));

                    QFile *out_file = new QFile(m_root_path + QString(_msg_file_begin.name));
                    if(!out_file->open(QIODevice::WriteOnly))
                    {
                        return;
                    }

                    m_out_file_map[_msg_file_begin.name] = out_file;

                    tcp_buffer.remove(0, sizeof(msg_file_begin));
                    break;
                }
            case info_type::file_data:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_data))
                    {
                        return;
                    }
                    msg_file_data _msg_file_data;
                    memcpy(&_msg_file_data, tcp_buffer.data(), sizeof(msg_file_data));
                    tcp_buffer.remove(0, sizeof(msg_file_data));
                    m_out_file_map[_msg_file_data.name]->write(_msg_file_data.data, _msg_file_data.size);
                    break;
                }
            case info_type::file_end:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_end))
                    {
                        return;
                    }
                    msg_file_end _msg_file_end;
                    memcpy(reinterpret_cast<char *>(&_msg_file_end), tcp_buffer, sizeof(msg_file_end));
                    tcp_buffer.remove(0, sizeof(msg_file_end));

                    m_out_file_map[_msg_file_end.name]->close();

                    m_file_path = m_root_path + QString(_msg_file_end.name);

                    close();

                    break;
                }
        }
    }
}

void WaitForFileDlg::resizeEvent(QResizeEvent *event)
{
    if(m_sdisplay_wait_for_server != nullptr)
    {
        m_sdisplay_wait_for_server->setGeometry(rect());
    }
}
