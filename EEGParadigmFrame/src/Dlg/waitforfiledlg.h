#ifndef WAITFORFILEDLG_H
#define WAITFORFILEDLG_H

#include <QDialog>
#include <QTcpSocket>
#include <QFile>
#include <sdisplay.h>
#include <QRect>

#include "data_info.h"

class WaitForFileDlg : public QDialog
{
    Q_OBJECT
public:
    explicit WaitForFileDlg(QWidget *parent = nullptr);
    ~WaitForFileDlg();
    void SetTcpClient(QTcpSocket *tcp_client);
    void SetRootPath(QString root_path);
    QString GetFilePath();
private:
    void onTcpReadData();
    void resizeEvent(QResizeEvent *event);
private:
    QTcpSocket *m_tcp_client = nullptr;
    QMap<QString, QFile *> m_out_file_map;
    SDisplay *m_sdisplay_wait_for_server = nullptr;
    QString m_root_path;
    QString m_file_path;
signals:

};

#endif // WAITFORFILEDLG_H
