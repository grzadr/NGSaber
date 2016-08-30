#include "include/readsingle.h"

#include <QDebug>
#include <math.h>
//#include <QElapsedTimer>

ReadSingle::ReadSingle( Parameters::LaunchParameters *params)
{
	prm_min_ratio = params->min_ratio;

	prm_quality_opt = params->quality_opt;
	prm_quality_trash = params->quality_trash;
	prm_quality_phred = params->quality_phred;

	prm_min_length = params->min_length;

	prm_cut_first = params->cut_first;
	prm_cut_last = params->cut_last;

	prm_mode_clever = static_cast<Mode>(params->mode_clever);
	prm_mode_rescue = params->mode_rescue;
	prm_mode_verbose = params->mode_verbose;
}

ReadSingle::~ReadSingle()
{
	units.clear();
	if (thread != nullptr){
		thread->waitForFinished();
		delete thread;
	}
}

bool ReadSingle::isEmpty()
{
	if (name.isEmpty()) return 1;
	else if (seq.isEmpty()) return 1;
	else if (plus.isEmpty()) return 1;
	else if (quality.isEmpty()) return 1;
	else return 0;
}

void ReadSingle::clear()
{
	name.clear();
	seq.clear();
	plus.clear();
	quality.clear();

	trash = 0;
	ratio = 0;

	units.clear();
}
void ReadSingle::trim(int arg_first, int arg_last)
{
	if (arg_first < seq.size() and arg_last < seq.size()){
		seq = seq.mid(arg_first, arg_last - arg_first + 1);
		quality = quality.mid(arg_first, arg_last - arg_first + 1);
	} else
		clear();
}

int ReadSingle::size()
{
	return seq.size();
}

void ReadSingle::print(QTextStream *arg_output)
{
	*arg_output << name << endl;
	*arg_output << seq << endl;
	*arg_output << plus << endl;
	*arg_output << quality << endl;
}

void ReadSingle::printUnits()
{
	QTextStream terminal(stdout);

	terminal.setFieldWidth(8);
	terminal << "ID" << "FIRST" << "LAST" << "LENGTH" << "GOOD" << "FATAL" << "FATAL_O"
			 << "AVE_P" << "AVE_N" << endl;

	int i = 0;
	for (Unit unit : units){
		terminal << i << unit.first << unit.last << unit.length << unit.good
				 << unit.trash << unit.trash_own << unit.ave_p << unit.ave_n << endl;
		++i;
	}
}

void ReadSingle::printStats()
{
	QTextStream terminal(stdout);

	terminal << "NAME: " << name << endl;
	terminal << "SEQ:  " << seq << endl;
	terminal << "PLUS: " << plus << endl;
	terminal << "QUAL: " << quality << endl;
	terminal << "LENGHT: " << size() << endl;
	terminal << "RATIO: " << ratio << endl;
	terminal << endl;
}

ReadSingle::Result ReadSingle::process()
{
	trim(prm_cut_first, size() - prm_cut_last - 1);

	if (size() < prm_min_length or !size())
		return TooShort;

	getRatio();

	if (ratio < prm_min_ratio){
		if (prm_mode_clever)
			return clever();
		else
			return LowQuality;
	} else {
		if (trash and prm_mode_clever)
			return clever();
		else
			return Ok;
	}
}

void ReadSingle::getRatio(bool mode)
{
	if (!prm_mode_clever or mode){

		int sum = 0;

		for (auto c : quality){
			int q = c.unicode() - prm_quality_phred;
			if (q >= prm_quality_opt) ++sum;
		}

		ratio = static_cast<float>(sum)/static_cast<float>(quality.size());
	} else {
		int sum = 0;
		int first = -1;
		int last = -1;
		int fatal = 0;
		int average = 0;

		if(units.size())
			units.clear();

		for (int i = 0; i < quality.size(); ++i){
			int q = quality.at(i).unicode() - prm_quality_phred;

			if (q >= prm_quality_opt){
				if (first == -1)
					first = i;
				last = i;
			} else if (q < prm_quality_opt){
				if (first != -1){
					Unit temp_unit;

					temp_unit.first = first;
					temp_unit.last = last;
					temp_unit.length = last - first + 1;
					temp_unit.good = sum;
					temp_unit.trash = fatal;
					temp_unit.ave_p = average;

					if (units.size()){
						if (units.last().trash == temp_unit.trash)
							units.last().ave_n = average;
						units.last().trash_own = fatal - units.last().trash;
					}

					units.append(temp_unit);

					sum += temp_unit.length;
					first = -1;
					average = 0;
				}

				if (q <= prm_quality_trash){
					if (units.size() and units.last().trash == fatal)
						units.last().ave_n = average;

					average = 0;
					++fatal;
				} else
					average += 1;
			}
		}

		if (units.size()){
			if (units.last().trash == fatal)
				units.last().ave_n = average;
			units.last().trash_own = fatal - units.last().trash;
		}


		if (first != -1){
			Unit temp_unit;

			temp_unit.first = first;
			temp_unit.last = last;
			temp_unit.length = last - first + 1;
			temp_unit.good = sum;
			temp_unit.trash = fatal;
			temp_unit.ave_p = average;
			temp_unit.ave_n = 0;

			units.append(temp_unit);
			sum += temp_unit.length;
		}

		trash = fatal;
		ratio = static_cast<float>(sum)/static_cast<float>(quality.size());
	}
}

