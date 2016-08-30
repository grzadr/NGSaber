#include "include/readpaired.h"

ReadPaired::ReadPaired(Parameters::LaunchParameters *params): alpha(params), beta(params)
{
}

ReadPaired::~ReadPaired()
{
	if (thread != nullptr){
		thread->waitForFinished();
		delete thread;
	}
}

QPair<bool, bool> ReadPaired::isEmpty()
{
	return QPair<bool, bool>(alpha.isEmpty(), beta.isEmpty());
}

void ReadPaired::clear()
{
	alpha.clear();
	beta.clear();
}

void ReadPaired::print(QTextStream *arg_alpha_stream,QTextStream *arg_beta_stream)
{
	alpha.print(arg_alpha_stream);
	beta.print(arg_beta_stream);
}

ReadPaired::Result ReadPaired::process()
{
	alpha_result = alpha.process();
	beta_result = beta.process();

	if ((alpha_result == ReadSingle::Ok or alpha_result == ReadSingle::OkAfterClever)
		 and (beta_result == ReadSingle::Ok or beta_result == ReadSingle::OkAfterClever))
		return Ok;
	else if (alpha_result == ReadSingle::Ok or alpha_result == ReadSingle::OkAfterClever)
		return AlphaOk;
	else if (beta_result == ReadSingle::Ok or beta_result == ReadSingle::OkAfterClever)
		return BetaOk;
	else
		return Trash;
}

void ReadPaired::printStats()
{
	QTextStream terminal(stdout);
	terminal << "READ ALPHA" << endl;
	alpha.printStats();
	terminal << "READ BETA" << endl;
	beta.printStats();
}
