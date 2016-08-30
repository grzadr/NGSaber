#include "include/launcher.h"

#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

Launcher::Launcher(QObject *parent, Parameters::LaunchParameters *arg_params,
				   bool *arg_mStop) : QObject(parent)
{
	params = arg_params;
	if (params->mode_verbose)
		params->print();

	if (arg_mStop != nullptr)
		mStop = arg_mStop;
	else
		mStop = new bool(false);

	stats.singlefilestotal = params->input_single.size();
	stats.pairedfilestotal = params->input_alpha.size();
}

Launcher::Result Launcher::process()
{
	QTextStream terminal(stdout);
	elapsed_time.start();

	Result result = Ok;
	if (params->input_single.size())
		result = processSingle();

	if (!result and params->input_alpha.size()){
		stats.singlefinished = true;
		result = processPaired();
	}

	qint64 temp_elapsed = (elapsed_time.elapsed())/1000;
	QString seconds = QString::number(temp_elapsed%60);
	QString minutes = QString::number((temp_elapsed/60)%60);
	QString hours = QString::number(temp_elapsed/3600);
	QString time = QString("Analyzis took %1 hours, %2 minutes and %3 seconds")
			.arg(hours, minutes, seconds);
	 terminal << time << endl;

	emit finished();
	return result;
}

Launcher::Result Launcher::processSingle()
{
	QTextStream terminal(stdout);

	if (params->mode_verbose)
		terminal << "Processing Single Read Files" << endl;

	for(auto input_name : params->input_single){
		FASTQFile *input_fastq = nullptr;

		try{
			input_fastq =
					new FASTQFile(input_name, params->output_dir, params->output_prefix);
		} catch (int e){
			HandleError(&input_fastq->error);
			emit error();
			delete input_fastq;
			return OpenError;
		}

		if (params->mode_verbose){
			terminal << "Processing:\n" + input_fastq->input_name << endl;
			terminal << "Saving to:\n" + input_fastq->output_name << endl;
			terminal << endl;
		}

		emit message("Processing:\n" + input_fastq->input_name);
		emit message("Saving to:\n" + input_fastq->output_name);
		emit message("");
		qInfo() << "Reads analysed\tRetrieved\tFiltered\tRatio";

		QVector<ReadSingle*> reads;

		for (int idt = 0; idt < params->threads; ++idt){
			ReadSingle *temp_read = new ReadSingle(params);
			input_fastq->GetRead(temp_read);

			if (input_fastq->input_status == FASTQFile::Ok){
				temp_read->thread = new QFuture<ReadSingle::Result>;
				*temp_read->thread = QtConcurrent::run(temp_read, &ReadSingle::process);
				reads.append(temp_read);
			} else if (input_fastq->input_status == FASTQFile::EndOfFile) {
//				terminal << "END OF FILE" << endl;
//				emit message("END OF FILE");
				break;
			} else {
				terminal << "ERROR" << endl;
				ClearVector(&reads);
				HandleError(&input_fastq->error);
				emit error();
				return Error;
			}
		}

		while (!reads.isEmpty()){
			if (*mStop){
				terminal << "CANCELED" << endl;
				emit canceled();
				ClearVector(&reads);
				delete input_fastq;
				return Canceled;
			}

			ReadSingle *temp_read = reads.takeFirst();
			if (temp_read->thread->isRunning()){
				reads.append(temp_read);
				continue;
			}

			ReadSingle::Result result = temp_read->thread->result();

			if (temp_read->thread->result() == ReadSingle::Ok){
				stats.singleretrieved++;
				temp_read->print(input_fastq->output_stream);
			} else
				stats.singlerejected++;

			stats.singletotal++;

			if (params->mode_verbose and
					!((stats.singlerejected + stats.singleretrieved)%250000)){
				terminal.setFieldWidth(12);
				terminal << stats.singlerejected + stats.singleretrieved
						 << stats.singleretrieved
						 << stats.singlerejected
						 << 100*static_cast<float>(stats.singleretrieved)/static_cast<float>(stats.singletotal)

						 << endl;
			}

			input_fastq->stats.update(result);

			delete temp_read;

			ReadSingle *new_read = new ReadSingle(params);
			input_fastq->GetRead(new_read);

			if (input_fastq->input_status == FASTQFile::Ok){
				new_read->thread = new QFuture<ReadSingle::Result>;
				*new_read->thread = QtConcurrent::run(new_read, &ReadSingle::process);
				reads.append(new_read);
			} else {
				delete new_read;
				if (input_fastq->input_status == FASTQFile::EndOfFile) {
//					terminal << "END OF FILE" << endl;
					continue;
				} else {
					terminal << "ERROR" << endl;
					ClearVector(&reads);
					HandleError(&input_fastq->error);
					emit error();
					return Error;
				}
			}
		}

		emit message(input_fastq->stats.print(params->mode_clever));
		stats.singlefilescurrent++;
		delete input_fastq;
	}
	return Ok;
}

