#ifndef CPHASEMODELNAIVE_H
#define CPHASEMODELNAIVE_H

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <ros/console.h>

/**
@author Tom Krajnik
*/

#define FREQS 50

typedef enum{
	 TAG_EVENT_NONE = 0,
	 TAG_EVENT_DETECTED, 
	 TAG_EVENT_READY,
	 TAG_EVENT_MOVED,
	 TAG_EVENT_NUMBER
}ETagEvent;

class CPhaseModelNaive
{
	public:
		CPhaseModelNaive(const char *id);
		~CPhaseModelNaive();

		float addMeasurement(int rssi,int freq,int phase);
		float evaluateMeasurement(int rssi,int freq,int phase);
		void buildModel();
		void print();
		void reset();
		ETagEvent readEvent();

		char ID[100];
		
		ETagEvent report;
		bool ready;
		bool moved;

		private:	
		float rssiMean[FREQS];
		float rssiStd[FREQS];
		float phaseMean[FREQS];
		float phaseStd[FREQS];
		int measurements[FREQS];
		float filteredError;

};

#endif
