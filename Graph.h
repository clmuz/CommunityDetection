#pragma once

//#include <vld.h>

#include <vector>
#include <string>

#include "Partition.h"

class Partition;

class Graph
{

public:

	Graph(int vertex_count);
	Graph(std::string filename);
	~Graph();

	int EdgeCount();
	int VertexCount();
	std::vector<int> *GetNeighbors(int vertex);

	void AddEdge(int v1, int v2);
	void SaveToFile(std::string filename);
	void SaveToFile(std::string filename, Partition *partition);

private:
	
	void LoadFromFileNet(std::string filename);
	void LoadFromFileGraph(std::string filename);
	std::string HsvToRgb(float hue, float saturation, float value);

	std::vector<std::vector<int>*> *neighbors_;
	int edge_count_;

};

