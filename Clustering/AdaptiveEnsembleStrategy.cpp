#include "AdaptiveEnsembleStrategy.h"

#include <iostream>
#include <fstream>

#include "RandomGreedy.h"

using namespace std;

AdaptiveEnsembleStrategy::AdaptiveEnsembleStrategy(
	int d,
	function<double(double q, int k)> f,
	int k0,
	int l,
	double r,
	ClusteringInterface *final_algorithm)
{
	d_ = d;
	f_ = f;
	k0_ = k0;
	l_ = l;
	r_ = r;
	final_algorithm_ = final_algorithm;
}

AdaptiveEnsembleStrategy::~AdaptiveEnsembleStrategy()
{
}

Partition *AdaptiveEnsembleStrategy::DetectCommunities(Graph *graph,
	Partition *partition,
	double &q)
{
	vector<pair<double, Partition *>> clusterings;

	int k = k0_;

	int k_minus, k_plus;
	double q_minus, q_plus;
	double f_minus, f_plus;

	Partition *tmp_partition = nullptr;

	int alpha = -f_(exp(1), 1);

	char name[60];
	sprintf_s(name, "%s%d%s", "Results/", alpha, ".csv");
	ofstream fout(name);
	fout << "step;k;q;f\n";

	for (int i = 0; i < l_; i++)
	{
		// Minus
		k_minus = max(1, k - d_);
		k_plus = k + d_;

		tmp_partition = RandomGreedy(k_minus).DetectCommunities(graph,
			partition,
			q_minus);
		clusterings.push_back(make_pair(q_minus, tmp_partition));

		tmp_partition = RandomGreedy(k_plus).DetectCommunities(graph,
			partition,
			q_plus);
		clusterings.push_back(make_pair(q_plus, tmp_partition));

		f_minus = f_(q_minus, k_minus);
		f_plus = f_(q_plus, k_plus);

		/*cout << k << endl;
		cout << "-  " << k_minus << " " << q_minus << " " << f_minus << endl;
		cout << "+  " << k_plus << " " << q_plus << " " << f_plus << endl;*/
		fout << i * 2 + 1 << ";" << k_minus << ";" << q_minus << ";" << f_minus << endl;
		fout << i * 2 + 2 << ";" << k_plus << ";" << q_plus << ";" << f_plus << endl;

		k = round(max(1.0, k - (f_plus - f_minus) / (k_plus - k_minus)));
	}

	sort(clusterings.begin(), clusterings.end(),
		[](pair<double, Partition *> &i, pair<double, Partition *> &j) -> bool
	{
		return i.first < j.first;
	}
	);

	fout.close();

	int l2 = ceil(2 * r_ * l_);

	int i;
	for (i = 0; i < 2 * l_ - l2; i++)
	{
		delete clusterings[i].second;
	}

	Partition *overlapping;
	overlapping = clusterings[i++].second;

	for ( ; i < 2 * l_ - 1; i++)
	{
		tmp_partition = Partition::OverlapClusters(overlapping,
			clusterings[i].second,
			graph->VertexCount());
		delete clusterings[i].second;
		delete overlapping;
		overlapping = tmp_partition;
	}
	//delete clusterings[0].second;

	tmp_partition = Partition::OverlapClusters(overlapping,
		clusterings[2 * l_ - 1].second,
		graph->VertexCount());
	delete overlapping;
	overlapping = tmp_partition;

	Partition *result_partition = final_algorithm_->DetectCommunities(graph,
		overlapping,
		q);
	delete overlapping;
	
	if (q < clusterings.back().first)
	{
		delete result_partition;
		q = clusterings.back().first;
		return clusterings.back().second;
	}

	delete clusterings.back().second;
	return result_partition;
}