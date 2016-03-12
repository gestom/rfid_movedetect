#include "CPhaseModelSet.h"

using namespace std;
static bool debug = false; 

CPhaseModelSet::CPhaseModelSet()
{
	numModels = 0;
	active = NULL;
}

CPhaseModelSet::~CPhaseModelSet()
{
}

float CPhaseModelSet::addMeasurement(const char *id,int rssi,int freq,int phase)
{
	if (find(id) == -1)
	{
		models[numModels] = new CPhaseModelNaive(id);
		active = models[numModels];
		numModels++;
	}
	return active->addMeasurement(rssi,freq,phase);
}

ETagEvent CPhaseModelSet::readEvent(const char *id)
{
	if (find(id) == -1) return TAG_EVENT_NONE;
	return active->readEvent();
}
int CPhaseModelSet::find(const char *id)
{
	int i = 0;
	active = NULL;
	while (i<numModels && strcmp(id,models[i]->ID)) i++;
	if (i==numModels) return -1;
	active = models[i];
	return i;
}

void CPhaseModelSet::print(int verbosity)
{
	printf("MODELS: %i\n",numModels);	
	for (int i = 0;i<numModels;i++) printf("ID: %s moved: %i\n",models[i]->ID,models[i]->moved);	
}
