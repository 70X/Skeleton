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
	Polychords(){};
	Polychords(Cage *c);
	Polychords(Cage *c, vector<vector<int>> P);
	~Polychords(){};
	
	int getSize()	{	return P.size();	};

	void computePolychords();
	void expandPolychords(vector<int> *polychord, int q_start, int q_next, int it);

	vector<vector<int>> P;
	Cage *C;

	// key: id quad  value: two polychord across quad
	map<int , pair<int, int> > PQ;
	private:
	void clear();
	void updatePQ(int q, int it);

	vector<int> counter;
	vector<int> from;

};
#endif