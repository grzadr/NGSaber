#ifndef FASTQFILE_H
#define FASTQFILE_H

#include "include/readsingle.h"

#include <QString>
#include <QFile>
#include <QTextStream>

class FASTQFile
{
	private:

		QFile * input_file;
		QTextStream * input_stream;
		void openInput();

		QString output_dir = "";
		QString output_prefix = "";
		QFile * output_file;
		void openOutput();

	public:
		struct Statistics{
			int Total = 0;

			int TooShort = 0;
			int ZeroQuality = 0;
			int LowQuality = 0;
			int GoodQuality = 0;

			int NoUnits = 0;
			int TooShortForClever = 0;
			int TooShortAfterClever = 0;
			int OkAfterClever = 0;

			void update(ReadSingle::Result);
			QString print(bool arg_clever);
		} stats;

		enum Status{
			Ok = 0,
			EndOfFile = 1,
			OpenError = 2,
			EmptyLine = 3,
			MissingAt = 4,
			MissingName = 5,
			MissingPlus = 6,
			DifferentLength = 7
		};

		FASTQFile(QString arg_input_name , QString arg_output_dir,
				  QString arg_output_prefix);
		~FASTQFile();

		struct Error{
			Status *status = nullptr;
			QString *name = nullptr;
			int *line = nullptr;
		} error;

		QString input_name = "";
		Status input_status = Ok;

		QString output_name = "";
		Status output_status = Ok;

		QTextStream * output_stream;

		int last_line = 0;

		QString GetFilesPath( QString arg_file );
		QString GetFilesName( QString arg_file );

		void GetRead(ReadSingle* rst_read);
};

#endif // FASTQFILE_H
