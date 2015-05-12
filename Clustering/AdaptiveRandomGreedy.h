#pragma once

#include <vector>
#include <utility>

#include <boost/unordered_map.hpp>

#include "ClusteringInterface.h"
#include "../ActiveElements.h"

class AdaptiveRandomGreedy :
	public ClusteringInterface
{

public:

	AdaptiveRandomGreedy(int d,
		std::function<double(double median, int k)> f,
		int s,
		int k0,
		bool cumulative);
	~AdaptiveRandomGreedy();
	Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) override;

private:

	void FindJoin(int max_sample,
		int step,
		double &max_delta_q,
		int &join_a,
		int &join_b);
	void JoinRows(int &a, int &b);
	void Init();
	void Init(Partition *partition);
	std::vector<std::pair<int, int> > *PerformJoins(double &q, int &best_step);
	void Clear();
	Partition *CreatePartitionFromJoins(
		Partition *start_partition,
		std::vector<std::pair<int, int>> &joins,
		int until_step);

	int s_;
	int k0_;
	int d_;
	std::function<double(double delta_q, int k)> f_;
	bool cumulative_;

	Graph *graph_;
	std::vector<boost::unordered_map<int, double> *> *rows_; // Matrix E
	std::vector<double> *row_sums_;	// Vector A
	ActiveElements *active_elements_;

};

