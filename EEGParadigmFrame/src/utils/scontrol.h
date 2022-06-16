#ifndef SCONTROL_H
#define SCONTROL_H

#include <QWidget>
#include "sdisplay.h"
#include "sserial.h"

class SControl : public QWidget
{
    Q_OBJECT
public:
    explicit SControl(QWidget *parent = nullptr);
    ~SControl();
public:
    void ShowBlocks();
    void HideBlocks();
private:
    QList<SDisplay *> m_display_block_list;
    //按键事件
    QMap<int, QString> m_keypress_event_map;
    //持续时间,100毫秒为单位，-1为永不过期
    int m_duration_100ms = -1;
public:
    void ReSize(QRect window_rect);
    //添加显示区块
    void AddDisplayBlock(SDisplay *block);
    //添加按键及相应事件
    void AddKeyPressEvent(int key_code, QString event_name);
    //获取事件
    QString GetEvent(int key_code);
    //设置持续时间
    void SetDuration(int time_100ms);
    //获取持续时间
    int GetDuration();
public:
    //打标
    void COMMark(SSerial *serial);
    void SetMarkData(QByteArray mark_data);
private:
    //打标信息
    QByteArray m_mark_data;
signals:

};

#endif // SCONTROL_H
