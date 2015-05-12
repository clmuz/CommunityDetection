#pragma once

#include <vector>

#include "../Graph.h"
#include "../Partition.h"

class ClusteringInterface
{

public:

	virtual Partition *DetectCommunities(Graph *graph,
		Partition *partition,
		double &q) = 0;

};