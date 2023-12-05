#include "paradigm.h"

#include <QDebug>

Paradigm::Paradigm(QWidget *parent)
    : QDialog{parent}
{
    m_timer_100ms.setTimerType(Qt::PreciseTimer);
}

Paradigm::~Paradigm()
{

}

void Paradigm::Start(QString file_path)
{
    unzip::UnZip(m_root_path, file_path);

    SJson_Qt sjson(m_root_path + "./paradigm.json");
    if(!sjson.ReadFromFile())
    {
        QMessageBox::critical(this, tr("Error"), sjson.GetLastError());
        exit(-1);
    }

    QJsonDocument json_root_doc = sjson.GetJsonDoc();


    bool res = ParseJson(json_root_doc.object());
    if(res != true)
    {
        exit(-3);
    }


    m_timer_100ms.setInterval(100);
    connect(&m_timer_100ms, &QTimer::timeout, this, &Paradigm::ontimer);
    m_timer_100ms.start();
}

void Paradigm::SetRootPath(QString root_path)
{
    m_root_path = root_path;
}

bool Paradigm::ParseJson(QJsonObject json_object)
{
    //全屏
    if(json_object["DialogSize"].toObject()["Type"].toString() == "full_screen")
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }

    //打标信息
    if(json_object["MarkerInfo"].isArray())
    {
        QJsonArray mark_info_array = json_object["MarkerInfo"].toArray();
        for(auto iter : mark_info_array)
        {
            if(iter.isObject())
            {
                QJsonObject mark_info_obj = iter.toObject();
                if(mark_info_obj["Type"].toString() == "COM")
                {
                    m_serial = new SSerial(this);
                    m_serial->SetPortName(mark_info_obj["PortName"].toString());
                    m_serial->SetBaudRate(mark_info_obj["BaudRate"].toInt());
                    m_serial->SetDataBits(mark_info_obj["DataBits"].toInt());
                    m_serial->SetParity(mark_info_obj["Parity"].toString());
                    m_serial->SetStopBits(mark_info_obj["StopBits"].toInt());
                    m_serial->SetFlowControl(mark_info_obj["FlowControl"].toString());
                    m_serial->SetPreFix(hexStr_To_Hexchar(mark_info_obj["PreFix"].toString()));
                    if(!m_serial->OpenSerial())
                    {
                        QMessageBox::critical(this, tr("Error"), "打开串口出错");
                    }
                }
            }
        }
    }

    //Instructions
    if(json_object["Instructions"].isObject())
    {
        SControl *tmp = GenControl(json_object["Instructions"].toObject());
        if(tmp == nullptr)
        {
            QMessageBox::critical(this, tr("Error"), "Instructions模块解析出错");
        }
        else
        {
            m_map_static_control_block["instructions"] = tmp;
            m_vect_paradigm_order.push_back("instructions");
        }
    }

    //TrailList
    if(json_object["TrailList"].isObject())
    {
        QJsonObject taillist_obj = json_object["TrailList"].toObject();

        m_round = taillist_obj["Round"].toInt();
        m_loop_times = taillist_obj["LoopTimes"].toInt();
        m_loop_mode = taillist_obj["LoopMode"].toString();


        //TrailList_BetweenRound
        if(taillist_obj["BetweenRound"].isObject())
        {
            SControl *tmp = GenControl(taillist_obj["BetweenRound"].toObject());
            if(tmp == nullptr)
            {
                QMessageBox::critical(this, tr("Error"), "BetweenRound模块解析出错");
            }
            else
            {
                m_map_static_control_block["betweenround"] = tmp;
            }
        }

        //TrailList_TrailProc
        if(taillist_obj["TrailProc"].isObject())
        {
            //TrailList_TrailProc_StaticBlock
            QJsonObject trailProc_obj = taillist_obj["TrailProc"].toObject();
            if(trailProc_obj["StaticBlock"].isObject())
            {
                QJsonObject staticBlock_obj = trailProc_obj["StaticBlock"].toObject();
                for(auto iter = staticBlock_obj.begin(); iter != staticBlock_obj.end(); iter++)
                {
                    SControl *tmp = GenControl(iter.value().toObject());
                    if(tmp == nullptr)
                    {
                        QMessageBox::critical(this, tr("Error"), "StaticBlock模块解析出错:" + iter.key());
                    }
                    else
                    {
                        m_map_static_control_block[iter.key()] = tmp;
                    }
                }
            }

            //TrailList_TrailProc_StimulusBlock
            if(trailProc_obj["StimulusBlock"].isArray())
            {
                QJsonArray stimulusBlock_array = trailProc_obj["StimulusBlock"].toArray();
                for(auto iter : stimulusBlock_array)
                {
                    if(iter.isObject())
                    {
                        SControl *tmp = GenControl(iter.toObject());
                        if(tmp == nullptr)
                        {
                            QMessageBox::critical(this, tr("Error"), "StimulusBlock模块解析出错");
                        }
                        else
                        {
                            m_vect_stimulus_control_block.push_back(tmp);
                        }
                    }
                }
            }

            //TrailList_TrailProc_Trail
            if(trailProc_obj["Trail"].isArray())
            {
                QJsonArray trail_array = trailProc_obj["Trail"].toArray();
                QVector<QString> tmp_order_trail;

                for(auto iter : trail_array)
                {
                    QJsonObject single_trail_obj = iter.toObject();
                    if(single_trail_obj["Type"].toString() == "StaticBlock")
                    {
                        tmp_order_trail.push_back(single_trail_obj["Data"].toString());
                    }
                    else if(single_trail_obj["Type"].toString() == "StimulusBlock")
                    {
                        tmp_order_trail.push_back("stimulus");
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Error"), "unknown trail type");
                    }
                }

                for(int i = 0; i < m_round; i++)
                {
                    for(int j = 0; j < m_loop_times; j++)
                    {
                        m_vect_paradigm_order.append(tmp_order_trail);
                    }
                    if(i < m_round - 1)
                    {
                        m_vect_paradigm_order.push_back("betweenround");
                    }
                }
            }
        }
    }

    //GoodBye
    if(json_object["GoodBye"].isObject())
    {
        SControl *tmp = GenControl(json_object["GoodBye"].toObject());
        if(tmp == nullptr)
        {
            QMessageBox::critical(this, tr("Error"), "GoodBye模块解析出错");
        }
        else
        {
            m_map_static_control_block["goodbye"] = tmp;
            m_vect_paradigm_order.push_back("goodbye");
        }
    }

    //刺激顺序生成
    for(int i = 0; i < m_round * m_loop_times; i++)
    {
        m_vect_stimulus_order.push_back(i % m_vect_stimulus_control_block.size());
    }
    if(m_loop_mode == "random")
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(m_vect_stimulus_order.begin(), m_vect_stimulus_order.end(), std::default_random_engine(seed));
    }

    return true;
}

