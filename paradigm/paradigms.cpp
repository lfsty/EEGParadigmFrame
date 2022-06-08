#include "paradigms.h"
#include "ui_paradigms.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPixmap>
#include <QDir>
#include <QRect>
#include <QFont>
#include <random>
#include <QFileInfo>
#include "unzip.h"
#include "data_info.h"
#include "utils.h"
#include <QMessageBox>

paradigms::paradigms(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::paradigms)
{
    ui->setupUi(this);

    //设置临时目录地址
    if(m_temp_dir.isValid())
    {
        m_root_path = m_temp_dir.path().toStdString() + "/";
        qDebug() << m_root_path.c_str();
    }
    else
    {
        exit(-1);
    }

}

paradigms::~paradigms()
{
    delete ui;

    for(auto iter = m_control_block_map.begin(); iter != m_control_block_map.end(); iter++)
    {
        delete iter->second;
    }
    for(auto iter = m_stimulus_block_vect.begin(); iter != m_stimulus_block_vect.end(); iter++)
    {
        delete *iter;
    }
    for(auto iter = m_out_file_map.begin(); iter != m_out_file_map.end(); iter++)
    {
        if(iter->second->is_open())
        {
            iter->second->close();
        }
        delete iter->second;
    }


    if(m_sdisplay_wait_for_server != nullptr)
    {
        delete m_sdisplay_wait_for_server;
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

    unzip::UnZip(m_root_path.c_str(), file_path);

    configor::json json_data;
    try
    {
        std::ifstream ifs(m_root_path + "./paradigm.json");
        ifs >> json_data;
        ifs.close();
    }
    catch (...)
    {
        qDebug() << "json文件读取出错";
        exit(-2);
    }

    if(!json_data.empty())
    {
        bool res = ParseJson(json_data);
        if(res != true)
        {
            exit(-3);
        }
    }

    m_timer_100ms.setInterval(100);
    connect(&m_timer_100ms, &QTimer::timeout, this, &paradigms::ontimer);
    m_timer_100ms.start();
}

void paradigms::SetTcpClient(QTcpSocket *tcp_client)
{
    m_tcp_client = tcp_client;

    m_sdisplay_wait_for_server = new SDisplay(this);

    connect(m_tcp_client, &QTcpSocket::readyRead, this, &paradigms::onTcpReadData);


    showFullScreen();

    //设置内容
    m_sdisplay_wait_for_server->SetText(QString("服务器已连接，等待服务器下发范式"));

    //设置定位
    m_sdisplay_wait_for_server->SetPosition(rect());
    m_sdisplay_wait_for_server->SetTextCenter();
    //设置字体格式
    QFont ft;
    ft.setPointSize(20);
    ft.setBold(true);
    m_sdisplay_wait_for_server->SetFont(ft);

    m_sdisplay_wait_for_server->show();

}

void paradigms::keyPressEvent(QKeyEvent *event)
{
    if(m_current_control_block != nullptr)
    {
        std::string key_event = m_current_control_block->GetEvent(event->key());
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
    for(auto iter = m_control_block_map.begin(); iter != m_control_block_map.end(); iter++)
    {
        iter->second->ReSize(rect());
    }
    for(auto iter = m_stimulus_block_vect.begin(); iter != m_stimulus_block_vect.end(); iter++)
    {
        (*iter)->ReSize(rect());
    }
    if(m_sdisplay_wait_for_server != nullptr)
    {
        m_sdisplay_wait_for_server->SetPosition(rect());
    }
//    qDebug() << "resize";
}

bool paradigms::ParseJson(configor::json json_data)
{
    try
    {
        //全屏
        try
        {
            if(json_data["DialogSize"]["Type"] == "full_screen")
            {
                showFullScreen();
            }
            else
            {
                showNormal();
            }
        }
        catch (...)
        {
            qDebug() << "对话框大小读取出错";
            return false;
        }

        //打标
        try
        {
            configor::json mark_info = json_data["MarkerInfo"];
            for(auto iter = mark_info.begin(); iter != mark_info.end(); iter++)
            {
                configor::json value = iter.value();
                if(value["Type"] == "COM")
                {
                    m_serial = new SSerial();
                    m_serial->SetPortName(QString().fromStdString(value["PortName"]));
                    m_serial->SetBaudRate(value["BaudRate"]);
                    m_serial->SetDataBits(value["DataBits"]);
                    m_serial->SetParity(QString().fromStdString(value["Parity"]));
                    m_serial->SetStopBits(value["StopBits"]);
                    m_serial->SetFlowControl(QString().fromStdString(value["FlowControl"]));
                    m_serial->SetPreFix(hexStr_To_Hexchar(QString().fromStdString(value["PreFix"])));
                    if(!m_serial->OpenSerial())
                    {
                        QMessageBox::critical(this, tr("Error"), "打开串口出错");
                    }
                }
            }
        }
        catch(...)
        {
            qDebug() << "打标信息解析出错";
            return false;
        }

        //Instructions
        try
        {
            configor::json json_instruction = json_data["Instructions"];
            SControl *tmp = GenControl(json_instruction);
            if(tmp == nullptr)
            {
                throw std::string("GenControl error");
            }
            else
            {
                m_control_block_map["instructions"] = tmp;
            }
        }
        catch(std::string msg)
        {
            qDebug() << "Instructions 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "Instructions 出错:未知错误";
            return false;
        }

        //GoodBye
        try
        {
            configor::json json_goodbye = json_data["GoodBye"];
            SControl *tmp = GenControl(json_goodbye);
            if(tmp == nullptr)
            {
                throw std::string("GenControl error");
            }
            else
            {
                m_control_block_map["goodbye"] = tmp;
            }
        }
        catch(std::string msg)
        {
            qDebug() << "GoodBye 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "GoodBye 出错:未知错误";
            return false;
        }

        //TrailList
        configor::json json_trail_list = json_data["TrailList"];

        m_round = json_trail_list["Round"];
        m_loop_times = json_trail_list["TrailProc"]["LoopTimes"];
        m_loop_mode = std::string(json_trail_list["TrailProc"]["LoopMode"]);

        //TrailList_static_block
        try
        {
            configor::json json_static_block = json_trail_list["TrailProc"]["StaticBlock"];
            for(auto iter = json_static_block.begin(); iter != json_static_block.end(); iter++)
            {
                SControl *tmp = GenControl(iter.value());
                if(tmp == nullptr)
                {
                    throw std::string("GenControl error" + iter.key());
                }
                else
                {
                    m_control_block_map[iter.key()] = tmp;
                }
            }
        }
        catch(std::string msg)
        {
            qDebug() << "TrailList_static_block 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "TrailList_static_block 出错:未知错误";
            return false;
        }

        //TrailList_between_round
        try
        {
            configor::json json_between_round = json_trail_list["BetweenRound"];
            SControl *tmp = GenControl(json_between_round);
            if(tmp == nullptr)
            {
                throw std::string("GenControl error");
            }
            else
            {
                m_control_block_map["betweenround"] = tmp;
            }
        }
        catch(std::string msg)
        {
            qDebug() << "TrailList_between_round 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "TrailList_between_round 出错:未知错误";
            return false;
        }

        //TrailList_stimulus_block
        try
        {
            configor::json json_stimulus_block = json_trail_list["TrailProc"]["StimulusBlock"];
            for(auto iter = json_stimulus_block.begin(); iter != json_stimulus_block.end(); iter++)
            {
                configor::json json_single_stimulus_block = iter.value();

                SControl *tmp = GenControl(json_single_stimulus_block);
                if(tmp == nullptr)
                {
                    throw std::string("GenControl error");
                }
                else
                {
                    m_stimulus_block_vect.push_back(tmp);
                }
            }
        }
        catch(std::string msg)
        {
            qDebug() << "TrailList_stimulus_block 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "TrailList_stimulus_block 出错:未知错误";
            return false;
        }

        //范式所需内容读取完毕

        //执行顺序
        try
        {
            //刺激顺序生成
            for(int i = 0; i < m_round * m_loop_times; i++)
            {
                m_stimulus_order_vect.push_back(i % m_stimulus_block_vect.size());
            }
            if(m_loop_mode == "random")
            {
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::shuffle(m_stimulus_order_vect.begin(), m_stimulus_order_vect.end(), std::default_random_engine(seed));
            }

            //生成范式执行顺序
            m_paradigm_order_vect.push_back("instructions");

            for(int i = 0; i < m_round; i++)
            {
                for(int j = 0; j < m_loop_times; j++)
                {
                    for(auto iter = json_trail_list["TrailProc"]["Trail"].begin(); iter != json_trail_list["TrailProc"]["Trail"].end(); iter++)
                    {
                        configor::json json_tmp = iter.value();
                        if(json_tmp["Type"] == "StaticBlock")
                        {
                            m_paradigm_order_vect.push_back(json_tmp["Data"]);
                        }
                        else if(json_tmp["Type"] == "StimulusBlock")
                        {
                            m_paradigm_order_vect.push_back("stimulus");
                        }
                        else
                        {
                            throw std::string("unknown trail type");
                        }
                    }
                }
                if(i < m_round - 1)
                {
                    m_paradigm_order_vect.push_back("betweenround");
                }
            }

            m_paradigm_order_vect.push_back("goodbye");

//            for(auto iter = m_paradigm_order_vect.begin();iter!=m_paradigm_order_vect.end();iter++){
//                qDebug()<< (*iter).c_str();
//            }

        }
        catch(std::string msg)
        {
            qDebug() << "生成顺序 出错:" << msg.c_str();
            return false;
        }
        catch (...)
        {
            qDebug() << "生成顺序 出错:未知错误";
            return false;
        }

    }
    catch (...)
    {
        qDebug() << "unknown error";
    }

    return true;
}

SControl *paradigms::GenControl(configor::json json_data)
{
    SControl *p_scontrol = new SControl(this);
    if(p_scontrol == nullptr)
    {
        return nullptr;
    }

    //控制块
    try
    {
        p_scontrol->SetDuration(json_data["Control"]["Duration"]);

        //按键
        if(json_data["Control"]["Action"]["Key"].is_null() == false)
        {
            configor::json key_json = json_data["Control"]["Action"]["Key"];
            for(auto iter = key_json.begin(); iter != key_json.end(); iter++)
            {
                p_scontrol->AddKeyPressEvent(std::stoi(iter.key()), iter.value());
            }
        }

        //持续时间
        if(json_data["Control"]["Action"]["TimeOut"].is_null() == false)
        {
            //记超时为特殊的按键事件，为-1
            p_scontrol->AddKeyPressEvent(-1, json_data["Control"]["Action"]["TimeOut"]);
        }

        //读取打标
        if(json_data["Control"]["Marker"].is_null() == false)
        {
            for(auto iter = json_data["Control"]["Marker"].begin(); iter != json_data["Control"]["Marker"].end(); iter++)
            {
                configor::json marker_json = iter.value();
                if(marker_json["Type"] == "COM")
                {
                    std::string mark_data_str = marker_json["Data"];
                    p_scontrol->SetMarkData(hexStr_To_Hexchar(QString().fromStdString(mark_data_str)));
                }
            }
        }


    }
    catch (...)
    {
        delete p_scontrol;
        throw "control解析出错";
    }

    //添加display块
    try
    {
        for(auto iter = json_data["Display"].begin(); iter != json_data["Display"].end(); iter++)
        {
            configor::json display_json = iter.value();

            SDisplay *p_sdisplay = new SDisplay(this);

            try
            {
                if(display_json["DisplayType"] == "TextDisplay")
                {
                    //文本块
                    p_sdisplay->SetDisPlayType(SDisplay::DisPlayType::Text);
                    //设置内容
                    p_sdisplay->SetText(display_json["TextData"]);

                    //设置定位
                    p_sdisplay->SetPosition(rect());
                    if(display_json["Position"]["PosMode"] == "center")
                    {
                        p_sdisplay->SetTextCenter();
                    }
                    else
                    {
                        //固定定位还没有写！
                    }

                    //设置字体格式
                    QFont ft;
                    ft.setPointSize(display_json["TextFont"]);
                    ft.setBold(display_json["isBold"]);
                    p_sdisplay->SetFont(ft);

                }
                else if(display_json["DisplayType"] == "ImageDisplay")
                {
                    //图片块
                    p_sdisplay->SetDisPlayType(SDisplay::DisPlayType::Image);
                    //读取图片
                    std::string img_path = display_json["ImagePath"];
                    img_path = m_root_path + img_path;

                    QFileInfo file(img_path.c_str());
                    if(!file.exists())
                    {
                        throw img_path + "文件不存在";
                    }

                    QPixmap pix = QPixmap(img_path.c_str());
                    p_sdisplay->SetPixMap(pix);

                    //图片定位
                    QRect pix_rect = pix.rect();
                    QRect pix_loc;
                    if(display_json["Position"]["PosMode"] == "center")
                    {
                        pix_loc = QRect((width() - pix_rect.width()) / 2, (height() - pix_rect.height()) / 2, pix_rect.width(), pix_rect.height());
                    }
                    else
                    {
                        //固定定位还没有写！
                    }
                    p_sdisplay->SetPosition(pix_loc);

                }
                else if(display_json["DisplayType"] == "Nothing")
                {
                    //nothing 块

                }
                else
                {
                    throw std::string("unknown block type");
                }

            }
            catch(std::string msg)
            {
                qDebug() << "display块:" << msg.c_str();
                if(p_sdisplay != nullptr)
                {
                    delete p_sdisplay;
                    p_sdisplay = nullptr;
                }
            }
            catch(...)
            {
                qDebug() << "display块:unknown error";
                if(p_sdisplay != nullptr)
                {
                    delete p_sdisplay;
                    p_sdisplay = nullptr;
                }
            }

            if(p_sdisplay != nullptr)
            {
                p_scontrol->AddDisplayBlock(p_sdisplay);
            }
        }
    }
    catch (...)
    {
        delete p_scontrol;
        throw std::string("display解析出错");
    }

    return p_scontrol;
}

void paradigms::setNextBlock()
{
    m_currnet_timer_count_100ms = 0;
    std::string current_block_name = m_paradigm_order_vect[m_current_paradigm_order_step++];
    SControl *last_control = m_current_control_block;
    if(current_block_name != "stimulus")
    {
        m_current_control_block = m_control_block_map[current_block_name];
    }
    else
    {
        m_current_control_block = m_stimulus_block_vect[m_stimulus_order_vect[m_current_stimulus_order_step++]];
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
    if(m_current_paradigm_order_step < m_paradigm_order_vect.size())
    {
        //第一次
        if(m_current_control_block == nullptr)
        {
            m_current_control_block = m_control_block_map[m_paradigm_order_vect[m_current_paradigm_order_step++]];
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
                    memcpy(reinterpret_cast<char *>(&_msg_file_begin), tcp_buffer, sizeof(msg_file_begin));
                    tcp_buffer.remove(0, sizeof(msg_file_begin));

                    std::ofstream *ofs = new std::ofstream;
                    ofs->open(m_root_path + _msg_file_begin.name, std::ios::binary);
                    m_out_file_map[_msg_file_begin.name] = ofs;
                    break;
                }
            case info_type::file_data:
                {

                    if(tcp_buffer.size() < sizeof(msg_file_data))
                    {
                        return;
                    }
                    msg_file_data _msg_file_data;
                    memcpy(reinterpret_cast<char *>(&_msg_file_data), tcp_buffer, sizeof(msg_file_data));
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
                    m_exist_file_name_vect.push_back(_msg_file_end.name);

                    std::string file_path = m_root_path + _msg_file_end.name;
                    start_paradigms(file_path.c_str());

                    break;
                }
        }

    }


//    msg_base _msg_base;
//    memcpy(&_msg_base, m_tcp_client->read(sizeof(msg_base)).data(), sizeof(msg_base));

//    qDebug()<<_msg_base.m_type;

//    switch(_msg_base.m_type){
//    case info_type::file_begin:{
//        int remain_length = sizeof(msg_file_begin) - sizeof(msg_base);
//        msg_file_begin _msg_file_begin;
//        memcpy(reinterpret_cast<char*>(&_msg_file_begin) + sizeof(msg_base), m_tcp_client->read(remain_length).data(), remain_length);
//        std::ofstream* ofs = new std::ofstream;
//        ofs->open(m_root_path+_msg_file_begin.name,std::ios::binary);
//        m_out_file_map[_msg_file_begin.name] = ofs;
//        break;
//    }
//    case info_type::file_data: {
//        int remain_length = sizeof(msg_file_data) - sizeof(msg_base);
//        msg_file_data _msg_file_data;
//        memcpy(reinterpret_cast<char*>(&_msg_file_data) + sizeof(msg_base), m_tcp_client->read(remain_length).data(), remain_length);
//        m_out_file_map[_msg_file_data.name]->write(_msg_file_data.data, _msg_file_data.size);
//        break;
//    }
//    case info_type::file_end: {
//        int remain_length = sizeof(msg_file_end) - sizeof(msg_base);
//        msg_file_end _msg_file_end;
//        memcpy(reinterpret_cast<char*>(&_msg_file_end) + sizeof(msg_base), m_tcp_client->read(remain_length).data(), remain_length);
//        m_out_file_map[_msg_file_end.name]->close();
//        m_exist_file_name_vect.push_back(_msg_file_end.name);
//        break;
//    }
//    }
}
