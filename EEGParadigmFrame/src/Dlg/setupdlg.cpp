#include "setupdlg.h"
#include "ui_setupdlg.h"

SetupDlg::SetupDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupDlg)
{
    ui->setupUi(this);
    if(!m_temp_dir.isValid())
    {
        QMessageBox::critical(this, "error", "临时目录获取失败!");
        exit(-3);
    }
    ui->m_line_server_port->setValidator(new QIntValidator(ui->m_line_server_port));

    connect(ui->m_btn_tcp_back, &QPushButton::clicked, this, &SetupDlg::on_m_btn_back_index);
    connect(ui->m_btn_local_back, &QPushButton::clicked, this, &SetupDlg::on_m_btn_back_index);

    this->setWindowTitle("打开方式");
}

SetupDlg::~SetupDlg()
{
    delete ui;
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
    Paradigm *w = new Paradigm(this);
    w->SetRootPath(m_temp_dir.path());
    this->hide();
    w->show();
    w->Start(file_path);
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
    if(m_tcp_client == nullptr)
    {
        m_tcp_client = new QTcpSocket(this);
    }

    m_tcp_client->connectToHost(server_ip, server_port);
    bool is_connected = m_tcp_client->waitForConnected();//等待直到连接成功

    if(is_connected)
    {
        WaitForFileDlg w(this);

        w.SetRootPath(m_temp_dir.path());
        w.SetTcpClient(m_tcp_client);
        w.show();
        w.showFullScreen();
        w.exec();

        open_paradigm(w.GetFilePath());
    }
    else
    {
        QMessageBox::information(NULL, "提示", "服务器连接失败", QMessageBox::Yes, QMessageBox::Yes);
    }

}
