#include "include/helpwindow.h"
#include "ui_helpwindow.h"
#include "include/version.h"

#include <QDate>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
	ui->setupUi(this);
	ui->HelpName_label->setText("NGSaber " + version.BUILD);
	ui->HelpQtVersion_label->setText("Based on Qt " + QString(QT_VERSION_STR));
	image = new QImage(":/logo.png");
		ui->HelpLogo_label->setPixmap(QPixmap::fromImage(*image).
									  scaled(QSize(100,100),
											 Qt::KeepAspectRatio,
											 Qt::SmoothTransformation));
	ui->HelpLicence_label->setOpenExternalLinks(true);
}

HelpWindow::~HelpWindow()
{
	delete ui;
}

void HelpWindow::on_HelpManual_button_clicked()
{
}