SControl *Paradigm::GenControl(QJsonObject json_object)
{
    SControl *p_scontrol = new SControl(this);
    //p_scontrol->setGeometry(rect());
    if(p_scontrol == nullptr)
    {
        return nullptr;
    }

    //Control
    if(json_object["Control"].isObject())
    {
        QJsonObject control_obj = json_object["Control"].toObject();
        //Control_Duration
        p_scontrol->SetDuration(control_obj["Duration"].toInt());
        //Control_Action
        if(control_obj["Action"].isObject())
        {
            QJsonObject action_obj = control_obj["Action"].toObject();
            //Control_Action_key
            if(action_obj["Key"].isObject())
            {
                QJsonObject key_obj = action_obj["Key"].toObject();
                for(auto iter = key_obj.begin(); iter != key_obj.end(); iter++)
                {
                    p_scontrol->AddKeyPressEvent(iter.key().toInt(), iter.value().toString());
                }
            }

            //TimeOut
            if(action_obj["TimeOut"].isString())
            {
                //记超时为特殊的按键事件，为-1
                p_scontrol->AddKeyPressEvent(-1, action_obj["TimeOut"].toString());
            }
        }
        //Control_Marker
        if(control_obj["Marker"].isArray())
        {
            QJsonArray marker_array = control_obj["Marker"].toArray();
            for(auto iter : marker_array)
            {
                if(iter.isObject())
                {
                    QJsonObject marker_obj = iter.toObject();
                    if(marker_obj["Type"].toString() == "COM")
                    {
                        QString mark_data_str = marker_obj["Data"].toString();
                        p_scontrol->SetMarkData(hexStr_To_Hexchar(mark_data_str));
                    }
                }
            }
        }
    }
    else
    {
        p_scontrol->deleteLater();
        return nullptr;
    }

    //Display
    if(json_object["Display"].isArray())
    {
        QJsonArray display_array = json_object["Display"].toArray();
        for(auto iter : display_array)
        {
            if(iter.isObject())
            {
                QJsonObject display_obj = iter.toObject();
                if(display_obj["DisplayType"].toString() == "TextDisplay")
                {
                    //文本块
                    SDisplay *p_sdisplay = new SDisplay(SDisplay::DisPlayType::Text, this);
                    //设置内容
                    p_sdisplay->setText(display_obj["TextData"].toString());

                    //设置定位
                    p_sdisplay->setGeometry(rect());
                    if(display_obj["Position"].toObject()["PosMode"].toString() == "center")
                    {
                        p_sdisplay->setAlignment(Qt::AlignCenter);
                    }
                    else
                    {
                        //固定定位还没有写！
                    }

                    //设置字体格式
                    QFont ft;
                    ft.setPointSize(display_obj["TextFont"].toInt());
                    ft.setBold(display_obj["isBold"].toBool());
                    p_sdisplay->setFont(ft);

                    p_scontrol->AddDisplayBlock(p_sdisplay);
                }
                else if(display_obj["DisplayType"].toString() == "ImageDisplay")
                {
                    //图片块
                    SDisplay *p_sdisplay = new SDisplay(SDisplay::DisPlayType::Image, this);
                    //读取图片
                    QString img_path = display_obj["ImagePath"].toString();
                    img_path = m_root_path + img_path;

                    QFileInfo file(img_path);
                    if(!file.exists())
                    {
                        QMessageBox::critical(this, tr("Error"), "文件不存在：" + img_path);
                        p_sdisplay->deleteLater();
                        continue;
                    }

                    QPixmap pix = QPixmap(img_path);

                    //图片缩放
                    if(display_obj["Size"].isObject())
                    {
                        QJsonObject size_obj = display_obj["Size"].toObject();
                        int height = size_obj["Height"].toInt();
                        int width = size_obj["Width"].toInt();
                        pix = pix.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    }
                    //图片定位
                    QRect pix_rect = pix.rect();
                    QRect pix_loc;
                    if(display_obj["Position"].toObject()["PosMode"].toString() == "center")
                    {
                        pix_loc = QRect((width() - pix_rect.width()) / 2, (height() - pix_rect.height()) / 2, pix_rect.width(), pix_rect.height());
                    }
                    else
                    {
                        //固定定位还没有写！
                    }

                    p_sdisplay->setPixmap(pix);

                    p_sdisplay->setGeometry(pix_loc);

                    p_scontrol->AddDisplayBlock(p_sdisplay);
                }
                else if(display_obj["DisplayType"].toString() == "Nothing")
                {

                    //nothing 块

                }
            }
        }
    }
    else
    {
        p_scontrol->deleteLater();
        return nullptr;
    }

    return p_scontrol;
}

