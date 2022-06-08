#ifndef PARADIGMOBJ_H
#define PARADIGMOBJ_H

#include<QRect>
#include<QLabel>
#include<QWidget>
#include<QPixmap>
#include<string>
#include<QString>
#include<QFont>
#include<vector>
#include<unordered_map>
#include <QByteArray>
#include "sserial.h"
class SDisplay
{
public:
    SDisplay(QWidget *parent);
    enum class DisPlayType
    {
        Text,
        Image
    };
    void SetDisPlayType(DisPlayType type);
    DisPlayType GetDisPlayType();
private:
    QRect m_pos_rect;
    DisPlayType m_type;
protected:
    QLabel m_dis_label;
public:
    //设置显示位置
    void SetPosition(const QRect &rect);
    void SetPosition(const int &x, const int &y, const int &width, const int &height);
    QRect GetPosition();
    //设置图片
    void SetPixMap(const QPixmap &pix);

    //设置文字
    void SetText(const std::string &str);
    void SetText(const QString &str);
    //设置文字居中，可把位置设定为全屏幕
    void SetTextCenter();
    //设置文字格式
    void SetFont(const QFont &ft);
public:
    //设置是否显示
    void show();
    void hide();
};

class SControl
{
public:
    SControl(QWidget *parent);
    ~SControl();
public:
    void ShowBlocks();
    void HideBlocks();
private:
    QWidget *m_parent = nullptr;
    std::vector<SDisplay *> m_display_block_vector;
    //按键事件
    std::unordered_map<int, std::string> m_keypress_event;
    //持续时间,100毫秒为单位，-1为永不过期
    int m_duration_100ms = -1;
public:
    void ReSize(QRect window_rect);
    //添加显示区块
    void AddDisplayBlock(SDisplay *block);
    //添加按键及相应事件
    void AddKeyPressEvent(int key_code, std::string event_name);
    //获取事件
    std::string GetEvent(int key_code);
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

};

#endif // PARADIGMOBJ_H

