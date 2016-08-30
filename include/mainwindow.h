#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QFuture>
#include <QImage>

#include "include/parameters.h"
#include "include/launcher.h"
#include "include/processwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void AllowStart();

		void on_CancelThread();

		void on_MainQuitButton_clicked();

		void on_MainOutputDirectory_clicked();

		void on_MainHelpButton_clicked();

		void on_MainLoadSingle_clicked();

		void on_MainLoadPaired_clicked();

		void on_MainButtonStart_clicked();

		void on_AdvOptHighSpinBox_editingFinished();

		void on_AdvOptLowSpinBox_editingFinished();

	private:
		Ui::MainWindow *ui;

		Parameters::LaunchParameters params;
		Launcher *launcher = nullptr;
		QFuture<Launcher::Result> *thread = nullptr;
		ProcessWindow *window = nullptr;

		bool mStop = false;

		QImage *image = nullptr;

	signals:
		void exec_window();
};

#endif // MAINWINDOW_H
