#include "BreakDialog.h"
#include "ui_BreakDialog.h"

BreakDialog::BreakDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::BreakDialog)
{
    ui->setupUi(this);
}

BreakDialog::~BreakDialog()
{
    delete ui;
}
