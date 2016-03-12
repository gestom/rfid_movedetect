#include "CPhaseModelLinear.h"

using namespace std;
static bool debug = false; 

int compareFreq(const void* m1,const void* m2)
{
	if (((SMeasurement*)m1)->freq  < ((SMeasurement*)m2)->freq)  return -1;
	if (((SMeasurement*)m1)->freq  > ((SMeasurement*)m2)->freq)  return +1;
	if (((SMeasurement*)m1)->phase < ((SMeasurement*)m2)->phase) return -1;
	if (((SMeasurement*)m1)->phase > ((SMeasurement*)m2)->phase) return +1;
	return 0;
}

CPhaseModelLinear::CPhaseModelLinear()
{
	numMeasurements = 0;
}

CPhaseModelLinear::~CPhaseModelLinear()
{
}

void CPhaseModelLinear::addMeasurement(int rssi,int freq,int phase)
{	
 	if (numMeasurements < MAX_MEASUREMENTS)
	{	
		measurements[numMeasurements].strength =  rssi; 
		measurements[numMeasurements].phase =  phase; 
		measurements[numMeasurements].freq =  (freq-902750)/500;
		numMeasurements++;
	}
}

float CPhaseModelLinear::evaluateMeasurement(int rssi,int freq,int phase)
{
}

void CPhaseModelLinear::buildModel()
{
	qsort(measurements,numMeasurements,sizeof(SMeasurement),compareFreq);

	int lastPhase = -1;
	int addPhase = 0;
	int a = 0;
	for (int i = 0;i<numMeasurements;i++)
	{
		if (measurements[i].phase + addPhase - lastPhase > 100) addPhase-=180;
		if (measurements[i].phase + addPhase - lastPhase < -100) addPhase+=180;
		corrected[a].phase = measurements[i].phase + addPhase; 
		corrected[a].freq = measurements[i].freq; 
		corrected[a].wavelength = 300000.0/(measurements[i].freq*500+902750);
		a++;
		lastPhase = measurements[i].phase + addPhase;
	}
	numMeasurements = a;
	for (int i = 0;i<numMeasurements;i++)
	{
		for (int j = i+1;j<numMeasurements;j++)
		{
			if (abs(corrected[i].freq-corrected[j].freq) > 48)
			{
				float estima = ((corrected[i].phase-corrected[j].phase)/180.0*corrected[i].wavelength*corrected[j].wavelength)/(corrected[i].wavelength-corrected[j].wavelength);
			}
		}
	}

	float sx,sy,sxx,sxy;
	sx=sy=sxx=sxy=0;
	for (int i = 0; i < numMeasurements; i++)
	{
		sx += corrected[i].wavelength; 
		sy += corrected[i].phase/180.0;
		sxx += corrected[i].wavelength*corrected[i].wavelength; 	
		sxy += corrected[i].wavelength*corrected[i].phase/180.0; 
	}
	printf("%f %f %f %f %i\n",sxx,sxy,sx,sy,a);

	slope = (numMeasurements * sxy - sx * sy) / (numMeasurements * sxx - sx*sx);
	intercept = sy/numMeasurements - slope * sx/numMeasurements;
	printf("%f %f\n",slope,intercept);

}



