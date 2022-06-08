#ifndef SETUPDLG_H
#define SETUPDLG_H

#include <QWidget>
#include <QTcpSocket>
#include "paradigms.h"

namespace Ui {
class SetupDlg;
}

class SetupDlg : public QWidget
{
    Q_OBJECT

public:
    explicit SetupDlg(QWidget *parent = nullptr);
    ~SetupDlg();

private slots:
    void on_m_btn_tcp_clicked();
    void on_m_btn_local_clicked();
    void on_m_btn_back_index();

    void on_m_btn_choose_local_file_clicked();

    void open_paradigm(QString file_path);
    void on_m_local_btn_ok_clicked();

    void on_m_tcp_btn_ok_clicked();
private:
    QTcpSocket* m_tcp_client = nullptr;
    paradigms *m_paradigm_window = nullptr;
private:
    Ui::SetupDlg *ui;
};

#endif // SETUPDLG_H