void Paradigm::setNextBlock()
{
    m_currnet_timer_count_100ms = 0;
    QString current_block_name = m_vect_paradigm_order[m_current_paradigm_order_step++];
    SControl *last_control = m_current_control_block;
    if(current_block_name != "stimulus")
    {
        m_current_control_block = m_map_static_control_block[current_block_name];
    }
    else
    {
        m_current_control_block = m_vect_stimulus_control_block[m_vect_stimulus_order[m_current_stimulus_order_step++]];
    }
    last_control->HideBlocks();
    m_current_control_block->ShowBlocks();

    //打标
    if(m_serial != nullptr)
    {
        m_current_control_block->COMMark(m_serial);
    }

}

void Paradigm::keyPressEvent(QKeyEvent *event)
{
    if(m_current_control_block != nullptr)
    {
        QString key_event = m_current_control_block->GetEvent(event->key());
        if(key_event == "next")
        {
            setNextBlock();
        }
    }

    if(event->key() == Qt::Key_Escape)
    {
        showNormal();
    }
}

void Paradigm::resizeEvent(QResizeEvent *event)
{
    //窗口大小发生变化，重新设置显示位置
    //注：当前没有规划固定位置，全部都为居中显示。
    foreach(auto iter, m_map_static_control_block)
    {
        iter->ReSize(rect());
    }
    for(auto iter : m_vect_stimulus_control_block)
    {
        iter->ReSize(rect());
    }
}

void Paradigm::ontimer()
{
    if(m_current_paradigm_order_step < m_vect_paradigm_order.size())
    {
        //第一次
        if(m_current_control_block == nullptr)
        {
            QString block_control_name = m_vect_paradigm_order[m_current_paradigm_order_step++];
            if(block_control_name != "stimulus")
            {
                m_current_control_block = m_map_static_control_block[block_control_name];
            }
            else
            {
                m_current_control_block = m_vect_stimulus_control_block[m_vect_stimulus_order[m_current_stimulus_order_step++]];
            }
            m_current_control_block->ShowBlocks();
            return;
        }

        //判断是否有持续时间
        if(m_current_control_block->GetDuration() == -1)
        {
            return;
        }
        else
        {
            m_currnet_timer_count_100ms++;
        }

        if(m_currnet_timer_count_100ms >= m_current_control_block->GetDuration())
        {

            //当前应全为next
            if(m_current_control_block->GetEvent(-1) != "next")
            {
                qDebug() << "timeout next 不为next";
            }
            setNextBlock();
        }

    }
    else
    {
        m_timer_100ms.stop();
    }
}
