#ifndef SETTINGSBIOT_H
#define SETTINGSBIOT_H

#include <QWidget>

namespace Ui {
class SettingsBiot;
}

class SettingsBiot : public QWidget
{
    friend class SettingsUi;
    Q_OBJECT

public:
    explicit SettingsBiot(QWidget *parent = nullptr);
    ~SettingsBiot();

    void Init(class CSettings &settingsIn);
    void Accept(class CSettings &settingsIn);
private:
    Ui::SettingsBiot *ui;
};

#endif // SETTINGSBIOT_H
