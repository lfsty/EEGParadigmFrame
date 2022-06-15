#include "paradigms.h"
#include "ui_paradigms.h"

paradigms::paradigms(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::paradigms)
{
    ui->setupUi(this);

    //设置临时目录地址
    if(m_temp_dir.isValid())
    {
        m_root_path = m_temp_dir.path() + "/";
    }
    else
    {
        exit(-1);
    }

}

paradigms::~paradigms()
{
    delete ui;

    for(auto iter : m_map_static_control_block)
    {
        iter->deleteLater();
    }
    for(auto iter : m_vect_stimulus_control_block)
    {
        iter->deleteLater();
    }

    for(auto iter = m_out_file_map.begin(); iter != m_out_file_map.end(); iter++)
    {
        if(iter.value()->isOpen())
        {
            iter.value()->close();
        }
        iter.value()->deleteLater();
    }


    if(m_sdisplay_wait_for_server != nullptr)
    {
        m_sdisplay_wait_for_server->deleteLater();
    }

    if(m_serial != nullptr)
    {
        delete m_serial;
    }
}

void paradigms::start_paradigms(QString file_path)
{
    if(m_sdisplay_wait_for_server != nullptr)
    {
        m_sdisplay_wait_for_server->hide();
    }

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
    connect(&m_timer_100ms, &QTimer::timeout, this, &paradigms::ontimer);
    m_timer_100ms.start();
}

void paradigms::SetTcpClient(QTcpSocket *tcp_client)
{
    m_tcp_client = tcp_client;

    m_sdisplay_wait_for_server = new SDisplay(SDisplay::DisPlayType::Text, this);

    connect(m_tcp_client, &QTcpSocket::readyRead, this, &paradigms::onTcpReadData);


    showFullScreen();

    //设置内容
    m_sdisplay_wait_for_server->setText("服务器已连接，等待服务器下发范式");

    //设置定位
    m_sdisplay_wait_for_server->setGeometry(rect());
    m_sdisplay_wait_for_server->setAlignment(Qt::AlignCenter);
    //设置字体格式
    QFont ft;
    ft.setPointSize(20);
    ft.setBold(true);
    m_sdisplay_wait_for_server->setFont(ft);

    m_sdisplay_wait_for_server->show();

}

void paradigms::keyPressEvent(QKeyEvent *event)
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

void paradigms::resizeEvent(QResizeEvent *event)
{
    //窗口大小发生变化，重新设置显示位置
    //注：当前没有规划固定位置，全部都为居中显示。
    for(auto iter : m_map_static_control_block)
    {
        iter->ReSize(rect());
    }
    for(auto iter : m_vect_stimulus_control_block)
    {
        iter->ReSize(rect());
    }
    if(m_sdisplay_wait_for_server != nullptr)
    {
        m_sdisplay_wait_for_server->setGeometry(rect());
    }
//    qDebug() << "resize";
}

bool paradigms::ParseJson(QJsonObject json_object)
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
                    m_serial = new SSerial();
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

SControl *paradigms::GenControl(QJsonObject json_obj)
{
    SControl *p_scontrol = new SControl(this);
    p_scontrol->setGeometry(rect());
    if(p_scontrol == nullptr)
    {
        return nullptr;
    }

    //Control
    if(json_obj["Control"].isObject())
    {
        QJsonObject control_obj = json_obj["Control"].toObject();
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
    if(json_obj["Display"].isArray())
    {
        QJsonArray display_array = json_obj["Display"].toArray();
        for(auto iter : display_array)
        {
            if(iter.isObject())
            {
                QJsonObject display_obj = iter.toObject();
                if(display_obj["DisplayType"].toString() == "TextDisplay")
                {
                    //文本块
                    SDisplay *p_sdisplay = new SDisplay(SDisplay::DisPlayType::Text, p_scontrol);
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
                    SDisplay *p_sdisplay = new SDisplay(SDisplay::DisPlayType::Image, p_scontrol);
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

void paradigms::setNextBlock()
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

void paradigms::ontimer()
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

void paradigms::onTcpReadData()
{
    static QByteArray  tcp_buffer;

    tcp_buffer.append(m_tcp_client->readAll());

    while(tcp_buffer.size() >= sizeof(msg_base))
    {
        msg_base _msg_base;
        memcpy(&_msg_base, tcp_buffer, sizeof(msg_base));

        switch(_msg_base.m_type)
        {
            case info_type::file_begin:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_begin))
                    {
                        return;
                    }
                    msg_file_begin _msg_file_begin;
                    memcpy(&_msg_file_begin, tcp_buffer.data(), sizeof(msg_file_begin));

                    QFile *out_file = new QFile(m_root_path + QString(_msg_file_begin.name));
                    if(!out_file->open(QIODevice::WriteOnly))
                    {
                        return;
                    }

                    m_out_file_map[_msg_file_begin.name] = out_file;

                    tcp_buffer.remove(0, sizeof(msg_file_begin));
                    break;
                }
            case info_type::file_data:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_data))
                    {
                        return;
                    }
                    msg_file_data _msg_file_data;
                    memcpy(&_msg_file_data, tcp_buffer.data(), sizeof(msg_file_data));
                    tcp_buffer.remove(0, sizeof(msg_file_data));
                    m_out_file_map[_msg_file_data.name]->write(_msg_file_data.data, _msg_file_data.size);
                    break;
                }
            case info_type::file_end:
                {
                    if(tcp_buffer.size() < sizeof(msg_file_end))
                    {
                        return;
                    }
                    msg_file_end _msg_file_end;
                    memcpy(reinterpret_cast<char *>(&_msg_file_end), tcp_buffer, sizeof(msg_file_end));
                    tcp_buffer.remove(0, sizeof(msg_file_end));

                    m_out_file_map[_msg_file_end.name]->close();

                    QString file_path = m_root_path + QString(_msg_file_end.name);
                    start_paradigms(file_path);

                    break;
                }
        }

    }
}
