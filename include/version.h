#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QDate>
#include <QChar>
struct Version
{
	QString BUILD = "";
	QString QT = "";

	Version(){
		QDate temp_date = QDate().currentDate();
		BUILD += QString::number(temp_date.year());
		BUILD += QString("%1").arg(temp_date.month(), 2, 10, QChar('0'));
		BUILD += QString::number(temp_date.day());
	}
};

static Version version;

#endif // VERSION_H
