#ifndef SETTINGSHABITAT_H
#define SETTINGSHABITAT_H

#include <QWidget>
#include "core/Settings.h"

namespace Ui {
class SettingsHabitat;
}

class SettingsHabitat : public QWidget
{
    friend class SettingsUi;
    Q_OBJECT

public:
    explicit SettingsHabitat(QWidget *parent = nullptr);
    ~SettingsHabitat();

    void Init(class CSettings &settingsIn);
    void Accept(class CSettings &settingsIn);

private:
    Ui::SettingsHabitat *ui;
};

#endif // SETTINGSHABITAT_H
