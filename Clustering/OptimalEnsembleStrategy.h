#pragma once

#include <utility>
#include <functional>

#include "ClusteringInterface.h"

class OptimalEnsembleStrategy :
	public ClusteringInterface
{

public:

	OptimalEnsembleStrategy(
		int d,
		std::function<double(double q)> f,
		int k0,
		int s0,
		int l);
	~OptimalEnsembleStrategy();

	Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) override;

private:
	
	Graph *graph_;
	int d_;
	std::function<double(double q)> f_;
	int k0_;
	int s0_;
	int l_;

};