Launcher::Result Launcher::processPaired()
{
	QTextStream terminal(stdout);
	if (params->mode_verbose)
		terminal << "Staritng Paired Reads Analyzis" << endl << endl;

	for (int idx = 0; idx < params->input_alpha.size(); ++idx){
		QString alpha_name = params->input_alpha.at(idx);
		QString beta_name = params->input_beta.at(idx);

		FASTQPair *input_fastq = nullptr;

		try{
			input_fastq = new FASTQPair(alpha_name, beta_name, params->output_dir,
										params->output_prefix, params->mode_rescue);
		}
		catch (int e){
			HandleError(&input_fastq->alpha->error, &input_fastq->beta->error);
			emit error();
			delete input_fastq;
			return OpenError;
		}

		if (params->mode_verbose){
			terminal << "Processing:" << endl;
			terminal << "\t" << input_fastq->alpha->input_name << endl;
			terminal << "\t" << input_fastq->beta->input_name << endl;
			terminal << "Saving to:" << endl;
			terminal << "\t" << input_fastq->alpha->output_name << endl;
			terminal << "\t" << input_fastq->beta->output_name << endl;
			terminal << endl;
		}

		emit message("Processing:");
		emit message("\t" + input_fastq->alpha->input_name);
		emit message("\t" + input_fastq->beta->input_name);
		emit message("Saving to:");
		emit message("\t" + input_fastq->alpha->output_name);
		emit message("\t" + input_fastq->beta->output_name);
		emit message("");

		QVector<ReadPaired*> reads;

		for (int idt = 0; idt < params->threads; ++idt){
			ReadPaired *temp_read = new ReadPaired(params);
			input_fastq->getRead(temp_read);

			if (input_fastq->input_status == FASTQPair::Ok){
				temp_read->thread = new QFuture<ReadPaired::Result>;
				*temp_read->thread = QtConcurrent::run(temp_read, &ReadPaired::process);
				reads.append(temp_read);
			} else if (input_fastq->input_status == FASTQPair::EndOfFiles) {
//				terminal << "END OF FILES" << endl;
//				emit message("END OF FILES");
				break;
			} else {
				terminal << "ERROR" << endl;
				ClearVector(&reads);
				HandleError(&input_fastq->alpha->error, &input_fastq->beta->error);
				emit error();
				return Error;
			}
		}

		while (!reads.isEmpty()){
			if (*mStop){
				terminal << "CANCELED" << endl;
				emit canceled();
				ClearVector(&reads);
				delete input_fastq;
				return Canceled;
			}

			ReadPaired *temp_read = reads.takeFirst();
			if (temp_read->thread->isRunning()){
				reads.append(temp_read);
				continue;
			}

			ReadPaired::Result result = temp_read->thread->result();

			if (result == ReadPaired::Ok){
				stats.pairedretrieved++;
				temp_read->print(input_fastq->alpha_output_stream,
								 input_fastq->beta_output_stream);
			} else {
				if (params->mode_rescue){
					switch (result){
						case ReadPaired::AlphaOk:
							temp_read->alpha.print(
										input_fastq->alpha_output_rescue_stream);
							break;
						case ReadPaired::BetaOk:
							temp_read->beta.print(
										input_fastq->beta_output_rescue_stream);
							break;
						default:
							break;
					}
				}
				stats.pairedrejected++;
			}

			input_fastq->stats.update(result);
			input_fastq->alpha->stats.update(temp_read->alpha_result);
			input_fastq->beta->stats.update(temp_read->beta_result);

			delete temp_read;

			ReadPaired *new_read = new ReadPaired(params);
			input_fastq->getRead(new_read);

			if (input_fastq->input_status == FASTQPair::Ok){
				new_read->thread = new QFuture<ReadPaired::Result>;
				*new_read->thread = QtConcurrent::run(new_read, &ReadPaired::process);
				reads.append(new_read);
			} else {
				delete new_read;
				if (input_fastq->input_status == FASTQPair::EndOfFiles) {
					terminal << "END OF FILE" << endl;
					continue;
				} else {
					terminal << "ERROR" << endl;
					ClearVector(&reads);
					HandleError(&input_fastq->alpha->error, &input_fastq->beta->error);
					emit error();
					return Error;
				}
			}
		}

		terminal << endl;
		emit message(input_fastq->stats.print(params->mode_rescue));
		terminal << endl;
		emit message("Primary File Stats:");
		terminal << "Primary File Stats:" << endl;
		emit message(input_fastq->alpha->stats.print(params->mode_clever));
		emit message("Secondary File Stats:");
		terminal << "Secondary File Stats:" << endl;
		emit message(input_fastq->beta->stats.print(params->mode_clever));
		stats.pairedfilescurrent++;
		delete input_fastq;
	}
	return Ok;
}

