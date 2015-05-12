#include "AdaptiveEnsembleStrategy.h"

#include <iostream>

#include "RandomGreedy.h"

using namespace std;

AdaptiveEnsembleStrategy::AdaptiveEnsembleStrategy(
	int d,
	function<double(double q)> f,
	int k0,
	int l,
	ClusteringInterface *final_algorithm)
{
	d_ = d;
	f_ = f;
	k0_ = k0;
	l_ = l;
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

	for (int i = 0; i < l_; i++) // TODO
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
		
		f_minus = f_(q_minus);
		f_plus = f_(q_plus);

		/*cout << k << endl;
		cout << "-  " << k_minus << " " << q_minus << " " << f_minus << endl;
		cout << "+  " << k_plus << " " << q_plus << " " << f_plus << endl;*/

		k = round(max(1.0, k - (f_plus - f_minus) / (k_plus - k_minus)));
	}

	sort(clusterings.begin(), clusterings.end(),
		[](pair<double, Partition *> &i, pair<double, Partition *> &j) -> bool
		{
			return i.first < j.first;
		}
	);

	if (clusterings.size() == 1)
	{
		q = clusterings[0].first;
		return clusterings[0].second;
	}

	int i;
	for (i = 0; i < clusterings.size() / 2 - 1; i++)
		delete clusterings[i].second;

	Partition *overlapping = clusterings[i++].second;

	for ( ; i < clusterings.size() - 1; i++)
	{
		tmp_partition = Partition::OverlapClusters(overlapping,
			clusterings[i].second,
			graph->VertexCount());
		delete clusterings[i].second, overlapping;
		overlapping = tmp_partition;
	}
	
	tmp_partition = Partition::OverlapClusters(overlapping,
		clusterings[i].second,
		graph->VertexCount());
	delete overlapping;
	overlapping = tmp_partition;

	Partition *result_partition = final_algorithm_->DetectCommunities(graph,
		overlapping,
		q);

	if (q < clusterings.back().first)
	{
		delete result_partition;
		q = clusterings.back().first;
		return clusterings.back().second;
	}
	
	delete clusterings.back().second;
	return result_partition;
}