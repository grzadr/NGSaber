#include "include/singlereadwindow.h"
#include "ui_singlereadwindow.h"

#include <QFileDialog>
#include <QStringList>
#include <QDebug>
#include <QMenu>

int indexDelete = 0;

SingleReadWindow::SingleReadWindow(QWidget *parent, QStringList *arg_fileList) :
    QDialog(parent),
    ui(new Ui::SingleReadWindow)
{
	ui->setupUi(this);
	fileList = arg_fileList;
	PrintFiles();
	ui->List->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->List, &QWidget::customContextMenuRequested,
			this, &SingleReadWindow::on_ContextMenuTriggered);
}

SingleReadWindow::~SingleReadWindow()
{
	delete ui;
}

void SingleReadWindow::on_Clear_clicked()
{
	fileList->clear();
	PrintFiles();
}

void SingleReadWindow::on_Browse_clicked()
{
	QStringList temp_fileList = QFileDialog::getOpenFileNames(this,
											 "Select one or more files to open",
											 defaultPath);
	if (!temp_fileList.isEmpty()){
		defaultPath = getPath(temp_fileList.first());
		fileList->append(temp_fileList);
		fileList->removeDuplicates();
	}
	PrintFiles();
}

void SingleReadWindow::PrintFiles()
{
	ui->List->clear();
	if (fileList->isEmpty())
		ui->List->addItem("Empty");
	 else
		ui->List->addItems(*fileList);
}

QString SingleReadWindow::getPath(QString arg_path)
{
	QString result = arg_path.section("/", 0, -2);
	if (result.isEmpty())
		return ".";
	else
		return result;
}

void SingleReadWindow::on_ContextMenuTriggered(const QPoint &point)
{
	QMenu menu;
	QAction RemoveItem("Remove", &menu);
	RemoveItem.setData(point);
	menu.addAction(&RemoveItem);
	connect(&RemoveItem, &QAction::triggered, this, &SingleReadWindow::on_RemoveItem);
	QModelIndex index = ui->List->indexAt(point);
	if (index.isValid() and ui->List->item(0)->text() != "Empty") {
		qDebug() << index.row();
		indexDelete = index.row();
		menu.exec(ui->List->mapToGlobal(point));
	}
}

void SingleReadWindow::on_RemoveItem()
{
	fileList->removeAt(indexDelete);
	PrintFiles();
}
