#include "CPhaseModelNaive.h"

using namespace std;
static float alpha = 0.2;
static float threshold = 3.0;

CPhaseModelNaive::CPhaseModelNaive(const char* id)
{
	filteredError = 0;
	strcpy(ID,id);
	moved = ready = false;
	memset(phaseMean,0,sizeof(float)*FREQS);
	memset(phaseStd,0,sizeof(float)*FREQS);
	memset(rssiMean,0,sizeof(float)*FREQS);
	memset(rssiStd,0,sizeof(float)*FREQS);
	memset(measurements,0,sizeof(int)*FREQS);
	ROS_INFO("TAG %s detected.",ID);
	report = TAG_EVENT_DETECTED;
}

CPhaseModelNaive::~CPhaseModelNaive()
{
}

void CPhaseModelNaive::reset()
{
	memset(measurements,0,sizeof(int)*FREQS);
}

float CPhaseModelNaive::addMeasurement(int rssi,int freq,int phs)
{
 	int index = (freq-902750)/500;
	int nums  = measurements[index]; 
	if (fabs(phs-phaseMean[index]-180) < fabs(phs-phaseMean[index])) phs -= 180;
	if (fabs(phs-phaseMean[index]+180) < fabs(phs-phaseMean[index])) phs += 180;
	phaseMean[index] = (nums*phaseMean[index] + phs)/(nums+1);
	phaseStd[index] = (nums*phaseStd[index] + fabs(phs-phaseMean[index]))/(nums+1);
	if (measurements[index] < 10) measurements[index]++;
	int learning = 1;
	for (int i = 0;i<FREQS;i++) learning = learning*((int)(measurements[i]>0));
	if (learning > 0 && ready ==false)
	{
		ROS_INFO("TAG %s model estimation finished.",ID);
		ready = true;
		report = TAG_EVENT_READY;
	}
	if (phaseStd[index] > 0){
		float error = fabs(phaseMean[index] - phs)/phaseStd[index];
		filteredError = (1-alpha)*filteredError+alpha*error;
		if (filteredError > threshold){
			ROS_INFO("TAG %s was moved.",ID);
			moved = true;
			report = TAG_EVENT_MOVED;
			//reset(); //should we relearn the model ?
			return -error;
		}
		return error;
	}
	return 0;
}

ETagEvent CPhaseModelNaive::readEvent()
{
	ETagEvent repo = report;
	report = TAG_EVENT_NONE;
	return repo; 
}

float CPhaseModelNaive::evaluateMeasurement(int rssi,int freq,int phase)
{
}

void CPhaseModelNaive::buildModel()
{
}

void CPhaseModelNaive::print()
{
	for (int i = 0;i<FREQS;i++) if (phaseMean[i] < 0) phaseMean[i]+=180;
	for (int i = 0;i<FREQS;i++) if (phaseMean[i] > 180) phaseMean[i]-=180;
	for (int i = 0;i<FREQS;i++) printf("%i %.3f %.3f %.3f %.3f %i\n",i*500+902750,phaseMean[i],phaseStd[i],phaseMean[i]+phaseStd[i],phaseMean[i]-phaseStd[i],measurements[i]);
}
