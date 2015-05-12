#pragma once

//#include <vld.h>

#include <vector>
#include <list>

#include "Graph.h"

class Graph;

class Partition
{

public:

	Partition(int communities_count);
	Partition(Partition &other);
	~Partition();

	static Partition *OverlapClusters(Partition *partition_a,
		Partition *partition_b,
		int vertex_count);

	void CreateCommunitiesMap(std::vector<int> &communities_map,
		int vertex_count);
	int CommunitiesCount();
	void RemoveEmptyCommunities();

	void Remove(int vertex, int community);
	void Add(int vertex, int community);
	void StickTogether(int a, int b);

	std::vector<std::list<int>*> *PartitionVector();
	double Modularity(Graph *graph);
	void CreateNewCommunity();

private:

	std::vector<std::list<int>*> *partition_;

};

