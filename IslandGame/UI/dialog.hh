#ifndef DIALOG_HH
#define DIALOG_HH

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    int getPlayerAmount();

public slots:
    void asetaPelaajat();

signals:


private:
    Ui::Dialog *ui;

    int players_;  // Muuttuja johon tallennetaan pelaajamäärä
};

#endif // DIALOG_HH
