#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <include/parameters.h>
#include <include/fastqpair.h>

#include <QObject>
#include <QFuture>
#include <QElapsedTimer>

class Launcher : public QObject
{
		Q_OBJECT
	private:
		Parameters::LaunchParameters *params = nullptr;

		void ClearVector(QVector<ReadSingle*> *arg_vector);
		void ClearVector(QVector<ReadPaired*> *arg_vector);

		void HandleError(FASTQFile::Error *arg_error);
		void HandleError(FASTQFile::Error *arg_alpha_error,
						 FASTQFile::Error*arg_beta_error);

	public:
		enum Result{
			Ok = 0,
			Canceled = 1,
			Error = 2,
			OpenError = 3
		};

		struct Stats{
				bool singlefinished = false;
				int singlefilestotal = 0;
				int singlefilescurrent = 0;
				qint64 singleretrieved = 0;
				qint64 singlerejected = 0;
				qint64 singletotal = 0;

				int pairedfilestotal = 0;
				int pairedfilescurrent = 0;
				qint64 pairedretrieved = 0;
				qint64 pairedrejected = 0;
		} stats;

		explicit Launcher(QObject *parent = 0,
						  Parameters::LaunchParameters *arg_params = nullptr,
						  bool *arg_mStop = nullptr);
		bool *mStop = nullptr;
		QElapsedTimer elapsed_time;

		Result process();
		Result processSingle();
		Result processPaired();

	signals:
		void finished();
		void message(QString arg_string);
		void error();
		void canceled();

	public slots:
};

#endif // LAUNCHER_H
