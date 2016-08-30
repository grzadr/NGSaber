#include "include/processwindow.h"
#include "ui_processwindow.h"

#include "QMessageBox"
#include "QDebug"

ProcessWindow::ProcessWindow(QWidget *parent, QFuture<Launcher::Result> *arg_thread,
							 Parameters::LaunchParameters *params) :
	QDialog(parent),
	ui(new Ui::ProcessWindow)
{
	QTextStream terminal(stdout);
	ui->setupUi(this);
	thread = arg_thread;
	timer = new QTimer(this);

	ui->tabWidget->setCurrentIndex(0);

	if (params->input_single.isEmpty())
		ui->Status_SingleFrame->setVisible(false);
	if (params->input_alpha.isEmpty())
		ui->Status_PairedFrame->setVisible(false);

	ui->Param_SingleCount->setText(QString::number(params->input_single.size()));
	ui->Param_PairedCount->setText(QString::number(2 * params->input_alpha.size()));

	switch(params->mode_clever){
		case Parameters::None:
			ui->Param_ModeState->setText("None");
			break;
		case Parameters::Total:
			ui->Param_ModeState->setText("Total");
			break;
		case Parameters::Fast:
			ui->Param_ModeState->setText("Fast");
			break;
		case Parameters::CUDA:
			ui->Param_ModeState->setText("CUDA");
			break;
		default:
			break;
	}

	if (params->mode_rescue)
		ui->Param_RescueState->setText("Yes");
	else
		ui->Param_RescueState->setText("No");

	if (params->output_dir.isEmpty())
		ui->Param_DirectoryString->setText("None");
	else
		ui->Param_DirectoryString->setText(params->output_dir);

	if (params->output_prefix.isEmpty())
		ui->Param_PrefixString->setText("None");
	else
		ui->Param_PrefixString->setText(params->output_prefix);

	ui->Param_RatioValue->setText(QString::number(params->min_ratio));
	ui->Param_LengthValue->setText(QString::number(params->min_length));
	ui->Param_FirstValue->setText(QString::number(params->cut_first));
	ui->Param_LastValue->setText(QString::number(params->cut_last));

	ui->Param_OptValue->setText(QString::number(params->quality_opt));
	ui->Param_LowValue->setText(QString::number(params->quality_trash));
	ui->Param_PhredValue->setText(QString::number(params->quality_phred));
	ui->Param_ThreadsValue->setText(QString::number(params->threads));
}

ProcessWindow::~ProcessWindow()
{
	emit canceled();
	delete ui;
	timer->stop();
	delete timer;
}

void ProcessWindow::on_CancelButton_clicked()
{
	if (!state){
		ui->LogText->append("CANCELED");
		emit canceled();
	 } else
		this->close();
}

void ProcessWindow::on_ThreadFinished()
{
	timer->stop();
	thread->waitForFinished();
	state = 1;
	ui->CancelButton->setText("Quit");
	UpdateTime();
}

void ProcessWindow::on_Message(QString arg_message)
{
	ui->LogText->append(arg_message);
}

void ProcessWindow::UpdateTime(){
	qint64 temp_elapsed = (elapsed_time->elapsed())/1000;
	QString seconds = QString("%1").arg(temp_elapsed%60, 2, 10, QChar('0'));
	QString minutes = QString("%1").arg((temp_elapsed/60)%60, 2, 10, QChar('0'));
	QString hours = QString("%1").arg(temp_elapsed/3600, 2, 10, QChar('0'));
	QString time = QString("Elapsed Time %1:%2:%3").arg(hours, minutes, seconds);
	ui->Time->setText(time);

	UpdateStats();
}

void ProcessWindow::UpdateStats()
{
	if (ui->Status_SingleFrame->isVisible()){
		QString singlefiles = QString::number(stats->singlefilescurrent) + "/" +
				QString::number(stats->singlefilestotal);
		ui->Status_SingleFilesCount->setText(singlefiles);

		ui->Status_SingleFilesProgress->setValue(static_cast<int>(
						100*stats->singlefilescurrent/stats->singlefilestotal));

		ui->Status_SingleReadsCount->setText(QString::number(stats->singleretrieved+
															 stats->singlerejected));

		ui->Status_SingleResultRetrievedCount->setText(
					QString::number(stats->singleretrieved));
		ui->Status_SingleResultFilteredCount->setText(
					QString::number(stats->singlerejected));

		ui->Status_SingleResultProgress->setValue(
					static_cast<int>(100*stats->singleretrieved/
									 (stats->singleretrieved+stats->singlerejected)));
	}
	if (ui->Status_PairedFrame->isVisible() and stats->singlefinished){
		QString pairedfiles = QString::number(stats->pairedfilescurrent) + "/" +
				QString::number(stats->pairedfilestotal);
		ui->Status_PairedFilesCount->setText(pairedfiles);

		ui->Status_PairedFilesProgress->setValue(static_cast<int>(
								100*stats->pairedfilescurrent/stats->pairedfilestotal));

		ui->Status_PairedReadsCount->setText(QString::number(stats->pairedretrieved+
															 stats->pairedrejected));

		ui->Status_PairedResultRetrievedCount->setText(
					QString::number(stats->pairedretrieved));
		ui->Status_PairedResultFilteredCount->setText(
					QString::number(stats->pairedrejected));

		ui->Status_PairedResultProgress->setValue(
					static_cast<int>(100*stats->pairedretrieved/
									 (stats->pairedretrieved+stats->pairedrejected)));
	}
}

void ProcessWindow::on_Error()
{
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setText("An error occured\nPlease, view \'Log\' for further details");
	ui->tabWidget->setCurrentIndex(2);
	msgBox.exec();
}

void ProcessWindow::on_Canceled()
{
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("Filtering process has been canceled");
	ui->tabWidget->setCurrentIndex(2);
	msgBox.exec();
}
