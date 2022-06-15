#ifndef PARADIGMS_H
#define PARADIGMS_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTemporaryDir>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include <QFile>
#include <QVector>
#include <QMessageBox>
#include <QKeyEvent>
#include <random>

#include "sserial.h"
#include "sjson_qt.h"
#include "unzip.h"
#include "data_info.h"
#include "utils.h"
#include "scontrol.h"
#include "sdisplay.h"

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
    QMap<QString, QFile *> m_out_file_map;
    SSerial *m_serial = nullptr;
public:
    void start_paradigms(QString file_path);
    void SetTcpClient(QTcpSocket *tcp_client);
private:
    //静态Control块
    QMap<QString, SControl *> m_map_static_control_block;
    //刺激Control块
    QVector<SControl *> m_vect_stimulus_control_block;
    //顺序执行范式块的名称,总的范式执行顺序，根据m_current_paradigm_order_step顺序索引
    //若返回QString为stimulus，则根据m_current_stimulus_order_step去m_vect_stimulus_order索引
    //              再从m_vect_stimulus_order得到的索引去m_vect_stimulus_control_block获取刺激块
    //若返回QString非stimulus，则直接根据QString去m_map_static_control_block取得control块
    QVector<QString> m_vect_paradigm_order;
    //刺激块的顺序
    QVector<int> m_vect_stimulus_order;

    int m_current_stimulus_order_step = 0;
    int m_current_paradigm_order_step = 0;
    SControl *m_current_control_block = nullptr;
    int m_currnet_timer_count_100ms = 0;

    QString m_root_path;
    int m_round = 0;
    int m_loop_times = 0;
    QString m_loop_mode = "order";
private:
    QTemporaryDir m_temp_dir;
protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    bool ParseJson(QJsonObject json_object);
    SControl *GenControl(QJsonObject json_object);
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
