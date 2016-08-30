#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QImage>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QDialog
{
		Q_OBJECT

	public:
		explicit HelpWindow(QWidget *parent = 0);
		~HelpWindow();

	private slots:
		void on_HelpManual_button_clicked();

	private:
		Ui::HelpWindow *ui;
		QImage *image = nullptr;
};

#endif // HELPWINDOW_H
