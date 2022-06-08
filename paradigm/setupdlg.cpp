#include "setupdlg.h"
#include "ui_setupdlg.h"
#include <QFileDialog>
#include <QIntValidator>
#include <QDebug>
#include <QMessageBox>
SetupDlg::SetupDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupDlg)
{
    ui->setupUi(this);
    ui->m_line_server_port->setValidator(new QIntValidator(ui->m_line_server_port));

    connect(ui->m_btn_tcp_back, &QPushButton::clicked, this, &SetupDlg::on_m_btn_back_index);
    connect(ui->m_btn_local_back, &QPushButton::clicked, this, &SetupDlg::on_m_btn_back_index);

    m_tcp_client = new QTcpSocket();

    m_paradigm_window = new paradigms();

    this->setWindowTitle("打开方式");
}

SetupDlg::~SetupDlg()
{
    delete ui;
    delete m_tcp_client;
    delete m_paradigm_window;
}

void SetupDlg::on_m_btn_tcp_clicked()
{
    ui->m_stack->setCurrentWidget(ui->m_file_way_tcp);
}

void SetupDlg::on_m_btn_local_clicked()
{
    ui->m_stack->setCurrentWidget(ui->m_file_way_local);
}

void SetupDlg::on_m_btn_back_index()
{
    ui->m_stack->setCurrentWidget(ui->m_choose_file_way);
}

void SetupDlg::on_m_btn_choose_local_file_clicked()
{
//    ui->m_line_choose_local_file_path->setText("test");
    QString filename = QFileDialog::getOpenFileName(
                           this,
                           "open a file",
                           "./",
                           "压缩文件(*.zip *7z);;All files(*.*)"
                       );
    ui->m_line_choose_local_file_path->setText(filename);


}

void SetupDlg::open_paradigm(QString file_path)
{

//    w->setAttribute(Qt::WA_DeleteOnClose);
    m_paradigm_window->start_paradigms(file_path);
    m_paradigm_window->show();
    this->hide();
}

void SetupDlg::on_m_local_btn_ok_clicked()
{
    open_paradigm(ui->m_line_choose_local_file_path->text());
}

void SetupDlg::on_m_tcp_btn_ok_clicked()
{
    QString server_ip = ui->m_line_server_ip->text();
    int server_port = ui->m_line_server_port->text().toInt();

    //连接服务器
    m_tcp_client->connectToHost(server_ip, server_port);
    bool is_connected = m_tcp_client->waitForConnected();//等待直到连接成功

    if(is_connected)
    {
        m_paradigm_window->SetTcpClient(m_tcp_client);
        m_paradigm_window->show();
        this->hide();
    }
    else
    {
        QMessageBox::information(NULL, "提示", "服务器连接失败", QMessageBox::Yes, QMessageBox::Yes);
    }

}
