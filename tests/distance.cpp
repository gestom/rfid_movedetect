#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LEN 10000
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

int compareFreq(const void* m1,const void* m2)
{
	if (((SMeasurement*)m1)->freq  < ((SMeasurement*)m2)->freq)  return -1;
	if (((SMeasurement*)m1)->freq  > ((SMeasurement*)m2)->freq)  return +1;
	if (((SMeasurement*)m1)->phase < ((SMeasurement*)m2)->phase) return -1;
	if (((SMeasurement*)m1)->phase > ((SMeasurement*)m2)->phase) return +1;
	return 0;
}

int comparePhase(const void* m1,const void* m2)
{
	if (((SMeasurement*)m1)->phase < ((SMeasurement*)m2)->phase) return -1;
	if (((SMeasurement*)m1)->phase > ((SMeasurement*)m2)->phase) return +1;
	if (((SMeasurement*)m1)->freq  < ((SMeasurement*)m2)->freq)  return -1;
	if (((SMeasurement*)m1)->freq  > ((SMeasurement*)m2)->freq)  return +1;
	return 0;
}

SMeasurement measurements[LEN];
SMeasurement corrected[LEN];
int numMeasurements;
char dum[1000];

float distance;
int waves[FREQS];
int phase[FREQS];
int freqs[FREQS];

int main(int argc,char *argv[])
{
	FILE *radioFile = fopen(argv[1],"r");
	int rssi,phi,freq;
	/*read stuff*/
	while (feof(radioFile)==0)
	{
		fscanf(radioFile,"%s %i %i %i %s %s\n",dum,&rssi,&phi,&freq,dum,dum);
		measurements[numMeasurements].strength =  rssi; 
		measurements[numMeasurements].phase =  phi; 
		//	measurements[numMeasurements].wavelength =  300000.0/freq;
		measurements[numMeasurements].freq =  (freq-902750)/500;
		numMeasurements++;
	}
	fclose(radioFile);

	/*analysis*/
	qsort(measurements,numMeasurements,sizeof(SMeasurement),comparePhase);
	qsort(measurements,numMeasurements,sizeof(SMeasurement),compareFreq);

	/*tag frequencies*/

	int lastPhase = -1;
	int addPhase = 0;
	//for (int i = 0;i<numMeasurements;i++) printf("%i %i\n",measurements[i].freq,measurements[i].phase);
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
				//	printf("Estim: %f\n",estima);
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

	float slope = (numMeasurements * sxy - sx * sy) / (numMeasurements * sxx - sx*sx);
	float intercept = sy/numMeasurements - slope * sx/numMeasurements;

	float error = 0;
	for (int i = 0;i<numMeasurements;i++)
	{
		error += fabs(slope*corrected[i].wavelength+intercept-corrected[i].phase/180.0);
	}
	error /= numMeasurements;
	printf("%f %f %f\n",slope,intercept,error);

	char filename[100];
	strcpy(filename,argv[1]);
	strncpy(strstr(filename,".txt"),".dat",4);
	radioFile = fopen(filename,"w");
	for (int i = 0;i<numMeasurements;i++) fprintf(radioFile,"%.4f %.3f %.3f\n",corrected[i].wavelength,corrected[i].phase/180.0,corrected[i].wavelength*slope+intercept);
	fclose(radioFile);
	return 0;
}
