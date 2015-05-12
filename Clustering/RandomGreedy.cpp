#include "RandomGreedy.h"


//#include <vld.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <chrono>

#include "../ActiveElements.h"

using namespace std;
using namespace boost;
using namespace chrono;

RandomGreedy::RandomGreedy(int k)
{
	graph_ = nullptr;
	k_ = k;

	rows_ = nullptr;
	row_sums_ = nullptr;
	active_elements_ = nullptr;
}


RandomGreedy::~RandomGreedy()
{
	Clear();
}

Partition *RandomGreedy::DetectCommunities(Graph *graph,
	Partition *partition,
	double &q)
{
	graph_ = graph;

	if (partition == nullptr)
		Init();
	else
		Init(partition);

	q = 0;
	for (auto i : *active_elements_->Elements())
		q += (*rows_->at(i))[i] - row_sums_->at(i) * row_sums_->at(i);
	
	int best_step;
	vector<pair<int, int>> *joins = PerformJoins(q, best_step);
	Partition *result_partition = CreatePartitionFromJoins(partition,
		*joins, best_step);
	delete joins;

	Clear();

	return result_partition;
}

void RandomGreedy::Clear()
{
	if (rows_ != nullptr)
		for (auto row : *rows_)
			delete row;

	delete rows_;
	rows_ = nullptr;

	delete row_sums_;
	row_sums_ = nullptr;

	delete active_elements_;
	active_elements_ = nullptr;
}

void RandomGreedy::Init()
{
	rows_ = new vector<unordered_map<int, double> *>(graph_->VertexCount());
	row_sums_ = new vector<double>(graph_->VertexCount());
	double init_value = 1 / (2.0 * graph_->EdgeCount());

	for (int i = 0; i < graph_->VertexCount(); i++)
	{
		rows_->at(i) = new unordered_map<int, double>();
		
		for (auto neighbor : *(graph_->GetNeighbors(i)))
			(*rows_->at(i))[neighbor] = init_value;

		row_sums_->at(i) = rows_->at(i)->size() * init_value;
	}

	active_elements_ = new ActiveElements(graph_->VertexCount());
}

void RandomGreedy::Init(Partition *partition)
{
	active_elements_ = new ActiveElements(partition->CommunitiesCount());
	
	vector<int> cluster_map(graph_->VertexCount());
	partition->CreateCommunitiesMap(cluster_map, graph_->VertexCount());

	rows_ = new vector<unordered_map<int, double> *>(
		partition->CommunitiesCount());
	row_sums_ = new vector<double>(partition->CommunitiesCount());
	double init_value = 1 / (2.0 * graph_->EdgeCount());

	for (int i = 0; i < partition->CommunitiesCount(); i++)
		rows_->at(i) = new unordered_map<int, double>();

	for (int i = 0; i < graph_->VertexCount(); i++)
	{
		int cluster1 = cluster_map[i];

		vector<int> *neighbors = graph_->GetNeighbors(i);
		for (int j = 0; j < neighbors->size(); j++)
		{
			int cluster2 = cluster_map[neighbors->at(j)];
			(*rows_->at(cluster1))[cluster2] += init_value;
		}
	}

	for (int i = 0; i < partition->CommunitiesCount(); i++)
	{
		row_sums_->at(i) = 0;
		for (auto it : *rows_->at(i))
		{
			row_sums_->at(i) += it.second;
		}
	}
}

vector<pair<int, int>> *RandomGreedy::PerformJoins(double &q, int &best_step)
{
	/*char name[45];
	sprintf_s(name, "Results/Inside/rg %i.txt", k_);
	ofstream fout(name);
*/
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();

	vector<pair<int, int> > *joins = new vector<pair<int, int> >();
	best_step = -1;
	double best_q = -1;

	double prev_q = q;

	int steps_count = active_elements_->Count() - 1;
	for (int step = 0; step < steps_count; step++)
	{
		if (step > 0)
		{
			//fout << step << ";" << q - prev_q //<< "; " //<< duration_cast<std::chrono::milliseconds>
				//(high_resolution_clock::now() - t1).count() / 1000.0f
//				<< endl;
			prev_q = q;
			//t1 = high_resolution_clock::now();
		}

		int k =
			k_ < graph_->VertexCount() - 1 - step ?
						k_ :
						   graph_->VertexCount() - 1 - step;

		
		double max_delta_q = -1;
		int join_a = -1;
		int join_b = -1;

		FindJoin(k, max_delta_q, join_a, join_b);
		
		if (join_a == -1)
			break;
		joins->push_back(make_pair(join_a, join_b));
		active_elements_->Remove(join_b);
		JoinRows(join_a, join_b);

		q += max_delta_q;
		if (q > best_q)
		{
			best_q = q;
			best_step = step + 1;
		}
	}

	q = best_q;
	//fout.close();
	return joins;
}

void RandomGreedy::FindJoin(int k,
	double &max_delta_q,
	int &join_a,
	int &join_b)
{
	for (int i = 0; i < k; i++)
	{
		int row_num = active_elements_->RandomElement();
		unordered_map<int, double> *sample_row = rows_->at(row_num);

		for (auto entry : *sample_row)
		{
			int column_num = entry.first;
			double value = entry.second;

			if (column_num == row_num)
				continue;

			double delta_q = 2 * (value -
				row_sums_->at(row_num) * row_sums_->at(column_num));

			if (delta_q > max_delta_q)
			{
				max_delta_q = delta_q;

				if (rows_->at(row_num)->size() >=
					rows_->at(column_num)->size())
				{
					join_a = row_num;
					join_b = column_num;
				}
				else
				{
					join_a = column_num;
					join_b = row_num;
				}
			}
		}
	}
}

void RandomGreedy::JoinRows(int &a, int &b)
{
	for (auto entry : *rows_->at(b))
	{
		int column = entry.first;
		if (column == a || column == b)
			continue;
		
		double value = entry.second + (*rows_->at(a))[column];

		(*rows_->at(a))[column] = value;
		(*rows_->at(column))[a] = value;

		rows_->at(column)->erase(b);
	}

	(*rows_->at(a))[a] += (*rows_->at(a))[b] + (*rows_->at(b))[a] +
		(*rows_->at(b))[b];
	rows_->at(a)->erase(b);

	row_sums_->at(a) += row_sums_->at(b);
	row_sums_->at(b) = 0;
}


Partition *RandomGreedy::CreatePartitionFromJoins(
	Partition *start_partition,
	vector<pair<int, int> > &joins,
	int until_step)
{
	Partition *result_partition;

	if (start_partition == nullptr)
	{
		result_partition = new Partition(graph_->VertexCount());
		for (int i = 0; i < graph_->VertexCount(); i++)
			result_partition->Add(i, i);
	}
	else
		result_partition = new Partition(*start_partition);
	for (int i = 0; i < until_step; i++)
		result_partition->StickTogether(joins[i].first, joins[i].second);

	result_partition->RemoveEmptyCommunities();
	return result_partition;
}
