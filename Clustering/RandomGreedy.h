#pragma once

//#include <vld.h>

#include <vector>
#include <utility>

#include <boost/unordered_map.hpp>

#include "ClusteringInterface.h"
#include "../ActiveElements.h"

class RandomGreedy :
	public ClusteringInterface
{

public:

	RandomGreedy(int k);
	~RandomGreedy();
	Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) override;

private:

	void FindJoin(int max_sample,
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

	int k_;

	Graph *graph_;
	std::vector<boost::unordered_map<int, double> *> *rows_; // Matrix E
	std::vector<double> *row_sums_;	// Vector A
	ActiveElements *active_elements_;

};

