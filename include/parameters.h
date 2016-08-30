#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

class Parameters
{
public:
	enum Result{
		Ok = 0,
		Error = 1
	};

	enum Mode{
		None = 0,
		Total = 1,
		Fast = 2,
		CUDA = 3
	};

	struct LaunchParameters{
		QStringList input_single;
		QStringList input_alpha;
		QStringList input_beta;

		QString output_prefix = "CLEARED_";
		QString output_dir = QString();

		int threads = 1;

		float min_ratio = 0.75;

		int min_length = 0;

		int quality_opt = 30;
		int quality_trash = 10;
		int quality_phred = 33;

		int cut_first = 0;
		int cut_last = 0;

		Mode mode_clever = None;
		bool mode_rescue = 0;
		bool mode_verbose = 0;

		void print();
	} launch_parameters;

	Parameters(QApplication *arg_a);
	~Parameters();

	QCommandLineParser parser;

	Result processArguments();
};

#endif // PARAMETERS_H
