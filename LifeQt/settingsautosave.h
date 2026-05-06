#ifndef SETTINGSAUTOSAVE_H
#define SETTINGSAUTOSAVE_H

#include <QWidget>
#include "core/Settings.h"

namespace Ui {
class SettingsAutosave;
}

class SettingsAutosave : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsAutosave(QWidget *parent = nullptr);
    ~SettingsAutosave();

    void Init(class CSettings &settingsIn);
    void Accept(class CSettings &settingsIn);

private:
    Ui::SettingsAutosave *ui;
};

#endif // SETTINGSAUTOSAVE_H
