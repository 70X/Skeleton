#include <Eigen/Core>
#include <vector>
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
	void expandPolychords(vector<int> *polychord, int q_start, int q_next);

	vector<vector<int>> P;
	Cage *C;
	private:
	void clear();

	vector<int> counter;
	vector<int> from;


};
#endif