#include "EnsembleStrategy.h"

#include <algorithm>
#include <iostream>

using namespace std;

EnsembleStrategy::EnsembleStrategy(
	ClusteringInterface *initial_algorithm,
	ClusteringInterface *final_algorithm,
	int initial_steps,
	bool iterative)
{
	initial_algorithm_ = initial_algorithm;
	final_algorithm_ = final_algorithm;
	initial_steps_ = initial_steps;
	iterative_ = iterative;
}


EnsembleStrategy::~EnsembleStrategy()
{
}


Partition *EnsembleStrategy::DetectCommunities(Graph *graph,
	Partition *partition,
	double &q)
{
	if (iterative_)
		return DetectCommunitiesIterative(graph, partition, q);

	double tmp_q;

	Partition *initial_clustering = initial_algorithm_->
		DetectCommunities(graph, partition, tmp_q);
	for (int i = 1; i < initial_steps_; i++)
	{
		Partition *tmp_clustering = initial_algorithm_->
			DetectCommunities(graph, partition, tmp_q);

		Partition *next_clustering = Partition::OverlapClusters(
			initial_clustering,	tmp_clustering, graph->VertexCount());

		delete tmp_clustering;
		delete initial_clustering;
		initial_clustering = next_clustering;
	}

	Partition *result = final_algorithm_->DetectCommunities(graph,
		initial_clustering, q);
	delete initial_clustering;

	return result;
}

Partition *EnsembleStrategy::DetectCommunitiesIterative(Graph *graph,
	Partition *partition,
	double &q)
{
	double tmp_q;
	double prev_q = -1;

	Partition *previous_partition = partition;

	while (true)
	{
		Partition *initial_clustering = initial_algorithm_->
			DetectCommunities(graph, previous_partition, tmp_q);

		for (int i = 1; i < initial_steps_; i++)
		{
			Partition *tmp_clustering = initial_algorithm_->
				DetectCommunities(graph, previous_partition, tmp_q);

			Partition *next_clustering = Partition::OverlapClusters(
				initial_clustering, tmp_clustering, graph->VertexCount());
				
			delete tmp_clustering;
			delete initial_clustering;
			initial_clustering = next_clustering;
		}

		//cout << best_q << " " << prev_q << endl;
		double current_q = initial_clustering->Modularity(graph);

		if (current_q > prev_q)
		{
			prev_q = current_q;

			if (previous_partition != partition)
				delete previous_partition;

			previous_partition = initial_clustering;
			continue;
		}

		delete initial_clustering;
		Partition *result = final_algorithm_->DetectCommunities(graph,
			previous_partition, q);
		
		if (previous_partition != partition)
			delete previous_partition;

		return result;
	}
}
