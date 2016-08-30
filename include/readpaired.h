#ifndef READPAIRED_H
#define READPAIRED_H

#include "include/readsingle.h"
#include "include/parameters.h"

#include <QPair>

class ReadPaired
{
public:
	ReadSingle alpha;
	ReadSingle beta;

	enum Result{
		Ok = 0,
		AlphaOk,
		BetaOk,
		Trash
	};

	QFuture<Result> *thread = nullptr;

	ReadPaired(Parameters::LaunchParameters *params);
	~ReadPaired();

	QPair<bool, bool> isEmpty();
	void clear();
	void print(QTextStream *arg_alpha_stream, QTextStream *arg_beta_stream);
	Result process();
	void printStats();

	ReadSingle::Result alpha_result = ReadSingle::Ok;
	ReadSingle::Result beta_result = ReadSingle::Ok;

};

#endif // READPAIRED_H
