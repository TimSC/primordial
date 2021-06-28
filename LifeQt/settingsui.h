#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QDialog>

namespace Ui {
class SettingsUi;
}

class SettingsUi : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsUi(QWidget *parent = nullptr);
    ~SettingsUi();

private:
    Ui::SettingsUi *ui;
};

#endif // SETTINGSUI_H
