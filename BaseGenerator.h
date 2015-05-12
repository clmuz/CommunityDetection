#pragma once

//#include <vld.h>
#include <functional>

#include "Graph.h"

class BaseGenerator
{

public:

	static Graph *Generate(int vertex_count,
		int cluster_count,
		std::function<int (int vertex)> vertex_cluster,
		double probability_inside_cluster,
		double probability_outside_cluster);

};

