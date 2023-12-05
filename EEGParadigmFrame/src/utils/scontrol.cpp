#include "scontrol.h"

SControl::SControl(QWidget *parent)
    : QObject{parent}
{

}

SControl::~SControl()
{
    for(auto disp_block : m_display_block_list)
    {
        disp_block->deleteLater();
    }
}

void SControl::ShowBlocks()
{
    for(auto disp_block : m_display_block_list)
    {
        disp_block->show();
    }
//    show();
}

void SControl::HideBlocks()
{
    for(auto disp_block : m_display_block_list)
    {
        disp_block->hide();
    }
    //hide();
}

void SControl::ReSize(QRect window_rect)
{
    //setGeometry(rect());
    for(auto disp_block : m_display_block_list)
    {
        switch(disp_block->GetDisPlayType())
        {
            case SDisplay::DisPlayType::Text:
                disp_block->setGeometry(window_rect);
                break;
            case SDisplay::DisPlayType::Image:
                QRect pix_rect = disp_block->rect();
                QRect pix_loc;
                pix_loc = QRect((window_rect.width() - pix_rect.width()) / 2, (window_rect.height() - pix_rect.height()) / 2, pix_rect.width(), pix_rect.height());
                disp_block->setGeometry(pix_loc);
                break;
        }
    }
}

void SControl::AddDisplayBlock(SDisplay *block)
{
    m_display_block_list.push_back(block);
}

void SControl::AddKeyPressEvent(int key_code, QString event_name)
{
    m_keypress_event_map[key_code] = event_name;
}

QString SControl::GetEvent(int key_code)
{
    auto find_iter = m_keypress_event_map.find(key_code);
    if(find_iter != m_keypress_event_map.end())
    {
        return find_iter.value();
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
