#ifndef CPHASEMODELSET_H
#define CPHASEMODELSET_H

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "CPhaseModelNaive.h"
#include <ros/console.h>

/**
@author Tom Krajnik
*/

#define MAX_PHASE_MODELS 100 

using namespace std;

class CPhaseModelSet
{
	public:
		CPhaseModelSet();
		~CPhaseModelSet();

		float addMeasurement(const char *id,int rssi,int freq,int phase);
		ETagEvent readEvent(const char *id);
		void print(int verbosity = 0);
		int find(const char *id);

		CPhaseModelNaive *models[MAX_PHASE_MODELS];
		CPhaseModelNaive *active;
		int numModels;
};

#endif
