#include "include/fastqfile.h"
#include "include/parameters.h"

#include <QDir>
#include <QFileInfo>
#include <QWeakPointer>
#include <QDebug>

FASTQFile::FASTQFile( QString arg_input_name,
					  QString arg_output_dir,
					  QString arg_output_prefix )
{
	input_name = arg_input_name;

	openInput();

	if (input_status != Ok)
		throw 2;

	output_prefix = arg_output_prefix;

	if(arg_output_dir.isNull())
		output_dir = GetFilesPath(arg_input_name);
	else
		output_dir = arg_output_dir;

	if (!QDir(output_dir).exists())
		QDir().mkdir(output_dir);

	openOutput();

	if (output_status != Ok)
		throw 2;

	error.name = &input_name;
	error.status = &input_status;
	error.line = &last_line;
}

FASTQFile::~FASTQFile(){

	delete input_stream;
	input_file->close();
	delete input_file;

	delete output_stream;
	output_file->close();
	delete output_file;
}

QString FASTQFile::GetFilesPath(QString arg_file)
{
	if ( !arg_file.contains("/") )
		return "./";
	else
		return (arg_file.section( "/", 0, arg_file.split("/").size() - 2) + "/");
}

QString FASTQFile::GetFilesName(QString arg_file)
{
	return arg_file.section("/", arg_file.split("/").size() - 1,
							arg_file.split("/").size() - 1);
}

void FASTQFile::GetRead(ReadSingle *rst_read )
{
	QTextStream terminal(stdout);

	rst_read->clear();

	if (input_status == EndOfFile){
		rst_read->clear();
		return;
	}

	for (int i = 0; i < 4; ++i){
		++last_line;

		if ( !input_stream->atEnd()){
			QString line = input_stream->readLine();

			if (line.isEmpty() and (i == 0 or i == 2)){
					terminal << "i = " << i << endl;
					terminal << "ERROR, line is empty" << endl;
					input_status = EmptyLine;
					rst_read->clear();
					return;
			}

			switch(i){
				case 0:
					if (line.at(0) == '@'){
						rst_read->name = line;

						if (line.size() < 2)
							input_status = MissingName;

					} else if (line.at(0) != '@') {
						input_status = MissingAt;
						rst_read->clear();
						return;
					}
					break;
				case 1:
					rst_read->seq = line;
					break;
				case 2:
					if (line.at(0) == '+'){
						rst_read->plus = line;
					} else if (line.at(0) != '+'){
						input_status = MissingPlus;
						rst_read->clear();
						return;
					}
					break;
				case 3:
					rst_read->quality = line;
					break;
				default:
					break;
			}

		} else {
			if (rst_read->name.size() or rst_read->seq.size() or rst_read->plus.size()
				or rst_read->quality.size())
				input_status =  EmptyLine;
			else
				input_status = EndOfFile;

			rst_read->clear();
			return;
		}
	}

	if(rst_read->seq.size() != rst_read->quality.size()){
		terminal << "Different length of nucleotide sequecne and quality sequence"
				 << endl;
		input_status = DifferentLength;
		rst_read->clear();
	}
}

void FASTQFile::openInput()
{
	QTextStream terminal( stdout );

	input_file = new QFile(input_name);

	if (!input_file->open(QIODevice::ReadOnly | QIODevice::Text)){
		terminal << "### FILE ERROR ###" << endl
				 << "FILE NAME: " << input_name << endl
				 << "CANNOT OPEN INPUT FILE" << endl;
		input_status = OpenError;
	} else {
		input_status = Ok;
		input_stream = new QTextStream(input_file);
	}
}

void FASTQFile::openOutput()
{
	QTextStream terminal(stdout);

	output_name = output_dir + output_prefix + GetFilesName(input_name);

	output_file = new QFile(output_name);

	if (!output_file->open( QIODevice::WriteOnly | QIODevice::Text)){
		terminal << "### FILE ERROR ###" << endl
				 << "OUTPUT FILE NAME: " << output_name << endl
				 << "CANNOT OPEN OUTPUT FILE" << endl;
		output_status = OpenError;
	} else {
		output_status = Ok;
		output_stream = new QTextStream(output_file);
	}
}

QString FASTQFile::Statistics::print(bool arg_clever){
	QTextStream terminal(stdout);

	QString message = "";

	float total = static_cast<float>(Total);
	int totalgood = GoodQuality;
	int totalshort = TooShort;
	int totallow = LowQuality;

	if (arg_clever){
		totalgood += OkAfterClever;
		totalshort += TooShortAfterClever;
		totallow += ZeroQuality + TooShortForClever;
	}

	float good = static_cast<float>(totalgood)/total*100;
	float tooshort = static_cast<float>(totalshort)/total*100;
	float low = static_cast<float>(totallow)/total*100;

	message += "All Reads:     " + QString::number(Total) + "\t(100.00%)\n";
	message += "Reads Rescued: " + QString::number(totalgood) + "\t(" +
			QString::number(good, 'f', 2) + ")%\n";
	message += "Too Short:     " + QString::number(totalshort) + "\t(" +
			QString::number(tooshort, 'f', 2) + ")%\n";
	message += "Low Quality:   " + QString::number(totallow) + "\t(" +
			QString::number(low, 'f', 2) + ")%\n";

	terminal << message << endl;
	return message;
}

void FASTQFile::Statistics::update(ReadSingle::Result arg_result){
	Total++;
	switch (arg_result){
		case ReadSingle::Ok:
			GoodQuality++;
			break;
		case ReadSingle::TooShort:
			TooShort++;
			break;
		case ReadSingle::LowQuality:
			LowQuality++;
			break;
		case ReadSingle::ZeroQuality:
			ZeroQuality++;
			break;
		case ReadSingle::TooShortForClever:
			TooShortForClever++;
			break;
		case ReadSingle::OkAfterClever:\
			OkAfterClever++;
			break;
		case ReadSingle::TooShortAfterClever:
			TooShortAfterClever++;
			break;
		default:
			break;
	}
}
