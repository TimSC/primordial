#ifndef SETTINGSNETWORK_H
#define SETTINGSNETWORK_H

#include <QDialog>
#include <QStringListModel>
#include "core/Settings.h"

namespace Ui {
class SettingsNetwork;
}

class SettingsNetwork : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsNetwork(QWidget *parent = nullptr);
    ~SettingsNetwork();

    void Init(class CSettings &settingsIn);
    void Accept(class CSettings &settingsIn);

private slots:
    void on_addPushButton_clicked();

    void on_removePushButton_clicked();

private:
    Ui::SettingsNetwork *ui;
    QStringListModel connectList;
};

#endif // SETTINGSNETWORK_H
