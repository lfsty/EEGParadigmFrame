#include "paradigmobj.h"

#include <QDebug>
SDisplay::SDisplay(QWidget *parent)
{
    m_dis_label.setParent(parent);
    hide();
}

void SDisplay::SetDisPlayType(DisPlayType type)
{
    m_type = type;
}

SDisplay::DisPlayType SDisplay::GetDisPlayType()
{
    return m_type;
}

void SDisplay::SetPosition(const QRect &rect)
{
    m_pos_rect = rect;
    m_dis_label.setGeometry(m_pos_rect);
}

void SDisplay::SetPosition(const int &x, const int &y, const int &width, const int &height)
{
    m_pos_rect = QRect(x, y, width, height);
    m_dis_label.setGeometry(m_pos_rect);
}

QRect SDisplay::GetPosition()
{
    return m_pos_rect;
}

void SDisplay::show()
{
    m_dis_label.show();
}

void SDisplay::hide()
{
    m_dis_label.hide();
}

void SDisplay::SetPixMap(const QPixmap &pix)
{
    m_dis_label.setPixmap(pix);
}

void SDisplay::SetText(const std::string &str)
{
    m_dis_label.setText(QString::fromStdString(str));
}

void SDisplay::SetText(const QString &str)
{
    m_dis_label.setText(str);
}

void SDisplay::SetTextCenter()
{
    m_dis_label.setAlignment(Qt::AlignCenter);
}

void SDisplay::SetFont(const QFont &ft)
{
    m_dis_label.setFont(ft);
}

SControl::SControl(QWidget *parent)
{
    m_parent = parent;
}

SControl::~SControl()
{
    for(auto iter : m_display_block_vector)
    {
        delete iter;
    }
}

void SControl::ShowBlocks()
{
    for(auto iter : m_display_block_vector)
    {
        iter->show();
    }
}

void SControl::HideBlocks()
{
    for(auto iter : m_display_block_vector)
    {
        iter->hide();
    }
}

void SControl::ReSize(QRect window_rect)
{
    for(auto iter = m_display_block_vector.begin(); iter != m_display_block_vector.end(); iter++)
    {
        switch((*iter)->GetDisPlayType())
        {
            case SDisplay::DisPlayType::Text:
                (*iter)->SetPosition(window_rect);
                break;
            case SDisplay::DisPlayType::Image:
                QRect pix_rect = (*iter)->GetPosition();
                QRect pix_loc;
                pix_loc = QRect((window_rect.width() - pix_rect.width()) / 2, (window_rect.height() - pix_rect.height()) / 2, pix_rect.width(), pix_rect.height());
                (*iter)->SetPosition(pix_loc);
                break;
        }
    }
}

void SControl::AddDisplayBlock(SDisplay *block)
{
    m_display_block_vector.push_back(block);
}

void SControl::AddKeyPressEvent(int key_code, std::string event_name)
{
    m_keypress_event[key_code] = event_name;
}

std::string SControl::GetEvent(int key_code)
{
    auto find_iter = m_keypress_event.find(key_code);
    if(find_iter != m_keypress_event.end())
    {
        return find_iter->second;
    }
    else
    {
        return "None";
    }
}

void SControl::SetDuration(int time_100ms)
{
    m_duration_100ms = time_100ms;
}

int SControl::GetDuration()
{
    return m_duration_100ms;
}

void SControl::COMMark(SSerial *serial)
{
    if(serial != nullptr && serial->IsOpen() && !m_mark_data.isEmpty())
    {
        serial->WriteByteArray(m_mark_data);
    }
}

void SControl::SetMarkData(QByteArray mark_data)
{
    m_mark_data = mark_data;
}
