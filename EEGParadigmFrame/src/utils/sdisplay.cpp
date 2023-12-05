#include "sdisplay.h"

SDisplay::SDisplay(DisPlayType type, QWidget *parent)
    : QLabel(parent)
{
    SetDisPlayType(type);
    //show();
    hide();
}

SDisplay::DisPlayType SDisplay::GetDisPlayType()
{
    return m_type;
}

void SDisplay::SetDisPlayType(DisPlayType type)
{

    m_type = type;
}
