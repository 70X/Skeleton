#include "ErrorsHalfEdgeQuad.hh"
	
	double ErrorsHalfEdgeQuad::errorsQuadAlongDirection(int q, double step_x, double step_y, int m, int n)
	{
		double tmpE, E = 0;
        map<Vector2d, double, Utility::classcomp> storeErrorSample;

		orphanSample.clear();
        
        #ifdef __MODE_DEBUG
        Env->storeSampleTriangles.push_back(map<Vector2d, vector<int>, Utility::classcomp>());
        #endif

        for (int i=1; i<m; i++)
	        for (int j=1; j<n; j++)
	        {
	            Vector2d s = Vector2d(step_x*i, step_y*j);
	            tmpE = errorSample(q, s);
	            storeErrorSample.insert(make_pair(s, tmpE) );
	            E += tmpE;
	            // when return 0 is handled ahead
        	}

        // management orphan sample
        if (orphanSample.size() > 0)
        {
            for(vector<Vector2d>::const_iterator s = orphanSample.begin(); s != orphanSample.end(); ++s)
            {
                tmpE = errorAvarageSamples((*s),step_x, step_y, storeErrorSample);
                //cout << q << " s: "<< (*s)(0)<<","<<(*s)(1)<<" = "<< tmpE <<endl;
                E += tmpE;
            }
        }
        return E;
	}
	
	void ErrorsHalfEdgeQuad::errorsGridByQuadID(int q)
    {
        double E_directionX = 0, E_directionY = 0;
        double domain = DOMAIN_PARAMETER_SPACE;
        double diagonal = Env->C.bb();
        double spacing = diagonal * (1.0/20.0);

        Vector3d _A = Env->C.V.row(Env->C.Q(q,0));
        Vector3d _B = Env->C.V.row(Env->C.Q(q,1));
        Vector3d _C = Env->C.V.row(Env->C.Q(q,2));
        Vector3d _D = Env->C.V.row(Env->C.Q(q,3));
        double m = ceil( (Utility::computeDistance((_B-_A), (_D-_C))/2.0) / spacing)+1;
        double n = ceil( (Utility::computeDistance((_A-_D), (_C-_B))/2.0) / spacing)+1;
        // m = n = 2;
        double step_x = domain/m;
        double step_y = domain/n;

        double halfX = domain/2.0;
        double halfY = domain/2.0;

        pair<int, int> p0 = make_pair(q, 0); // direction
        pair<int, int> p1 = make_pair(q, 1);
        pair<int, int> p2 = make_pair(q, 2);
        pair<int, int> p3 = make_pair(q, 3);

        E_directionX = Utility::computeError(errorsQuadAlongDirection(q, halfX, step_y, 2, n), m+n, _A, _B, _C, _D);
        E_directionY = Utility::computeError(errorsQuadAlongDirection(q, step_x, halfY, m, 2), m+n, _A, _B, _C, _D);

        //cout << "N : "<<q << "  "<<E_directionY << " | "<< E_directionX<< endl;
        if (errorQuadsByDirection.find(make_pair(q, 0)) == errorQuadsByDirection.end())
        {
        	errorQuadsByDirection.insert(make_pair(p0, E_directionY) );
        	errorQuadsByDirection.insert(make_pair(p2, E_directionY) );
        	errorQuadsByDirection.insert(make_pair(p1, E_directionX) );
        	errorQuadsByDirection.insert(make_pair(p3, E_directionX) );
        	return;
        }
       
        errorQuadsByDirection[p0] = E_directionY;
        errorQuadsByDirection[p2] = E_directionY;
        errorQuadsByDirection[p1] = E_directionX;
        errorQuadsByDirection[p3] = E_directionX;
    }

	void ErrorsHalfEdgeQuad::computeErrorsGrid(vector<int> listQuad)
	{
        updateTQ();

        vector<int> recomputePolychord;
		for(vector<int>::const_iterator q = listQuad.begin(); q != listQuad.end(); ++q)
        {
		    errorsGridByQuadID(*q);

            pair<int, int> polychords = Env->P.PQ[*q];
		    if (find(recomputePolychord.begin(), recomputePolychord.end(), polychords.first) == recomputePolychord.end())
		    	recomputePolychord.push_back(polychords.first);
		    if (find(recomputePolychord.begin(), recomputePolychord.end(), polychords.second) == recomputePolychord.end())
		    	recomputePolychord.push_back(polychords.second);
		}	
		errorPolychords(recomputePolychord);
	}

	int ErrorsHalfEdgeQuad::getPolychordWithMaxError()
    {
    	double errorMax = 0;
    	int worstPolychord = -1;
    	for (map<int, double>::const_iterator it = storeErrorPolychords.begin(); it != storeErrorPolychords.end(); ++it)
        {
        	if (errorMax < it->second)
        	{
        		errorMax = it->second;
        		worstPolychord = it->first;
        	}
        }
        Env->info.LastError = errorMax;
    	return worstPolychord;
    }


    double ErrorsHalfEdgeQuad::getErrorpolychordByID(int idP)
    {
    	int direction, q_next;
    	double E =0, errorMax = 0;
    	for(vector<int>::const_iterator q = Env->P.P[idP].begin(); q != Env->P.P[idP].end(); ++q)
        {
        	if ((q+1) != Env->P.P[idP].end())
        		q_next = *(q+1);
        	else q_next = Env->P.P[idP][0];
        	direction = Env->C.getEdgeQuadAdjacent(*q, q_next);
        	if (errorMax < (E = errorQuadsByDirection[{*q, direction}]) )
        		errorMax = E;
    		//cout << "POLYCHORD "<<idP<<"("<<*q<<","<<q_next<<") : "<< errorQuadsByDirection[{*q, direction}]<<" -> direction: "<< direction<<endl;
    	}
    	//cout << "POLYCHORD "<<idP<<" | "<<E * Env->P.P[idP].size() / C->Q.rows()<< direction<<endl;
    	return errorMax;// * Env->P.P[idP].size() / C->Q.rows();
    }

    void ErrorsHalfEdgeQuad::errorPolychords(vector<int> listPolychord)
    {
    	for(vector<int>::const_iterator idP = listPolychord.begin(); idP != listPolychord.end(); ++idP)
        {
        	if (storeErrorPolychords.find(*idP) == storeErrorPolychords.end())
        	{
        		storeErrorPolychords.insert(make_pair(*idP, getErrorpolychordByID(*idP) ) );
        		continue;
        	}
        	storeErrorPolychords[*idP] = getErrorpolychordByID(*idP);
        }

        //for (map<int, double>::const_iterator it = storeErrorPolychords.begin(); it != storeErrorPolychords.end(); ++it)
        //    cout << "["<<it->first<<"]"<<" "<<it->second <<endl;
        //cout << "END "<<endl;
    }