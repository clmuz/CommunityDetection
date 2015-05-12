#include "BaseGenerator.h"

//#include <vld.h>

#include <stdlib.h>

Graph *BaseGenerator::Generate(int vertex_count,
	int cluster_count,
	std::function<int (int vertex)> vertex_cluster,
	double probability_inside_cluster,
	double probability_outside_cluster)
{
	Graph *graph = new Graph(vertex_count);

	for (int i = 0; i < vertex_count; i++)
		for (int j = i + 1; j < vertex_count; j++)
			if (vertex_cluster(i) == vertex_cluster(j))
			{
				if ((double)rand() / RAND_MAX < probability_inside_cluster)
					graph->AddEdge(i, j);
			}
			else if ((double)rand() / RAND_MAX < probability_outside_cluster)
				graph->AddEdge(i, j);

	return graph;
}
