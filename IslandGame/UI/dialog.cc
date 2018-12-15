#include "dialog.hh"
#include "ui_dialog.h"

#include <QSpinBox>

#include <string>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->continueButton,SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(ui->quitButton,SIGNAL(clicked(bool)),this,SLOT(reject()));

    // Asetetaan aluksi pelaajamäärä minimimääräksi.
    players_ = ui->pelaajaMaara->value();
    connect(ui->pelaajaMaara,SIGNAL(valueChanged(int)),
            this,SLOT(asetaPelaajat()));
}

Dialog::~Dialog()
{
    delete ui;
}

int Dialog::getPlayerAmount()
{
    return players_;
}

void Dialog::asetaPelaajat()
{
    players_ = ui->pelaajaMaara->value();
}




