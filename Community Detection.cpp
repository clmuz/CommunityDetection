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

#include "Graph.h"
#include "BaseGenerator.h"
#include "Partition.h"
#include "Clustering\RandomGreedy.h"
#include "Clustering\EnsembleStrategy.h"
#include "Clustering\AdaptiveRandomGreedy.h"
#include "Clustering\OptimalEnsembleStrategy.h"
#include "Clustering\AdaptiveEnsembleStrategy.h"
#include "Tester.h"

using namespace std;
using namespace chrono;

void run_graph(vector<string> &names)
{
	int m = 7;
	double q;

	ofstream fout("Results/runs.csv");

	for (string name : names)
	{
		Graph *graph = new Graph("D:/9. Диплом/Test/" + name + ".graph");

		stringstream ss_q;
		ss_q.str(name);
		stringstream ss_t;
		ss_t.str(name);

		for (int k = 1; k <= 15; k += 1)
		{
			vector<double> qs;

			ClusteringInterface *alg = new RandomGreedy(k);
			high_resolution_clock::time_point t1 = high_resolution_clock::now();

			for (int i = 0; i < m; i++)
			{
				delete alg->DetectCommunities(graph, nullptr, q);
				qs.push_back(q);
			}

			double time = duration_cast<milliseconds>(
				high_resolution_clock::now() - t1).count() / (double)m;

			sort(qs.begin(), qs.end());

			cout << name << " " << k << " " << qs[m / 2] << " " << time << endl;
			ss_q << ";" << qs[m / 2];
			ss_t << ";" << time;
		}

		fout << ss_q.str() << endl << ss_t.str() << endl;
	}

	fout.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));

	vector<string> names{ "caidaRouterLevel", "cnr-2000" };

	run_graph(names);

	/*
	double q;

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
	};

	ofstream fout("Results/all.csv");
	
	ClusteringInterface *alg = nullptr;

	for (string name : names)
	{
		Graph *graph = new Graph("D:/9. Диплом/Test/" + name + ".graph");

		int m = graph->EdgeCount() < 1000 ? 101 :
			graph->EdgeCount() < 10000 ? 11 :
			graph->EdgeCount() < 300000 ? 3 :
			1;

		vector<double> qs;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		for (int i = 0; i < m; i++)
		{
			delete alg->DetectCommunities(graph, nullptr, q);
			qs.push_back(q);
		}

		double time = duration_cast<milliseconds>(
			high_resolution_clock::now() - t1).count() / (double)m;

		sort(qs.begin(), qs.end());

		cout << name << ": " << qs[m / 2] << " " << time << endl;
		fout << name << ";" << qs[m / 2] << ";" << time << endl;

		delete graph;
	}

		
	
	delete alg;
	//}

	fout.close();
	*/
	cout << "\nSuccess\a";
	_getch();
	return 0;
}