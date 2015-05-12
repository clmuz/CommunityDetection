#include "Partition.h"

//#include <vld.h>

#include <algorithm>
#include <map>
#include <iostream>

using namespace std;

Partition::Partition(int communities_count)
{
	partition_ = new vector<list<int>*>(communities_count);

	for (int i = 0; i < communities_count; i++)
		partition_->at(i) = new list<int>();
}


Partition::Partition(Partition &other)
{
	partition_ = new vector<list<int>*>();

	for (auto cluster : *other.partition_)
		partition_->push_back(new list<int>(*cluster));
}

Partition::~Partition()
{
	for (auto community : *partition_)
		delete community;
	delete partition_;
}

void Partition::CreateCommunitiesMap(vector<int> &communities_map,
	int vertex_count)
{	
	int i = 0;
	for (auto community : *partition_)
	{
		for (auto vertex : *community)
			communities_map[vertex] = i;
		i++;
	}
}

int Partition::CommunitiesCount()
{
	return partition_->size();
}

void Partition::RemoveEmptyCommunities()
{
	for (auto it = partition_->begin(); it != partition_->end(); )
	{
		if ((*it)->empty())
		{
			delete *it;
			it = partition_->erase(it);
		}
		else
			it++;
	}
}

void Partition::Remove(int vertex, int community)
{
	partition_->at(community)->remove(vertex);
}

void Partition::Add(int vertex, int community)
{
	partition_->at(community)->push_back(vertex);
}

void Partition::StickTogether(int a, int b)
{
	partition_->at(a)->splice(partition_->at(a)->end(), *partition_->at(b));
}

std::vector<std::list<int>*> *Partition::PartitionVector()
{
	return partition_;
}

double Partition::Modularity(Graph *graph)
{
	int cluster_count = CommunitiesCount();
	vector<int> communities_map(graph->VertexCount());
	CreateCommunitiesMap(communities_map, graph->VertexCount());

	vector<map<int, double> *> e(cluster_count);
	for (int i = 0; i < cluster_count; i++)
		e[i] = new map<int, double>();

	int edge_count = 0;
	for (int i = 0; i < graph->VertexCount(); i++)
	{
		vector<int> *neighbors = graph->GetNeighbors(i);

		for (int j = 0; j < neighbors->size(); j++)
		{
			if (i == neighbors->at(j))
				continue;

			int from = communities_map[i];
			int to = communities_map[neighbors->at(j)];
			(*e[from])[to] += 1.0;
			edge_count++;
		}
	}

	vector<double> a(cluster_count);
	for (int i = 0; i < cluster_count; i++)
	{
		a[i] = 0.0;
		for (auto it : *e[i])
		{
			int column = it.first;
			e[i]->at(column) /= (double) edge_count;
			a[i] += e[i]->at(column);
		}
	}

	double q = 0.0;
	for (int i = 0; i < cluster_count; i++)
	{
		if (e[i]->find(i) != e[i]->end())
			q += e[i]->at(i) - a[i] * a[i];
		else
			q -= a[i] * a[i];
	}

	for (int i = 0; i < e.size(); i++)
		delete e[i];

	return q;
}

void Partition::CreateNewCommunity()
{
	partition_->push_back(new list<int>());
}

Partition *Partition::OverlapClusters(Partition *partition_a,
	Partition *partition_b,
	int vertex_count)
{
	vector<int> map_b(vertex_count);
	partition_b->CreateCommunitiesMap(map_b, vertex_count);

	Partition *result_clustering = new Partition(0);
	vector<bool> assigned(vertex_count, false);

	for (auto cluster : *partition_a->PartitionVector())
	{
		for (int vertex1 : *cluster)
		{
			if (assigned[vertex1])
				continue;

			result_clustering->CreateNewCommunity();
			result_clustering->Add(vertex1,
				result_clustering->CommunitiesCount() - 1);
			assigned[vertex1] = true;

			for (int vertex2 : *cluster)
			{
				if (!assigned[vertex2] && map_b[vertex1] == map_b[vertex2])
				{
					result_clustering->Add(vertex2,
						result_clustering->CommunitiesCount() - 1);
					assigned[vertex2] = true;
				}
			}
		}
	}

	return result_clustering;
}
