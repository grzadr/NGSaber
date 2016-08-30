#ifndef FASTQPAIR_H
#define FASTQPAIR_H

#include "include/fastqfile.h"
#include "include/readpaired.h"

class FASTQPair
{
public:
	struct StatisticsPaired {
		int Total = 0;
		int Both = 0;
		int OkAlpha = 0;
		int OkBeta = 0;
		int Trash = 0;

		void update(ReadPaired::Result arg_result);
		QString print(bool arg_rescue);
	} stats;

	enum FASTQPairStatus{
		Ok = 0,
		EndOfFiles = 1,
		OpenError = 2,
		Error = 3

	};

	FASTQPair(QString arg_alpha_input_name, QString arg_beta_input_name,QString arg_output_dir, QString arg_output_prefix, bool arg_rescue);
	~FASTQPair();

	FASTQPairStatus input_status = Ok;
	FASTQPairStatus output_status = Ok;

	FASTQFile *alpha;
	QTextStream *alpha_output_stream = nullptr;

	FASTQFile *beta;
	QTextStream *beta_output_stream = nullptr;

	QFile *alpha_rescue;
	QTextStream *alpha_output_rescue_stream = nullptr;
	FASTQPairStatus alpha_rescue_status = Ok;

	QFile *beta_rescue;
	QTextStream *beta_output_rescue_stream = nullptr;
	FASTQPairStatus beta_rescue_status = Ok;

	void getRead(ReadPaired *result);

private:
	QString output_dir = "";
	QString output_prefix = "";

	QString alpha_input_name = "";
	QString alpha_output_rescue_name = "";

	QString beta_input_name = "";
	QString beta_output_rescue_name = "";

	void openRescue();

	QString GetFilesName( QString arg_file );
	QString GetFilesPath( QString arg_file );

};

#endif // FASTQPAIR_H
