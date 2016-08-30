#include "windowhelp.h"
#include "ui_windowhelp.h"

WindowHelp::WindowHelp(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::WindowHelp)
{
	ui->setupUi(this);
}

WindowHelp::~WindowHelp()
{
	delete ui;
}
