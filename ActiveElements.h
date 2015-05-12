#pragma once

//#include <vld.h>

#include <vector>

//#include <boost/unordered_map.hpp>

#include "Partition.h"

class ActiveElements
{

public:

	ActiveElements(int n);
	~ActiveElements();
	int Count();
	int RandomElement();
	void Remove(int &element);
	std::vector<int> *Elements();

private:

	std::vector<int> *elements_;
	std::vector<int> *element_positions_;
	int count_;

};

