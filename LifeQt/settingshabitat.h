#ifndef SETTINGSHABITAT_H
#define SETTINGSHABITAT_H

#include <QWidget>

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

private:
    Ui::SettingsHabitat *ui;
};

#endif // SETTINGSHABITAT_H
