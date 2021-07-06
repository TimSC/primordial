#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QDialog>
#include "core/Settings.h"
#include "networking.h"

namespace Ui {
class SettingsUi;
}

class SettingsUi : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsUi(class CSettings &settingsIn, QWidget *parent = nullptr);
    ~SettingsUi();

    void accept() override;
    void reject() override;

private:
    Ui::SettingsUi *ui;
    class CSettings &settings;
};

#endif // SETTINGSUI_H
