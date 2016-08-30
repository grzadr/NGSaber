#ifndef SINGLEREADWINDOW_H
#define SINGLEREADWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QPoint>
//#include <QStringList>

namespace Ui {
class SingleReadWindow;
}

class SingleReadWindow : public QDialog
{
		Q_OBJECT

	public:
		explicit SingleReadWindow(QWidget *parent = 0,
								  QStringList* arg_fileList = nullptr);
		~SingleReadWindow();
		QStringList* fileList = nullptr;
		QString defaultPath;

	private slots:

		void on_Clear_clicked();

		void on_Browse_clicked();

		void on_ContextMenuTriggered(const QPoint &point);

		void on_RemoveItem();

	private:
		Ui::SingleReadWindow *ui;
		void PrintFiles();
		QString getPath(QString arg_path);
};

#endif // SINGLEREADWINDOW_H
