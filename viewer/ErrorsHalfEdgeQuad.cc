#include "ErrorsHalfEdgeQuad.hh"
	
	double ErrorsHalfEdgeQuad::errorsQuadAlongDirection(int q, double step_x, double step_y, int m, int n)
	{
		double tmpE, E = 0;
        map<Vector2d, double, Utility::classcomp> storeErrorSample;

		orphanSample.clear();
        Env->storeSampleTriangles.push_back(map<Vector2d, vector<int>, Utility::classcomp>());
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
        double diagonal = C->bb();
        double spacing = diagonal * (1.0/20.0);

        Vector3d _A = C->V.row(C->Q(q,0));
        Vector3d _B = C->V.row(C->Q(q,1));
        Vector3d _C = C->V.row(C->Q(q,2));
        Vector3d _D = C->V.row(C->Q(q,3));
        double m = ceil( (Utility::computeDistance((_B-_A), (_D-_C))/2.0) / spacing)+1;
        double n = ceil( (Utility::computeDistance((_A-_D), (_C-_B))/2.0) / spacing)+1;
        // m = n = 2;
        double step_x = domain/m;
        double step_y = domain/n;

        double halfX = domain/2.0;
        double halfY = domain/2.0;

        int e0 = 0; // direction
        int e1 = 1;
        int e2 = 2;
        int e3 = 3;

        E_directionX = errorsQuadAlongDirection(q, halfX, step_y, 2, n) * Utility::areaQuad(_A, _B, _C, _D ) / (m*n);
        E_directionY = errorsQuadAlongDirection(q, step_x, halfY, m, 2) * Utility::areaQuad(_A, _B, _C, _D ) / (m*n);
       
        errorQuadsByDirection.insert(make_pair(make_pair(q, e0), E_directionY) );
        errorQuadsByDirection.insert(make_pair(make_pair(q, e2), E_directionY) );

		errorQuadsByDirection.insert(make_pair(make_pair(q, e1), E_directionX) );
        errorQuadsByDirection.insert(make_pair(make_pair(q, e3), E_directionX) );
    }

	void ErrorsHalfEdgeQuad::computeErrorsGrid()
	{
		for (int i=0; i<C->Q.rows(); i++)
		{
		    errorsGridByQuadID(i);
		    //cout << i <<" Err: "<< errorQuads(i) << " with: "<<orphanSample.size()<<endl;
		}
	}

	int ErrorsHalfEdgeQuad::getPolychordWithMaxError()
    {
    	int direction, q_next;
    	double errorMax = 0;
    	int worstPolychord = 0;
    	for (int idP=0; idP<Env->P.getSize(); idP++)
        {
            vector<double> storePolychordsWithMaxError;
            
            for(vector<int>::const_iterator q = Env->P.P[idP].begin(); q != Env->P.P[idP].end(); ++q)
            {
            	if ((q+1) != Env->P.P[idP].end())
            		q_next = *(q+1);
            	else q_next = Env->P.P[idP][0];
            	direction = C->getEdgeQuadAdjacent(*q, q_next);
            	double errDoubleHlfEdges = errorQuadsByDirection[{*q, direction}]; //+ errorQuadsByDirection[{*q, (direction+2)%4}];
            	//cout << "POLYCHORD "<<idP<<"("<<*q<<","<<q_next<<") : "<< errDoubleHlfEdges<<" -> direction: "<< direction<<endl;
            	if (errorMax < errDoubleHlfEdges)
            	{
            		errorMax = errDoubleHlfEdges;
            		worstPolychord = idP;
            	}
            }
        }
        return worstPolychord;
    }