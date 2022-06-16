#ifndef PARADIGM_H
#define PARADIGM_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include <QVector>
#include <QMessageBox>
#include <QKeyEvent>
#include <random>
#include <QFileInfo>

#include "sserial.h"
#include "sjson_qt.h"
#include "unzip.h"
#include "data_info.h"
#include "utils.h"
#include "scontrol.h"
#include "sdisplay.h"
class Paradigm : public QDialog
{
    Q_OBJECT
public:
    explicit Paradigm(QWidget *parent = nullptr);
    ~Paradigm();
    void Start(QString file_path);
    //设置临时文件存放目录
    void SetRootPath(QString root_path);

private:
    SSerial *m_serial = nullptr;
    QTimer m_timer_100ms;
private:
    bool ParseJson(QJsonObject json_object);
    SControl *GenControl(QJsonObject json_object);
    void setNextBlock();
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

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
private slots:
    void ontimer();

signals:

};

#endif // PARADIGM_H
