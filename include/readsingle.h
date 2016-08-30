#ifndef READSINGLE_H
#define READSINGLE_H


#include "include/parameters.h"

#include <QTextStream>
#include <QString>
#include <QVector>
#include <QFuture>

class ReadSingle
{
public:

	struct Unit{
		int first = -1;
		int last = -1;
		int length = 0;
		int good = 0;
		int trash = 0;
		int trash_own = 0;
		int ave_p = 0;
		int ave_n = 0;

		bool operator < ( const Unit& ptr ){
			return -length < -ptr.length;
		}

		void print(){
			QTextStream terminal(stdout);
			terminal << first << "\t" << last << "\t" << length << "\t"
					 << good << "\t" << trash << endl;
		}
	};

	enum Result{
		Ok = 0,
		TooShort = 1,
		LowQuality = 2,
		ZeroQuality = 3,
		TooShortForClever = 4,
		NoUnits = 5,
		OkAfterClever = 6,
		TooShortAfterClever = 7
	};

	enum Mode{
		None = 0,
		Total,
		Fast,
		CUDA
	};

	ReadSingle(Parameters::LaunchParameters *params);
	~ReadSingle();

	QString name = "";
	QString seq = "";
	QString plus = "";
	QString quality = "";

	int trash = 0;
	float ratio = 0;

	QVector<Unit> units;

	QFuture<Result> *thread = nullptr;

	bool isEmpty();
	int size();

	void clear();
	void trim( int arg_first, int arg_last );

	void print( QTextStream *arg_output);
	void printUnits();
	void printStats();

	Result process();

private:

	float prm_min_ratio;

	int prm_quality_opt;
	int prm_quality_trash;
	int prm_quality_phred;

	int prm_min_length;

	int prm_cut_first;
	int prm_cut_last;

	Mode prm_mode_clever = None;
	bool prm_mode_rescue = 0;
	bool prm_mode_verbose = 0;

	void getRatio(bool mode = 0);

	Result clever();
	Result cleverFast();
	Result cleverTotal();
	Result cleverCUDA();
};

#endif // READSINGLE_H
