#include "include/pairedreadwindow.h"
#include "ui_pairedreadwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>

int indexRemove = 0;

PairedReadWindow::PairedReadWindow(QWidget *parent, QStringList *arg_AlphaList,
								   QStringList *arg_BetaList) :
	QDialog(parent),
	ui(new Ui::PairedReadWindow)
{
	ui->setupUi(this);
	alphaList = arg_AlphaList;
	betaList = arg_BetaList;
	ui->AlphaList->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->BetaList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->AlphaList, &QWidget::customContextMenuRequested,
			this, &PairedReadWindow::on_AlphaContextMenuTriggered);
	connect(ui->BetaList, &QWidget::customContextMenuRequested,
			this, &PairedReadWindow::on_BetaContextMenuTriggered);
	printList();
}

PairedReadWindow::~PairedReadWindow()
{
	delete ui;
}

void PairedReadWindow::printList()
{
	ui->AlphaList->clear();
	ui->BetaList->clear();

	if (alphaList->isEmpty()){
		ui->AlphaList->addItem("Empty");
		ui->BetaList->addItem("Empty");
	} else {
		ui->AlphaList->addItems(*alphaList);
		ui->BetaList->addItems(*betaList);
	}
}

void PairedReadWindow::on_Browse_clicked()
{
	QStringList temp_fileList = QFileDialog::getOpenFileNames(this,
											 "Select one or more files to open",
											 defaultPath);
	if (!temp_fileList.isEmpty()){
		defaultPath = getPath(temp_fileList.first());
		if ( temp_fileList.size() % 2 ){
			QMessageBox msgBox(QMessageBox::Critical, "Error", "Odd number of files",
							   QMessageBox::Ok );
			msgBox.exec();
		} else {
			for ( int idx = 0; idx < temp_fileList.size(); ++idx ){
				if (idx % 2)
					alphaList->append(temp_fileList.at(idx));
				else
					betaList->append(temp_fileList.at(idx));
			}
			alphaList->removeDuplicates();
			betaList->removeDuplicates();
		}
	}
	printList();
}

QString PairedReadWindow::getPath(QString arg_path)
{
	QString result = arg_path.section("/", 0, -2);
	if (result.isEmpty())
		return ".";
	else
		return result;
}

void PairedReadWindow::on_Clear_clicked()
{
	ui->AlphaList->clear();
	ui->BetaList->clear();

	ui->AlphaList->addItem("Empty");
	ui->BetaList->addItem("Empty");

	alphaList->clear();
	betaList->clear();
}

void PairedReadWindow::on_AlphaContextMenuTriggered(const QPoint &point)
{
	QMenu menu;
	QAction RemoveItem("Remove", &menu);
	RemoveItem.setData(point);
	menu.addAction(&RemoveItem);
	connect(&RemoveItem, &QAction::triggered, this, &PairedReadWindow::on_RemoveItem);

	QModelIndex index = ui->AlphaList->indexAt(point);
	if (index.isValid() and ui->AlphaList->item(0)->text() != "Empty") {
		indexRemove = index.row();
		menu.exec(ui->AlphaList->mapToGlobal(point));
	}
}

void PairedReadWindow::on_BetaContextMenuTriggered(const QPoint &point)
{
	QMenu menu;
	QAction RemoveItem("Remove", &menu);
	RemoveItem.setData(point);
	menu.addAction(&RemoveItem);
	connect(&RemoveItem, &QAction::triggered, this, &PairedReadWindow::on_RemoveItem);

	QModelIndex index = ui->BetaList->indexAt(point);
	if (index.isValid() and ui->BetaList->item(0)->text() != "Empty") {
		indexRemove = index.row();
		menu.exec(ui->BetaList->mapToGlobal(point));
	}
}

void PairedReadWindow::on_RemoveItem()
{
	alphaList->removeAt(indexRemove);
	betaList->removeAt(indexRemove);
	printList();
}
