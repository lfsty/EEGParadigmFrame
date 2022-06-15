#ifndef SDISPLAY_H
#define SDISPLAY_H

#include <QLabel>

class SDisplay : public QLabel
{
public:
    enum class DisPlayType
    {
        Text,
        Image
    };
public:
    SDisplay(DisPlayType type, QWidget *parent = nullptr);
    DisPlayType GetDisPlayType();
protected:
    void SetDisPlayType(DisPlayType type);
private:
    DisPlayType m_type;
};

#endif // SDISPLAY_H
