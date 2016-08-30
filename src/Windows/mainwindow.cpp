#include "include/mainwindow.h"
#include "ui_mainwindow.h"

#include "include/helpwindow.h"
#include "ui_helpwindow.h"
#include "include/singlereadwindow.h"
#include "ui_singlereadwindow.h"
#include "include/pairedreadwindow.h"
#include "ui_pairedreadwindow.h"
#include "include/processwindow.h"
#include "ui_processwindow.h"
#include "include/launcher.h"
#include "include/version.h"

#include <QTextStream>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>

QString DefaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
QString OutputDir = QString();
QString OutputPrefix = "CLEARED_";
QStringList SingleList, AlphaList, BetaList;

MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->MainLoadSingle->setText("Load Single Read Files \u2716");
	ui->MainLoadPaired->setText("Load Paired Reads Files \u2716");
	image = new QImage(":/baner.png");
		ui->MainTitle_label->setPixmap(QPixmap::fromImage(*image).
									  scaled(QSize(500,100),
											 Qt::KeepAspectRatio,
											 Qt::SmoothTransformation));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::AllowStart()
{
	if(!SingleList.isEmpty() or (!AlphaList.isEmpty() and !BetaList.isEmpty())){
		ui->MainButtonStart->setEnabled(1);
	} else {
		ui->MainButtonStart->setEnabled(0);
	}
}

void MainWindow::on_CancelThread()
{
	mStop = true;
}

void MainWindow::on_MainQuitButton_clicked()
{
	QApplication::quit();
}

void MainWindow::on_MainOutputDirectory_clicked()
{
	QString output_dir = QFileDialog::
			getExistingDirectory(this, tr("Open Output Directory"), DefaultPath,
								 QFileDialog::ShowDirsOnly |
								 QFileDialog::DontResolveSymlinks);
	if (!output_dir.isEmpty())
		OutputDir = output_dir;
	else
		OutputDir = QString();
}

void MainWindow::on_MainHelpButton_clicked()
{
	HelpWindow window;
	window.exec();
}

void MainWindow::on_MainLoadSingle_clicked()
{
	SingleReadWindow window(this, &SingleList);
	window.defaultPath = DefaultPath;
	window.exec();
	if (SingleList.isEmpty()){
		ui->MainLoadSingle->setText("Load Single Read Files \u2716");
	} else
		ui->MainLoadSingle->setText("Load Single Read Files \u2714");
	DefaultPath = window.defaultPath;
	AllowStart();
}

void MainWindow::on_MainLoadPaired_clicked()
{
	PairedReadWindow window(this, &AlphaList, &BetaList);
	window.defaultPath = DefaultPath;
	window.exec();
	if (AlphaList.isEmpty()){
		ui->MainLoadPaired->setText("Load Paired Reads Files \u2716");
		ui->AdvOptRescue->setEnabled(0);
		ui->AdvOptRescue->setChecked(0);
	} else {
		ui->MainLoadPaired->setText("Load Paired Reads Files \u2714");
		ui->AdvOptRescue->setEnabled(1);
		ui->AdvOptRescue->setChecked(0);
	}
	DefaultPath = window.defaultPath;
	AllowStart();
}

void MainWindow::on_AdvOptHighSpinBox_editingFinished()
{
	int low_value = ui->AdvOptLowSpinBox->value();

	if (ui->AdvOptHighSpinBox->value() <= low_value)
		ui->AdvOptHighSpinBox->setValue(low_value + 1);
}

void MainWindow::on_AdvOptLowSpinBox_editingFinished()
{
	int high_value = ui->AdvOptHighSpinBox->value();

	if (ui->AdvOptLowSpinBox->value() >= high_value)
		ui->AdvOptLowSpinBox->setValue(high_value - 1);
}

void MainWindow::on_MainButtonStart_clicked()
{
	params.input_single = SingleList;

	if (!AlphaList.isEmpty()){
		params.input_alpha = AlphaList;
		params.input_beta = BetaList;
	}

	params.output_dir = OutputDir;

	if (!params.output_dir.isNull() and !params.output_dir.endsWith("/"))
		params.output_dir += "/";

	if (ui->MainOutputPrefix->text().isEmpty())
		params.output_prefix = "CLEARED_";
	else
		params.output_prefix = ui->MainOutputPrefix->text();
	if (!params.output_prefix.endsWith("_"))
		params.output_prefix += "_";

	params.threads = ui->AdvOptThreadsSpinBox->value();
	params.min_ratio = ui->AdvOptRatioSpin->value();
	params.min_length = ui->AdvOptMinLenSpinBox->value();
	params.quality_opt = ui->AdvOptHighSpinBox->value();
	params.quality_trash = ui->AdvOptLowSpinBox->value();

	switch(ui->AdvOptPhred->currentIndex()){
		case 0:
			params.quality_phred = 33;
			break;
		case 1:
			params.quality_phred = 64;
			break;
		default:
			break;
	}

	params.cut_first = ui->AdvOptCutFirstSpinBox->value();
	params.cut_last = ui->AdvOptCutLastSpinBox->value();

	switch (ui->MainModeComboBox->currentIndex()) {
		case 0:
			params.mode_clever = Parameters::None;
			break;
		case 1:
			params.mode_clever = Parameters::Total;
			break;
		case 2:
			params.mode_clever = Parameters::Fast;
			break;
		case 3:
			params.mode_clever = Parameters::CUDA;
			break;
		default:
			break;
	}

	params.mode_rescue = ui->AdvOptRescue->isChecked();
	mStop = false;

	thread = new QFuture<Launcher::Result>;
	launcher = new Launcher(this, &params, &mStop);
	window = new ProcessWindow(this, thread, &params);

	connect(window, &ProcessWindow::canceled, this, &MainWindow::on_CancelThread);
	connect(launcher, &Launcher::finished, window, &ProcessWindow::on_ThreadFinished);
	connect(launcher, &Launcher::message, window, &ProcessWindow::on_Message);
	connect(window->timer, &QTimer::timeout, window, &ProcessWindow::UpdateTime);
	connect(launcher, &Launcher::error, window, &ProcessWindow::on_Error);
	connect(launcher, &Launcher::canceled, window, &ProcessWindow::on_Canceled);

	window->elapsed_time = &launcher->elapsed_time;

	*thread = QtConcurrent::run(launcher, &Launcher::process);
	window->timer->start(1000);
	window->stats = &launcher->stats;
	window->exec();
	delete window;
	delete launcher;
	delete thread;
}
