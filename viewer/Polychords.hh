#include <Eigen/Core>
#include <vector>
#include <map>
#include "Cage.hh"

using namespace std;

#ifndef _POLYCHORDS_CLASS
#define _POLYCHORDS_CLASS

class Polychords
{
	public:
	vector<vector<int>> P;
	// key: id quad  value: ids two polychord across quad
	map<int , pair<int, int> > PQ;
	
	Polychords(){};
	Polychords(Cage *c);
	~Polychords(){};
	
	int getSize()	{	return P.size();	};

	void computePolychords();
	private:
	void clear();
	void updatePQ(int q, int it);
	void expandPolychords(vector<int> *polychord, int q_start, int q_next, int it);

	Cage *C;
	vector<int> counter;
	vector<int> from;

};
#endif