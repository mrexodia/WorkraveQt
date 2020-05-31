#include "TimerDialog.h"
#include "ui_TimerDialog.h"

TimerDialog::TimerDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);
}

TimerDialog::~TimerDialog()
{
    delete ui;
}
