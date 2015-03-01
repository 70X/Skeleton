#include "Polychords.hh"

Polychords::Polychords(Cage *c)
{
	C = c;
	clear();
}

void Polychords::clear()
{
	P.clear(); counter.clear(); from.clear();
	counter.assign(C->Q.rows(), 0);
	from.assign(C->Q.rows(), -1);
}

void Polychords::computePolychords()
{
	clear();
	int count, q_prec, q, ei;
	for(unsigned int q=0; q<C->Q.rows(); q++)
	{
		count = counter[q];
		vector<int> tmpp;
		if (count == 2) continue;
		if (count == 1)
		{
			q_prec = from[q];
			ei = C->getEdgeQuadAdjacent(q, q_prec);
			tmpp.push_back(q);
			counter[q]++;
			expandPolychords(&tmpp, q, C->QQ(q, (ei+1)%4));

			P.push_back(tmpp);
		}
		if (count == 0)
		{
			tmpp.push_back(q);
			expandPolychords(&tmpp, q, C->QQ(q, 0));
			P.push_back(tmpp);

			tmpp.clear();
			tmpp.push_back(q);
			expandPolychords(&tmpp, q, C->QQ(q, 1));
			P.push_back(tmpp);

			counter[q]+=2;
		}
	}
}

void Polychords::expandPolychords(vector<int> *polychord, int q_start, int q)
{
	int ei, q_prec = q_start;
	while(q != q_start)
	{
		polychord->push_back(q);
		from[q] = q_prec;
		counter[q]++;
		
		ei = C->getEdgeQuadAdjacent(q, q_prec);
		q_prec = q;
		q = C->QQ(q, (ei+2)%4);
	}
}
