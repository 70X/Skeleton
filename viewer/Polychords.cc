#include "Polychords.hh"

Polychords::Polychords(Cage c)
{
	C = c;
	counter.assign(c.Q.rows(), 0);
	from.assign(c.Q.rows(), -1);
	P.assign(c.Q.rows(), vector<int>(0));
}

void Polychords::computePolychords()
{
	int count, q_prec, q, ei, id=0;
	for(unsigned int i=0; i<C.Q.rows(); i++, ++id)
	{
		q = C.Q(i);
		count = counter[q];
		if (count == 2) continue;
		if (count == 1)
		{
			q_prec = from.at(q);
			ei = C.getEdgeQuadAdjacent(q, q_prec);
			P[id].push_back(q);
			counter[q]++;
			expandPolychords(id, q, C.QQ(q, (ei+1)%4));
		}
		if (count == 0)
		{
			P[id].push_back(q);
			expandPolychords(id, q, C.QQ(q, 0));
			++id;
			P[id].push_back(q);
			expandPolychords(id, q, C.QQ(q, 1));
			counter[q]+=2;
		}
	}
}

void Polychords::expandPolychords(int id, int q_start, int q)
{
	int ei, q_prec = q_start;
	while(q != q_start)
	{
		P[id].push_back(q);
		from[q] = q_prec;
		counter[q]++;

		ei = C.getEdgeQuadAdjacent(q, q_prec);
		q_prec = q;
		q = C.QQ(q, (ei+2)%4);
	}
}
