#include "ActiveElements.h"

//#include <vld.h>

#include <stdlib.h>

using namespace std;

ActiveElements::ActiveElements(int n)
{
	elements_ = new vector<int>(n);
	element_positions_ = new vector<int>(n);
	count_ = n;

	for (int i = 0; i < n; i++)
	{
		elements_->at(i) = i;
		element_positions_->at(i) = i;
	}
}\

ActiveElements::~ActiveElements()
{
	delete elements_;
	delete element_positions_;
}

int ActiveElements::Count()
{
	return count_;
}

int ActiveElements::RandomElement()
{
	return elements_->at(rand() % count_);
}

void ActiveElements::Remove(int &element)
{
	int index = element_positions_->at(element);
	element_positions_->at(elements_->at(count_ - 1)) = index;
	elements_->at(index) = elements_->at(count_ - 1);
	count_--;
}

std::vector<int>* ActiveElements::Elements()
{
	return elements_;
}

