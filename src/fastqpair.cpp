#include "include/fastqpair.h"

FASTQPair::FASTQPair(QString arg_alpha_input_name, QString arg_beta_input_name,
					 QString arg_output_dir, QString arg_output_prefix, bool arg_rescue)
{
	QTextStream terminal(stdout);

	alpha_input_name = arg_alpha_input_name;
	beta_input_name = arg_beta_input_name;

	output_prefix = arg_output_prefix;

	if(arg_output_dir.isNull())
		output_dir = GetFilesPath(alpha_input_name);
	else
		output_dir = arg_output_dir;

	try{
		alpha = new FASTQFile(alpha_input_name, output_dir, output_prefix);
	}
	catch(int e){
		terminal << "ALPHA ERROR" << endl;
		throw 2;
	}

	try{
		beta = new FASTQFile(beta_input_name, output_dir, output_prefix);
	}
	catch(int e){
		terminal << "BETA ERROR" << endl;
		throw 2;
	}

	if (alpha->input_status == FASTQFile::Ok and alpha->output_status == FASTQFile::Ok)
		alpha_output_stream = alpha->output_stream;

	if (beta->input_status == FASTQFile::Ok and beta->output_status == FASTQFile::Ok)
		beta_output_stream = beta->output_stream;

	if (arg_rescue){
		openRescue();
		if (alpha_rescue_status or beta_rescue_status)
			throw 2;
	}
}

FASTQPair::~FASTQPair(){
	delete alpha;
	delete beta;

	if (alpha_output_rescue_stream != nullptr){
		delete alpha_output_rescue_stream;
		alpha_rescue->close();
		delete alpha_rescue;
	}

	if (beta_output_rescue_stream != nullptr){
		delete beta_output_rescue_stream;
		beta_rescue->close();
		delete beta_rescue;
	}
}

void FASTQPair::openRescue()
{
	QTextStream terminal(stdout);

	alpha_output_rescue_name = output_dir + output_prefix + "R_" +
			GetFilesName(alpha_input_name);

	alpha_rescue = new QFile(alpha_output_rescue_name);

	if (!alpha_rescue->open( QIODevice::WriteOnly | QIODevice::Text)){
		terminal << "### FILE ERROR ###" << endl
				  << "OUTPUT FILE NAME: " << alpha_output_rescue_name << endl
				  << "CANNOT OPEN OUTPUT FILE" << endl;
		alpha_rescue_status = OpenError;
	} else {
		alpha_rescue_status = Ok;
		alpha_output_rescue_stream = new QTextStream(alpha_rescue);
	}

	beta_output_rescue_name = output_dir + output_prefix + "R_" +
			GetFilesName(beta_input_name);

	beta_rescue = new QFile( beta_output_rescue_name );

	if ( !beta_rescue->open( QIODevice::WriteOnly | QIODevice::Text)){
		terminal << "### FILE ERROR ###" << endl
				  << "OUTPUT FILE NAME: " << beta_output_rescue_name << endl
				  << "CANNOT OPEN OUTPUT FILE" << endl;
		beta_rescue_status = OpenError;
	} else {
		beta_rescue_status = Ok;
		beta_output_rescue_stream = new QTextStream(beta_rescue);
	}
}

QString FASTQPair::GetFilesName(QString arg_file)
{
	return arg_file.section("/", arg_file.split("/").size() - 1,
							arg_file.split("/").size() - 1);
}

QString FASTQPair::GetFilesPath(QString arg_file)
{
	return (arg_file.section("/", 0, arg_file.split("/").size() - 2) + "/");
}

void FASTQPair::getRead(ReadPaired *result)
{
	alpha->GetRead(&result->alpha);
	beta->GetRead(&result->beta);

	if ( alpha->input_status == FASTQFile::Ok and
		 beta->input_status == FASTQFile::Ok )
		input_status = Ok;
	else if ( alpha->input_status == FASTQFile::EndOfFile and
			  beta->input_status == FASTQFile::EndOfFile)
		input_status = EndOfFiles;
	else
		input_status = Error;
}

void FASTQPair::StatisticsPaired::update(ReadPaired::Result arg_result){
	Total++;
	switch (arg_result) {
		case ReadPaired::Ok:
			Both++;
			break;
		case ReadPaired::AlphaOk:
			OkAlpha++;
			break;
		case ReadPaired::BetaOk:
			OkBeta++;
			break;
		case ReadPaired::Trash:
			Trash++;
	}
}

QString FASTQPair::StatisticsPaired::print(bool arg_rescue){
	QTextStream terminal(stdout);
	terminal.setRealNumberNotation(QTextStream::FixedNotation);
	terminal.setRealNumberPrecision(3);

	QString message = "";

	float total = static_cast<float>(Total);
	float both = static_cast<float>(Both)/total * 100;
	float rescued = static_cast<float>(OkAlpha + OkBeta)/total * 50;
	float trash = 0;
	if (arg_rescue)
		trash = static_cast<float>(Trash)/total * 100;
	else
		trash = static_cast<float>(Total - Both)/total * 100;

	message += "General Stats\n";
	message += "Pairs Analyzed: " + QString::number(Total) + "\t(100.00%)\n";
	message += "Paires Passed:  " + QString::number(Both) + "\t(" +
			QString::number(both, 'f', 2) + "%)\n";
	if (arg_rescue){
		message += "Rescued Reads:  " + QString::number(OkAlpha + OkBeta) + "\t(" +
				QString::number(rescued, 'f', 2) + "%)\n";
		message += "Rejected Pairs: " + QString::number(Trash) + "\t(" +
				QString::number(trash, 'f', 2) + "%)\n";
	} else
		message += "Rejected Pairs: " + QString::number(Total - Both) + "\t(" +
				QString::number(trash, 'f', 2) + "%)\n";

	terminal << message;
	return message;
}
