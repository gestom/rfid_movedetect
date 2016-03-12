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

SMeasurement measurements[LEN];
SMeasurement corrected[LEN];
int numMeasurements;
char dum[1000];

float distance;
int waves[FREQS];
int phase[FREQS];
int freqs[FREQS];


int ransac(SMeasurement *measurements,int numMeasurements,float *slopeA,float *interceptA,SMeasurement* outliers,int *numOutliers)
{
	/*tag frequencies*/
	int lastPhase = -1;
	int addPhase = 0;
	float slope,intercept,bestFit,err;
	bestFit = -1;
	int fits = 0;
	for (int att = 0;att<10000;att++){
		//RANSAC
		int ptA = rand()%numMeasurements;
		int ptB = rand()%numMeasurements;
		int i;
		float slopeHyp,interceptHyp;
		if (fabs(measurements[ptA].freq - measurements[ptB].freq) < 250){
			slopeHyp = (float)(measurements[ptA].phase - measurements[ptB].phase)/(measurements[ptA].wavelength - measurements[ptB].wavelength);
			//printf("%i %i %i %i\n",measurements[ptA].freq,measurements[ptB].freq,measurements[ptA].phase,measurements[ptB].phase);
			interceptHyp = measurements[ptA].phase-measurements[ptA].wavelength*slopeHyp;
			i=ptA;
			err = fabs(slopeHyp*measurements[i].wavelength+interceptHyp-measurements[i].phase);
			//printf("%f %f %f\n",slope,intercept,err);
			i=ptB;
			//printf("%f %f %f\n",slope,intercept,err);
			int fits = 0;
			for (int i = 0;i<numMeasurements;i++)
			{
				int value = ((int)(slopeHyp*measurements[i].wavelength+interceptHyp) + 1800)%180;
				value = fabs(value-measurements[i].phase);
			       	if (value<10) fits+=(10-value);
			}
			if (fits > bestFit)
			{
				slope = slopeHyp;		
				intercept = interceptHyp;
				bestFit = fits;
			}
			//printf("FITS: %i %i \n",fits,numMeasurements);
		}
	}
	int a = 0;
	for (int i = 0;i<numMeasurements;i++)
	{
		int value = ((int)(slope*measurements[i].wavelength+intercept) + 1800)%180;
		value = fabs(value-measurements[i].phase);
		if (value>=15)
		{
			outliers[a++] = measurements[i];
		}
	}
	*numOutliers = a;
	*slopeA = slope;
	*interceptA = intercept;
	return bestFit;
}

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
		measurements[numMeasurements].wavelength =  300000.0/freq;
		measurements[numMeasurements].freq =  (freq-902750)/500;
		//measurements[numMeasurements].freq =  freq;
		numMeasurements++;
	}
	fclose(radioFile);
	float slope,intercept;
	int numOutliers;
	int fits = ransac(measurements,numMeasurements,&slope,&intercept,corrected,&numOutliers);
	//printf("%f %f \n",slope,intercept);
	for (int i = 0;i<FREQS;i++)
	{
		printf("PROPER: %f %i\n",300000.0/(i*500+902750),((int)(slope*(300000.0/(i*500+902750))+intercept) + 1800)%180);
	}
//	fits = ransac(corrected,numOutliers,&slope,&intercept,corrected,&numOutliers);
	for (int i = 0;i<FREQS;i++)
	{
	//	printf("OUTLI: %i %i\n",i*500+902750,((int)(slope*i+intercept) + 1800)%180);
	}

	//distance calculation
	float lambda1 = 300000.0/902750;
	float lambda2 = 300000.0/927250;
	float distance = slope/180*lambda1*lambda2/8;
	printf("Distance %.3f\n",distance);
	return 0;

	int a=0;	
	//line fit bitch
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
