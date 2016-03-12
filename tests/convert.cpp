#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long int timeradio = 0;
long int timepose = 0;
long int lastTimepose = 0;
long int firstTimepose = 0;
float radio,angle,lastAngle;
float approxAngle;
char dum[1000];

const int width = 750;
const int height = 750;

float grid[width][height];
float aux[width][height];
float img[width][height];
float sen[width][height];

int main(int argc,char *argv[])
{
	memset(grid,0,width*height*sizeof(float));
	memset(aux,0,width*height*sizeof(float));
	for (int x = 0;x<width;x++)
	{
		for (int y = 0;y<height;y++){
			grid[x][y] = 0.5;
		}	
	}
	FILE *angleFile = fopen(argv[1],"r");
	FILE *radioFile = fopen(argv[2],"r");
	fscanf(angleFile,"%ld %f\n",&lastTimepose,&lastAngle);
	fscanf(angleFile,"%ld %f\n",&timepose,&angle);
	fscanf(radioFile,"%ld %s %f %s %s\n",&timeradio,dum,&radio,dum,dum);
	while (timeradio >lastTimepose) fscanf(radioFile,"%ld %s %f %s %s\n",&timeradio,dum,&radio,dum,dum);
	firstTimepose = lastTimepose;
	int iterations = 0;
	while (feof(radioFile)==0 && feof(angleFile)==0)
	{
		fscanf(radioFile,"%ld %s %f %s %s\n",&timeradio,dum,&radio,dum,dum);
		while (timepose < timeradio){
			lastTimepose = timepose;
			lastAngle = angle;
			fscanf(angleFile,"%ld %f\n",&timepose,&angle);
		}
		if (angle - lastAngle > +2*M_PI) lastAngle += 2*M_PI;
		if (angle - lastAngle < -2*M_PI) lastAngle -= 2*M_PI;
		//printf("Time: %ld - %ld - %ld\n",(timepose-firstTimepose)/1000000,(timeradio-firstTimepose)/1000000,(lastTimepose-firstTimepose)/1000000);
		approxAngle = (angle-lastAngle)*((float)(timeradio-lastTimepose))/((float)(timepose-lastTimepose))+lastAngle;
		//printf("Time: %.8f - %.8f - %.8f\n",angle,approxAngle,lastAngle);
		printf("%i %.8f %.0f\n",iterations,approxAngle,radio);

		if (iterations++%10==0)
		{
			float px = 3;
			float py = 3;
			float sx,sy=0;
			float angle,aI,dI,a,range;
			//for (float an = 0;an<2*M_PI;an++)
			//{
			for (int x = 0;x<width;x++){ 		
				for (int y = 0;y<height;y++)
				{
					range = (-radio-40)/20;
					sx = x/125.0-px+cos(approxAngle)*range/2;
					sy = y/125.0-py+sin(approxAngle)*range/2;
					angle = atan2(sy,sx)-approxAngle;
					//a = fabs(sqrt(sx*sx + sy*sy)-range/2.0*(sin(angle)-0.5)*(sin(angle)-0.5))/1.5;
					a = expf(-fabs(sqrt(sx*sx + sy*sy)-range));
					grid[x][y] = ((a-0.7)+aux[x][y]*grid[x][y])/(1+aux[x][y]);
					sen[x][y] = a;
					if (grid[x][y] < 0) grid[x][y]=0; 
					aux[x][y]++;
//					if ( a < 1) grid[x][y] = 1-(1-0.1*(1-a))*(1-grid[x][y]);//(1-a/2.0);
//					if ( a > 1) grid[x][y] =  (expf((1-a)*0.1))*grid[x][y];
					//if ( a < 1) grid[x][y] = 1-(1-0.1*(1-a))*(1-grid[x][y]);
					//if ( a < 1) grid[x][y] = 1-(1-0.1*(1-a))*(1-grid[x][y]);
					//if ( a > 1) grid[x][y] = 1-(1-0.1*(1-a))*(1-grid[x][y]);
				}
			}	
			//}
			float gMax = 0;
			for (int x = 0;x<width;x++){ 		
				for (int y = 0;y<height;y++)
				{
					gMax = fmax(grid[x][y],gMax);
				}
			}

			for (int x = 0;x<width;x++){ 		
				for (int y = 0;y<height;y++)
				{
					img[x][y]=grid[x][y]/gMax;
				}
			}
			char name[100];
			sprintf(name,"maps/%08i.pgm",iterations);
			FILE* file = fopen(name,"w+");
			if (file == NULL)
			{
				fprintf(stderr,"File %s could not be opened for writing.",name);
				return -1;
			}

			fprintf(file,"P6\n");
			fprintf(file,"%i %i\n",width,height);
			fprintf(file,"255\n");

			float prob;
			unsigned char *dat = (unsigned char*)malloc(3*sizeof(unsigned char)*width*height);
			int x,y;
			for (int i = 0;i<width*height;i++)
			{
				x = i%width;
				y = i/width;
				dat[3*i] = 100*sen[x][y];//200*(1-img[x][y]);
				dat[3*i+1] = 200*img[x][y];
				dat[3*i+2] = 0;//200*img[x][y];
				float dd =  (sqrt((x-width/2)*(x-width/2)+(y-height/2)*(y-height/2)));
				if (dd > 20 && dd < 25)
				{
				       	dat[3*i] =  255;
					dat[3*i+1] = 0;
					dat[3*i+2] = 0; 
				}
				dd =  (sqrt((x-width/2+12)*(x-width/2+12)+(y-height/2+130)*(y-height/2+130)));
				if (dd > 0 && dd <5)
				{
				       	dat[3*i] = 0;
					dat[3*i+1] = 0;
					dat[3*i+2] =  255; 
				}
			}
			int pos = 0;
			for (int j = 0;j<80;j++){
				for (int k = -5;k<=5;k++){
					x=width/2+j*cos(approxAngle+M_PI)/2-k*sin(approxAngle+M_PI)/2;
					y=height/2+j*sin(approxAngle+M_PI)/2+k*cos(approxAngle+M_PI)/2;
					pos = 3*(y*width+x);
					dat[pos] =  255;
					dat[pos+1] = 0;
					dat[pos+2] = 0; 
				}
			}
			for (int j = 0;125*j<width;j++){
				for (int i = 0;i<width;i++){
					pos = 3*(j*125*width+i);
					dat[pos+0] = 50;
					dat[pos+1] = 50;
					dat[pos+2] = 50;
				}
			}
			for (int j = 0;j<height;j++){
				for (int i = 0;125*i<width;i++){
					pos = 3*(j*width+i*125);
					dat[pos+0] = 50;
					dat[pos+1] = 50;
					dat[pos+2] = 50;
				}
			}
			fwrite(dat,sizeof(unsigned char)*width*height*3,1,file);
			free(dat);
			fclose(file);
		}
	}
	fclose(radioFile);
	fclose(angleFile);

	return 0;
}
