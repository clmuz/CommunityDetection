#pragma once

#include <functional>

#include "ClusteringInterface.h"

class AdaptiveEnsembleStrategy :
	public ClusteringInterface
{

public:

	AdaptiveEnsembleStrategy(
		int d,
		std::function<double(double q)> f,
		int k0,
		int l,
		ClusteringInterface *final_algorithm);
	~AdaptiveEnsembleStrategy();

	Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) override;

private:

	int d_;
	std::function<double(double q)> f_;
	int k0_;
	int l_;
	ClusteringInterface *final_algorithm_;

};

