#ifndef SETTINGSBIOT_H
#define SETTINGSBIOT_H

#include <QWidget>

namespace Ui {
class SettingsBiot;
}

class SettingsBiot : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsBiot(QWidget *parent = nullptr);
    ~SettingsBiot();

private:
    Ui::SettingsBiot *ui;
};

#endif // SETTINGSBIOT_H
