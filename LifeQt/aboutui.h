#ifndef ABOUTUI_H
#define ABOUTUI_H

#include <QDialog>

namespace Ui {
class AboutUi;
}

class AboutUi : public QDialog
{
    Q_OBJECT

public:
    explicit AboutUi(QWidget *parent = nullptr);
    ~AboutUi();

private:
    Ui::AboutUi *ui;
};

#endif // ABOUTUI_H
