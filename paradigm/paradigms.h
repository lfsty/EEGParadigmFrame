#ifndef PARADIGMS_H
#define PARADIGMS_H

#include <iostream>
#include <fstream>

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTemporaryDir>
#include <QTcpSocket>

#include "paradigmobj.h"
#include "configor/json.hpp"
#include "sserial.h"


QT_BEGIN_NAMESPACE
namespace Ui
{
    class paradigms;
}
QT_END_NAMESPACE

class paradigms : public QMainWindow
{
    Q_OBJECT

public:
    paradigms(QWidget *parent = nullptr);
    ~paradigms();
private:
    QTcpSocket *m_tcp_client = nullptr;
    SDisplay *m_sdisplay_wait_for_server = nullptr;
    std::unordered_map<std::string, std::ofstream *> m_out_file_map;
    std::vector<std::string> m_exist_file_name_vect;
    SSerial *m_serial = nullptr;
public:
    void start_paradigms(QString file_path);
    void SetTcpClient(QTcpSocket *tcp_client);
private:
    std::unordered_map<std::string, SControl *> m_control_block_map;
    std::vector<SControl *> m_stimulus_block_vect;
    std::vector<std::string> m_paradigm_order_vect;

    std::vector<int> m_stimulus_order_vect;
    int m_current_stimulus_order_step = 0;
    int m_current_paradigm_order_step = 0;
    SControl *m_current_control_block = nullptr;
    int m_currnet_timer_count_100ms = 0;

    std::string m_root_path;
    int m_round = 0;
    int m_loop_times = 0;
    std::string m_loop_mode = "order";
private:
    QTemporaryDir m_temp_dir;
protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    bool ParseJson(configor::json json_data);
    SControl *GenControl(configor::json json_data);
    void setNextBlock();
private:
    QTimer m_timer_100ms;
private slots:
    void ontimer();
    void onTcpReadData();
private:
    Ui::paradigms *ui;
};
#endif // PARADIGMS_H