ReadSingle::Result ReadSingle::clever()
{
	if (!units.size())
		return LowQuality;

//	if (ratio == 0)
//		return ZeroQuality;

	if (static_cast<int>(ratio * static_cast<float>(size())) <
		 static_cast<int>(static_cast<float>(prm_min_length) * prm_min_ratio))
		return TooShortForClever;

	switch (prm_mode_clever){
		case Fast:
			return cleverFast();
		case Total:
			return cleverTotal();
		case CUDA:
			return cleverCUDA();
		case None:
			return Ok;
	}

	return Ok;
}

ReadSingle::Result ReadSingle::cleverFast()
{
//	QTextStream terminal(stdout);

//	std::sort(units.begin(), units.end());

//	QVectorIterator<Unit> ite_units(units);

//	Unit best_left = ite_units.next();
//	Unit best_right = best_left;
//	int best_length = best_right.last - best_left.first + 1;
//	float best_ratio = 1;

//	int temp_first = 0;
//	int temp_last = 0;
//	int temp_length = 0;
//	int temp_good = 0;
//	float temp_ratio = 1;

//	while(ite_units.hasNext()){

//		Unit temp_unit = ite_units.next();

//		if (temp_unit.first >= best_left.first and temp_unit.last <= best_right.last)
//			continue;

//		if (temp_unit.trash != best_right.trash) continue;

//		if (temp_unit.first < best_left.first){
//			temp_first = temp_unit.first;
//			temp_last = best_right.last;
//			temp_good = best_right.good - temp_unit.good + best_right.length;
//		} else {
//			temp_first = best_left.first;
//			temp_last = temp_unit.last;
//			temp_good = temp_unit.good - best_left.good + temp_unit.length;
//		}

//		temp_length = temp_last - temp_first + 1;

//		temp_ratio = static_cast<float>(temp_good)/static_cast<float>(temp_length);
//		if (temp_ratio < prm_min_ratio) continue;

//		if (temp_unit.first > best_right.last)
//			best_right = temp_unit;
//		else
//			best_left = temp_unit;

//		best_length = temp_length;
//		best_ratio = temp_ratio;
//	}

//	trim(best_left.first, best_right.last);
//	ratio = best_ratio;
//	trash = 0;

//	if (size() < prm_min_length)
//		TooShort;
//	if (ratio < prm_min_ratio)
//		LowQuality;

	return OkAfterClever;

}

ReadSingle::Result ReadSingle::cleverTotal()
{

	Unit best_read;
	Unit first, last;

	for (int ite_major = 0; ite_major < units.size(); ++ite_major){

		Unit major_unit = units.at(ite_major);

		for (int ite_minor = units.size() - 1; ite_minor >= ite_major; --ite_minor){

			Unit minor_unit = units.at(ite_minor);

			if (major_unit.first >= best_read.first and minor_unit.last <= best_read.last)
				break;

			if (major_unit.trash != minor_unit.trash){
				if (major_unit.trash_own)
					ite_minor = ite_major + 1;
				continue;
			}

			int temp_length = minor_unit.last - major_unit.first + 1;
			int temp_length_max = temp_length + major_unit.ave_p + minor_unit.ave_n;

			if (temp_length_max < prm_min_length or temp_length < best_read.length)
				break;

			int temp_good = minor_unit.good - major_unit.good + minor_unit.length;

			float temp_ratio =
					static_cast<float>(temp_good)/static_cast<float>(temp_length);

			if (temp_ratio < prm_min_ratio)
				continue;

			best_read.first = major_unit.first;
			best_read.last = minor_unit.last;
			best_read.length = temp_length;
			best_read.good = temp_good;
			first = major_unit;
			last = minor_unit;
		}
	}

	int add = static_cast<int>(static_cast<float>(best_read.good)/prm_min_ratio - static_cast<float>(best_read.length));

	if (add){
		add -= last.ave_n;

		best_read.length += last.ave_n;
		best_read.last += last.ave_n;

		if (add < 0){
			best_read.length += add;
			best_read.last += add;
		}
	}

	if (add > 0){
		add -= first.ave_p;

		best_read.length += first.ave_p;
		best_read.first -= first.ave_p;

		if (add < 0){
			best_read.length += add;
			best_read.first -= add;
		}
	}

	trim(best_read.first, best_read.last);

	ratio = static_cast<float>(best_read.good)/static_cast<float>(size());
	trash = 0;

	if (size() < prm_min_length)
		return TooShort;
	if (ratio < prm_min_ratio)
		return LowQuality;

	return Ok;
}

ReadSingle::Result ReadSingle::cleverCUDA()
{
	return Ok;
}