void Launcher::ClearVector( QVector<ReadSingle*> *arg_vector){
	while(!arg_vector->isEmpty()){
		ReadSingle *read = arg_vector->takeFirst();
		delete read;
	}
}

void Launcher::ClearVector(QVector<ReadPaired *> *arg_vector)
{
	while(!arg_vector->isEmpty()){
		ReadPaired *read = arg_vector->takeFirst();
		delete read;
	}
}

void Launcher::HandleError(FASTQFile::Error *arg_error)
{
	QTextStream terminal(stdout);
	QString output = "ERROR IN FILE: " + *arg_error->name + "\n";
	switch(*arg_error->status){
		case FASTQFile::Ok:
			output = "EVERYTHING IS OK WITH " + *arg_error->name + "\n";
			break;
		case FASTQFile::OpenError:
			output += "UNABLE TO OPEN FILE\n";
			break;
		case FASTQFile::EmptyLine:
			output += "EMPTY STRING IN LINE " + QString::number(*arg_error->line) + "\n";
			break;
		case FASTQFile::MissingAt:
			output += "MISSING @ IN LINE " + QString::number(*arg_error->line) + "\n";
			break;
		case FASTQFile::MissingName:
			output += "MISSING NAME IN LINE " + QString::number(*arg_error->line) + "\n";
			break;
		case FASTQFile::MissingPlus:
			output += "MISSING + IN LINE " + QString::number(*arg_error->line) + "\n";
			break;
		case FASTQFile::DifferentLength:
			output += "DIFFERENT LENGTHS OF SEQUENCE AND QUALITY STRING IN LINE " +
					QString::number(*arg_error->line) + "\n";
			break;
		default:
			break;
	}
	terminal << output << endl;
	emit message(output + "\n");
}

void Launcher::HandleError(FASTQFile::Error *arg_alpha_error, FASTQFile::Error *arg_beta_error)
{
	HandleError(arg_alpha_error);
	HandleError(arg_beta_error);
}
