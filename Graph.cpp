#include "Graph.h"

//#include <vld.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Graph::Graph(int vertex_count)
{
	neighbors_ = new vector<vector<int>*>(vertex_count);
	for (int i = 0; i < vertex_count; i++)
		neighbors_->at(i) = new vector<int>();
	edge_count_ = 0;
}

Graph::Graph(string filename)
{
	edge_count_ = 0;
	if (filename.back() == 't') // .net
		LoadFromFileNet(filename);
	else						// .graph
		LoadFromFileGraph(filename);
}


Graph::~Graph()
{
	for (int i = 0; i < VertexCount(); i++)
		delete neighbors_->at(i);
	delete neighbors_;
}

void Graph::AddEdge(int v1, int v2)
{
	neighbors_->at(v1)->push_back(v2);
	neighbors_->at(v2)->push_back(v1);
	edge_count_++;
}

void Graph::LoadFromFileNet(string filename) // Pajek .net format
{
	ifstream fin(filename);
	
	if (!fin)
	{
		cout << "Could not open file\n";
		throw;
	}

	string str;

	int vertex_count = 0;
	fin >> str; // *Vertixes
	fin >> vertex_count;

	neighbors_ = new vector<vector<int>*>(vertex_count);
	for (int i = 0; i < vertex_count; i++)
		neighbors_->at(i) = new vector<int>();

	fin >> str; // *Edges

	int v1, v2; // ends of edge

	while (fin >> v1)
	{
		fin >> v2;
		v1--;
		v2--;

		neighbors_->at(v1)->push_back(v2);
		neighbors_->at(v2)->push_back(v1);
		edge_count_++;
	}
	
	fin.close();
}

void Graph::LoadFromFileGraph(string filename) // METIS .graph format
{
	ifstream fin(filename);

	if (!fin)
	{
		cout << "Could not open file\n";
		throw;
	}

	string str;

	int vertex_count = 0;
	fin >> vertex_count;

	neighbors_ = new vector<vector<int>*>(vertex_count);
	for (int i = 0; i < vertex_count; i++)
		neighbors_->at(i) = new vector<int>();

	int edge_count;
	int tmp;
	fin >> edge_count >> tmp; // edges and 0

	string line;
	getline(fin, line);

	for (int i = 0; i < vertex_count; i++)
	{
		getline(fin, line);
		stringstream linestream(line);

		int vertex;
		while (linestream >> vertex)
		{
			if (i < vertex - 1)
			{
				neighbors_->at(i)->push_back(vertex - 1);
				neighbors_->at(vertex - 1)->push_back(i);
				edge_count_++;
			}
		}
	}

	fin.close();
	
	if (edge_count != edge_count_)
	{
		cout << "Wrong edge count\n";
		cout << edge_count << " " << edge_count_ << endl;
		cin >> tmp;
	}
}

void Graph::SaveToFile(string filename)
{
	ofstream fout(filename);

	if (!fout)
	{
		cout << "Could not open file\n";
		throw;
	}

	fout << "*Vertices " << VertexCount() << endl;
	fout << "*Edges\n";

	for (int i = 0; i < VertexCount(); i++)
	{
		for (int neighbor : *(neighbors_->at(i)) )
		{
			if (neighbor > i)
				fout << i + 1 << " " << neighbor + 1 << endl;
		}
	}

	fout.close();
}

void Graph::SaveToFile(string filename, Partition *partition)
{
	ofstream fout(filename);

	if (!fout)
	{
		cout << "Could not open file\n";
		throw;
	}

	fout << "*Vertices " << VertexCount() << endl;

	vector<int> communities_map(VertexCount());
	partition->CreateCommunitiesMap(communities_map, VertexCount());
	int communities_count = partition->CommunitiesCount();

	float phi = (1 + sqrt(5)) / 2;
	for (int i = 0; i < VertexCount(); i++)
	{
		fout << i + 1 << " \"" << i + 1 << "\" ic \"";
		float hue = communities_map[i] * phi 
			- floor(communities_map[i] * phi);
		fout << HsvToRgb(hue, 0.45f, 0.99f) << "\"\n";
	}

	fout << "*Edges\n";

	for (int i = 0; i < VertexCount(); i++)
	{
		for (int neighbor : *(neighbors_->at(i)))
		{
			if (neighbor > i)
				fout << i + 1 << " " << neighbor + 1 << endl;
		}
	}

	fout.close();
}

string Graph::HsvToRgb(float hue, float saturation, float value)
{
	int hue_i = (hue * 6);
	float f = hue * 6 - hue_i;
	float p = value * (1 - saturation);
	float q = value * (1 - f * saturation);
	float t = value * (1 - (1 - f) * saturation);
	
	float r, g, b;
	switch (hue_i)
	{
	case 0:
		r = value; g = t; b = p;
		break;
	case 1:
		r = q; g = value; b = p;
		break;
	case 2:
		r = p; g = value; b = t;
		break;
	case 3:
		r = p; g = q; b = value;
		break;
	case 4:
		r = t; g = p; b = value;
		break;
	case 5:
		r = value; g = p; b = q;
		break;
	}

	int red = r * 255;
	int blue = b * 255;
	int green = g * 255;

	char buffer[10];
	sprintf_s(buffer, "#%02X%02X%02XFF", red, green, blue);
	return string(buffer);
}

vector<int> *Graph::GetNeighbors(int vertex)
{
	return neighbors_->at(vertex);
}

int Graph::EdgeCount()
{
	return edge_count_;
}

int Graph::VertexCount()
{
	return neighbors_->size();
}