#include "Polychords.hh"

Polychords::Polychords(Cage *c)
{
	C = c;
	clear();
}

Polychords::Polychords(Cage *c, vector<vector<int>> P)
{
	C = c;
	clear();
	this->P = P;
}

void Polychords::clear()
{
	P.clear(); counter.clear(); from.clear();
	PQ.clear();
	counter.assign(C->Q.rows(), 0);
	from.assign(C->Q.rows(), -1);
}

void Polychords::computePolychords()
{
	clear();
	int count, q_prec, ei, it = 0;
	for(unsigned int q=0; q<C->Q.rows(); q++)
	{
		count = counter[q];
		vector<int> tmpp;
		if (count == 2) continue;
		else if (count == 1)
		{
			q_prec = from[q];

			ei = C->getEdgeQuadAdjacent(q, q_prec);
			tmpp.push_back(q);

			counter[q]++;
			expandPolychords(&tmpp, q, C->QQ(q, (ei+1)%4), it);
			P.push_back(tmpp);

			updatePQ(q, it);
			it++;
		}
		else if (count == 0)
		{
			tmpp.push_back(q);
			
			expandPolychords(&tmpp, q, C->QQ(q, 0), it);
			P.push_back(tmpp);

			updatePQ(q, it+1);

			tmpp.clear();
			tmpp.push_back(q);
			expandPolychords(&tmpp, q, C->QQ(q, 1), it);
			P.push_back(tmpp);

			updatePQ(q, it+2);

			it += 2;
			counter[q]+=2;
		}
	}
}

void Polychords::expandPolychords(vector<int> *polychord, int q_start, int q, int it)
{
	int ei, q_prec = q_start;
	while(q != q_start)
	{
		polychord->push_back(q);
		
		updatePQ(q, it);

		from[q] = q_prec;
		counter[q]++;
		ei = C->getEdgeQuadAdjacent(q, q_prec);
		q_prec = q;
		q = C->QQ(q, (ei+2)%4);

	}
}

void Polychords::updatePQ(int q, int it)
{
	if ( PQ.find(q) == PQ.end())
	{
		//cout << "PE "<< q<<" => "<<it<< "  -1"<<endl;
		PQ.insert(make_pair(q, make_pair(it, -1) ) );
	}
	else if (PQ[q].second == -1)
	{
		//cout << "PE "<< q<<" => "<<PQ[q].first<< "  "<<it<<endl;
		PQ[q] = make_pair(PQ[q].first, it );
	}
}