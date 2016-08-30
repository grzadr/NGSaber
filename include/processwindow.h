#ifndef PROCESSWINDOW_H
#define PROCESSWINDOW_H

#include "include/parameters.h"
#include "include/launcher.h"

#include <QDialog>
#include <QFuture>
#include <QTimer>

namespace Ui {
class ProcessWindow;
}

class ProcessWindow : public QDialog
{
		Q_OBJECT
	private:
		Ui::ProcessWindow *ui;
		int state = 0;
		QFuture<Launcher::Result> *thread = nullptr;

	public:
		explicit ProcessWindow(QWidget *parent = 0,
							   QFuture<Launcher::Result> *arg_thread = nullptr,
							   Parameters::LaunchParameters *params = nullptr);
		~ProcessWindow();

		void on_ThreadFinished();
		void on_Message(QString arg_message);
		void UpdateTime();
		void UpdateStats();
		void on_Error();
		void on_Canceled();

		QTimer *timer = nullptr;
		QElapsedTimer *elapsed_time = nullptr;
		Launcher::Stats *stats = nullptr;

	signals:
		void canceled();
		void finished();

	private slots:
		void on_CancelButton_clicked();
};

#endif // PROCESSWINDOW_H
