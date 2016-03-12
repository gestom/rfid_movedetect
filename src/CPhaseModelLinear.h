#ifndef CPHASEMODELLINEAR_H
#define CPHASEMODELLINEAR_H

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>

/**
@author Tom Krajnik
*/

#define MAX_MEASUREMENTS 10000
#define FREQS 50
#define PHASES 180 

typedef struct
{
	int strength;
	int phase;
	int optimiz;
	float wavelength;
	int freq;
}SMeasurement;


SMeasurement measurements[MAX_MEASUREMENTS];
SMeasurement corrected[MAX_MEASUREMENTS];

using namespace std;

class CPhaseModelLinear
{
	public:
		CPhaseModelLinear();
		~CPhaseModelLinear();

		void addMeasurement(int rssi,int freq,int phase);
		float evaluateMeasurement(int rssi,int freq,int phase);
		void buildModel();

		int numMeasurements;
		float slope;
		float intercept;
};

#endif
