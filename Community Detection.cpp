#include "tchar.h"

//#include <vld.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <conio.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <sstream>
#include <thread>

#include "Graph.h"
#include "BaseGenerator.h"
#include "Partition.h"
#include "Clustering\RandomGreedy.h"
#include "Clustering\EnsembleStrategy.h"
#include "Clustering\AdaptiveRandomGreedy.h"
#include "Clustering\OptimalEnsembleStrategy.h"
#include "Clustering\AdaptiveEnsembleStrategy.h"

using namespace std;
using namespace chrono;

Graph *load_graph(string name)
{
	return new Graph("D:/9. Диплом/Test/" + name + ".graph");
}

void test_graph(Graph* graph, ClusteringInterface *alg, int runs, double &q, double &time)
{
	vector<double> qs;
	double q1;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0; i < runs; i++)
	{
		delete alg->DetectCommunities(graph, nullptr, q1);

		qs.push_back(q1);
	}

	time = duration_cast<milliseconds>(
		high_resolution_clock::now() - t1).count() / (double)runs;
	sort(qs.begin(), qs.end());

	q = runs % 2 == 0 ? .5 * (qs[runs / 2] + qs[runs / 2 + 1]) :
		qs[runs / 2];
}

void run_graph(Graph *graph, string name)
{
	ofstream fout("Results/Intermediate/" + name + ".csv");
	fout << "alpha;q;time\n";

	int t_num = 3;
	int j_max = ceil(24.0 / t_num);
	vector<double> qs(t_num * j_max);
	vector<double> times(t_num * j_max);

	thread **t = new thread *[t_num];

	auto param = [](int z) -> int 
	{
		//return (z + 2) * 500;
		return 1 + z * 25;
	};

	auto thread_f = [&graph, j_max, t_num, &qs, &times, &param](int i)
		{
			RandomGreedy *rg10 = new RandomGreedy(10);
			for (int j = 0; j < j_max; j++)
			{
				double q, time;
				int z = t_num * j + i;

				ClusteringInterface *alg = new AdaptiveEnsembleStrategy(
					2,
					[z, &param](double q, double k) -> double { return -param(z) * log(q); },
					5,
					10,
					1,
					rg10);

				test_graph(graph, alg, 1, q, time);
				delete alg;
				qs[z] = q;
				times[z] = time;
				cout << i << " " << j << " " << z << " " << q << " " << time << endl;
			}

			delete rg10;
		};
	for (int i = 0; i < t_num; i++)
		t[i] = new thread(thread_f, i);


	for (int i = 0; i < t_num; i++)
	{
		t[i]->join();
		delete t[i];
	}
	delete[] t;

	for (int i = 0; i < t_num * j_max; i++)
	{
		fout << param(i) << ";" << qs[i] << ";" << times[i] << endl;
	}
	
	fout.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));

	//Graph *graph = new Graph("D:/9. Диплом/Информация для диплома/Результат прогона/13/graph-in.net");
	
	Graph *graph = load_graph("jazz");
	//run_graph(graph, "auto/auto_alpha");

	/*

	ofstream fout("Resuls/aes.csv");
	fout << "l1;q;t\n";

	double q, time;

	RandomGreedy *rg10 = new RandomGreedy(10);

	for (int i = 5; i <= 20; i++)
	{
		ClusteringInterface *alg = new AdaptiveEnsembleStrategy(
			2,
			[](double q, double k) -> double { return -5000 * log(q); },
			5,
			10,
			10,
			rg10);
		test_graph(graph, alg, 11, q, time);
		cout << i << " " << q << " " << time << endl;
		fout << i << ";" << q << ";" << time << endl;
		delete alg;
	}

	delete rg10;

	cout << q << " " << time;

	fout.close();*/
	delete graph;
	/*
	string names[] = {
		"karate",
		"dolphins",
		"chesapeake",
		"adjnoun",
		"polbooks",
		"football",
		"celegans_metabolic",
		"jazz",
		"netscience",
		"email",
		"polblogs",
		"PGPgiantcompo",
		"as-22july06",
		"cond-mat-2003",
		"caidaRouterLevel",
		"cnr-2000",
		"in-2004",
		"eu-2005"
	};*/

	cout << "\nSuccess\a";
	_getch();
	return 0;
}