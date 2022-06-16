#ifndef DATA_INFO_H
#define DATA_INFO_H

#define BUFFER_SIZE 1024


enum info_type
{
    invalid = 0,
    command,
    file_begin,
    file_data,
    file_end,
};

struct msg_base
{
    short m_type;
    msg_base()
    {
        m_type = info_type::invalid;
    }
};

struct msg_file_begin : public msg_base
{
    char name[32] = { 0 };
    int size = 0;
    msg_file_begin()
    {
        m_type = info_type::file_begin;
    }
};

struct msg_file_data : public msg_base
{
    char name[32] = { 0 };
    int size = 0;
    char data[BUFFER_SIZE] = { 0 };
    msg_file_data()
    {
        m_type = info_type::file_data;
    }
};

struct  msg_file_end : public msg_base
{
    char name[32] = { 0 };
    int result = -1;
    msg_file_end()
    {
        m_type = info_type::file_end;
    }
};

#endif // DATA_INFO_H
