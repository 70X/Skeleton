#include "ErrorsGrid.hh"
	
	 void ErrorsGrid::updateTQ()
    {
        //cout << "Q size: "<<Env->C.Q.rows() << endl;
        vector<vector<int>> listTQ(Env->C.Q.rows());
         for(int i=0; i<Env->M.F.rows(); i++)
        {
            int q0 = Env->C.QVmesh(Env->M.F(i,0));
            int q1 = Env->C.QVmesh(Env->M.F(i,1));
            int q2 = Env->C.QVmesh(Env->M.F(i,2));
            if (q0==q1 && q1==q2)
                listTQ[q0].push_back(i);
        }
        TQ = listTQ;
    }

    double ErrorsGrid::errorSample(int q, Vector2d s)
    {
        double distance = 0;
        
        vector<int> triangles = Env->M.findTriangles(TQ[q], s, Env->C);

        //Env->storeSampleTriangles[q].insert(std::make_pair(s, triangles) );
        if (triangles.size() == 0)
        {
            int Vi = Env->C.getAreaQuad(q, s);
            CageSubDomain sC;
            sC.initAll(Env->C);
            Env->getTrianglesInExpMapping(Vi, sC);
            if (sC.triangles.size() == 0)
            {
                //cout << " Sample: not found" <<endl;
                orphanSample.push_back(s);
                return 0;
            }
            
            orphanSample.push_back(s);
            distance = Env->computeErrorFromListTriangle(sC.triangles, sC, sC.examVertex,  Env->C.getVMapping(q, s));
        }
        else
            distance = Env->computeErrorFromListTriangle(triangles, Env->C, s,  Env->C.getVMapping(q, s));
      
        return distance;
    }

	double ErrorsGrid::errorAvarageSamples( Vector2d s, double step_x, double step_y, map<Vector2d, double, Utility::classcomp> storeErrorSample)
    {
        vector<double> eRound;
        double Err = 0;

        Vector2d W  = Vector2d( s(0) - step_x, s(1)          );
        Vector2d SW = Vector2d( s(0) - step_x, s(1) - step_y );
        Vector2d S  = Vector2d( s(0)         , s(1) - step_y );
        Vector2d SE = Vector2d( s(0) + step_x, s(1) - step_y );
        Vector2d E  = Vector2d( s(0) + step_x, s(1)          );
        Vector2d NE = Vector2d( s(0) + step_x, s(1) + step_y );
        Vector2d N  = Vector2d( s(0)         , s(1) + step_y );
        Vector2d NW = Vector2d( s(0) - step_x, s(1) + step_y );

        if ( storeErrorSample.find(W) != storeErrorSample.end() && storeErrorSample.at(W) > 0)
            eRound.push_back(storeErrorSample.at(W));

        if ( storeErrorSample.find(SW) != storeErrorSample.end() && storeErrorSample.at(SW) > 0)
            eRound.push_back(storeErrorSample.at(SW));

        if ( storeErrorSample.find(S) != storeErrorSample.end() && storeErrorSample.at(S) > 0)
            eRound.push_back(storeErrorSample.at(S));

        if ( storeErrorSample.find(SE) != storeErrorSample.end() && storeErrorSample.at(SE) > 0)
            eRound.push_back(storeErrorSample.at(SE));

        if ( storeErrorSample.find(E) != storeErrorSample.end() && storeErrorSample.at(E) > 0)
            eRound.push_back(storeErrorSample.at(E));

        if ( storeErrorSample.find(NE) != storeErrorSample.end() && storeErrorSample.at(NE) > 0)
            eRound.push_back(storeErrorSample.at(NE));

        if ( storeErrorSample.find(N) != storeErrorSample.end() && storeErrorSample.at(N) > 0)
            eRound.push_back(storeErrorSample.at(N));

        if ( storeErrorSample.find(NW) != storeErrorSample.end() && storeErrorSample.at(NW) > 0)
            eRound.push_back(storeErrorSample.at(NW));

        if (eRound.size() == 0) return 0; // not Exists neighborhood of s
        for(vector<double>::const_iterator Ei = eRound.begin(); Ei != eRound.end(); ++Ei)
            Err+= *Ei;
        return (Err/(double) eRound.size());
    }

	double ErrorsGrid::errorsGridByQuadID(int q)
    {
        double tmpE, E = 0;
        double domain = DOMAIN_PARAMETER_SPACE;
        double diagonal = Env->C.bb();
        double spacing = diagonal * (1.0/20.0);

        Vector3d _A = Env->C.V.row(Env->C.Q(q,0));
        Vector3d _B = Env->C.V.row(Env->C.Q(q,1));
        Vector3d _C = Env->C.V.row(Env->C.Q(q,2));
        Vector3d _D = Env->C.V.row(Env->C.Q(q,3));
        double m = ceil( (Utility::computeDistance((_B-_A), (_D-_C))/2.0) / spacing)+1;
        double n = ceil( (Utility::computeDistance((_A-_D), (_C-_B))/2.0) / spacing)+1;
        //m = n = 5;
        double step_x = domain/m;
        double step_y = domain/n;
        
        map<Vector2d, double, Utility::classcomp> storeErrorSample;
        orphanSample.clear();
        //Env->storeSampleTriangles.push_back(map<Vector2d, vector<int>, Utility::classcomp>());
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
        
       
        return Utility::computeError(E, m*n, _A, _B, _C, _D);
    }

	void ErrorsGrid::computeErrorsGrid()
	{
        updateTQ();
		errorQuads = VectorXd(Env->C.Q.rows());
		for (int i=0; i<Env->C.Q.rows(); i++)
		{
		    errorQuads(i) = errorsGridByQuadID(i);
		    //cout << i <<" Err: "<< errorQuads(i) << " with: "<<orphanSample.size()<<endl;
		}
	}

	// idP, < qSx, qDx >
	vector<pair<int, pair<int, int> > > ErrorsGrid::errorPolychords()
    {
        vector<pair<int, pair<int, int> > > polychordsError;
		double errorMax = errorQuads.maxCoeff();
        for (int idP=0; idP<Env->P.getSize(); idP++)
        {
        	int i=0;
            vector<double> storePolychordsWithMaxError;
            for(vector<int>::const_iterator q = Env->P.P[idP].begin(); q != Env->P.P[idP].end(); ++i, ++q)
            {
            	if (errorQuads(*q) == errorMax)
            	{
            		int iQsx = (i==0) ? Env->P.P[idP].size()-1 : i-1;
            		int iQdx = (i>=Env->P.P[idP].size()-1) ? 0 : i+1;

            		int qSx = Env->P.P[idP][iQsx];
            		int qDx = Env->P.P[idP][iQdx];
                	polychordsError.push_back(make_pair(idP, make_pair(qSx, qDx)) );
            	}
            }
        }
        return polychordsError;
    }

    int ErrorsGrid::getPolychordWithMaxError()
    {
        vector<pair<int, pair<int, int> > > polychordsError = errorPolychords();
        if (polychordsError.size() == 1)
        	return (polychordsError[0]).first;
        
        if (polychordsError.size() == 2)
        {
        	int idP0 = polychordsError[0].first;
        	double ESx_0 = errorQuads(polychordsError[0].second.first);
        	double EDx_0 = errorQuads(polychordsError[0].second.second);
        	
        	int idP1 = polychordsError[1].first;
        	double ESx_1 = errorQuads(polychordsError[1].second.first);
        	double EDx_1 = errorQuads(polychordsError[1].second.second);
        	
        	if (ESx_0 + EDx_0 > ESx_1 + EDx_1)
        		return polychordsError[0].first;
        	else return polychordsError[1].first;
        }

        cout << "WARNING: found "<< polychordsError.size() <<" polychords" <<endl;
        
        return -1;
    }

    double ErrorsGrid::getErrorpolychordByID(int idP)
    {
    	double E=0;
    	for(vector<int>::const_iterator q = Env->P.P[idP].begin(); q != Env->P.P[idP].end(); ++q)
        {
    		E += errorQuads(*q);
    	}

    	return E * Env->P.P[idP].size() / Env->C.Q.rows();
    }
