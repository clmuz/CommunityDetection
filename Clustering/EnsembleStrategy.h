#pragma once

#include <vector>

#include "ClusteringInterface.h"
#include "ClusteringInterface.h"

class EnsembleStrategy :
	public ClusteringInterface
{

public:
	
	EnsembleStrategy(ClusteringInterface *initial_algorithm,
		ClusteringInterface *final_algorithm,
		int initial_steps,
		bool iterative);

	~EnsembleStrategy();

	Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) override;

private:

	Partition *DetectCommunitiesIterative(Graph *graph,
		Partition *partition,
		double &q);

	ClusteringInterface *initial_algorithm_;
	ClusteringInterface *final_algorithm_;
	int initial_steps_;
	bool iterative_;

};

