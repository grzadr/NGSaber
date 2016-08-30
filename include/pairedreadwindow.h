#ifndef PAIREDREADWINDOW_H
#define PAIREDREADWINDOW_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PairedReadWindow;
}

class PairedReadWindow : public QDialog
{
		Q_OBJECT

	public:
		explicit PairedReadWindow(QWidget *parent = 0,
								  QStringList* arg_AlphaList = nullptr,
								  QStringList* arg_BetaList = nullptr);
		~PairedReadWindow();

		QString defaultPath;
		QStringList *fileList = nullptr;
		QStringList *alphaList = nullptr;
		QStringList *betaList = nullptr;

	private slots:
		void on_Browse_clicked();

		void on_Clear_clicked();

		void on_AlphaContextMenuTriggered(const QPoint &point);

		void on_BetaContextMenuTriggered(const QPoint &point);

		void on_RemoveItem();

	private:
		Ui::PairedReadWindow *ui;
		void printList();
		QString getPath(QString arg_path);
};

#endif // PAIREDREADWINDOW_H
