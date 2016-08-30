#include "include/mainwindow.h"
#include "include/parameters.h"
#include "include/launcher.h"
#include "include/version.h"

#include <QApplication>
#include <QDebug>
#include <QDate>

QTextStream terminal(stdout);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationDisplayName("NGSaber");
	a.setApplicationName("ngsaber");
	a.setApplicationVersion(version.BUILD);

	if (a.arguments().size() == 1){
		MainWindow w;
		w.show();

		return a.exec();
	} else {
		Parameters params(&a);
		if (params.processArguments()){
			qCritical() << "Error occured during parsing arguments. NGSaber terminates.";
			return 1;
		}

		Launcher worker(0, &params.launch_parameters);

		if (worker.process()){
			qCritical() << "NGSaber terminates.";
			return 1;
		}
	}
}
