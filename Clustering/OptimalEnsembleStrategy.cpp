#include "OptimalEnsembleStrategy.h"

#include <iostream>

#include "EnsembleStrategy.h"
#include "RandomGreedy.h"

using namespace std;

OptimalEnsembleStrategy::OptimalEnsembleStrategy(int d,
	std::function<double(double q)> f,
	int k0,
	int s0,
	int l)
{
	d_ = d;
	f_ = f;
	k0_ = k0;
	s0_ = s0;
	l_ = l;
}


OptimalEnsembleStrategy::~OptimalEnsembleStrategy()
{
}


Partition *OptimalEnsembleStrategy::DetectCommunities(Graph *graph,
	Partition *partition,
	double &q)
{
	double best_q = -1;
	Partition *best_partition = nullptr;

	int k = k0_;
	int s = s0_;

	Partition *tmp_partition = nullptr;
	double q_minus, q_plus;
	double f_minus, f_plus;

	int k_minus, k_plus;
	int s_minus, s_plus;

	//cout << "-----------\n";

	for (int i = 0; i < l_; i++) // TODO
	{
		int delta_k = d_ * (2 * (rand() % 2) - 1);
		k_minus = max(1, k - delta_k);
		k_plus = max(1, k + delta_k);
				
		int delta_s = d_ * (2 * (rand() % 2) - 1);
		s_minus = max(1, s - delta_s);
		s_plus = max(1, s + delta_s);

		// Minus
		RandomGreedy *rg_minus = new RandomGreedy(k_minus);
		tmp_partition = EnsembleStrategy(rg_minus, rg_minus, s_minus, false).
			DetectCommunities(graph, nullptr, q_minus);

		delete rg_minus;

		if (q_minus > best_q)
		{
			best_q = q_minus;
			delete best_partition;
			best_partition = tmp_partition;
		}
		else
			delete tmp_partition;

		// Plus
		RandomGreedy *rg_plus = new RandomGreedy(k_plus);
		tmp_partition = EnsembleStrategy(rg_plus, rg_plus, s_plus, false).
			DetectCommunities(graph, nullptr, q_plus);

		delete rg_plus;

		if (q_plus > best_q)
		{
			best_q = q_plus;
			delete best_partition;
			best_partition = tmp_partition;
		}
		else
			delete tmp_partition;

		// Next

		f_minus = f_(q_minus);
		f_plus = f_(q_plus);

		//cout << k << " " << s << endl;
		//cout << "-  " << k_minus << " " << s_minus << " " << q_minus << " " << f_minus << endl;
		//cout << "+  " << k_plus << " " << s_plus << " " << q_plus << " " << f_plus << endl;

		k = round(max(1.0,
			k - (delta_k / d_) * (f_plus - f_minus) / (abs(k_plus - k_minus))));

		s = round(max(1.0,
			s - (delta_s / d_) * (f_plus - f_minus) / (abs(s_plus - s_minus))));
	}

	q = best_q;
	return best_partition;
}