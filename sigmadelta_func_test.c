#include <stdio.h>
#include <math.h>

const int oversample = 32;
#define M_PI 3.14159265358979323846
extern int sigmadelta_func(int s);

//2nd order delta-sigma DAC
//See http://www.beis.de/Elektronik/DeltaSigma/DeltaSigma.html for a nice explanation
static int sampToI2sDeltaSigma(int s) {
	int x;
	int val=0;
	int w;
	static int i1v=0, i2v=0;
	static int outReg=0;
	for (x=0; x<oversample; x++) {
		val<<=1; //next bit
		w=s;
		if (outReg>0) w-=32767; else w+=32767; //Difference 1
		//if (outReg>0) w-=1.0; else w+=1.0; //Difference 1

		w+=i1v; i1v=w; //Integrator 1
		if (outReg>0) w-=32767; else w+=32767; //Difference 2
		//if (outReg>0) w-=1.0; else w+=1.0; //Difference 2
		w+=i2v; i2v=w; //Integrator 2
		outReg=w;		//register
		if (w>0) val|=1; //comparator
	}
	return val;
}

//Topological Variant – Feed-Forward with Extra Input Feed-In
//check: http://classes.engr.oregonstate.edu/eecs/spring2010/ece627/Lectures/2nd%20&%20Higher-Order2.pdf
static int sampToI2sDeltaSigma_type2(float s) {
	int x;
	int val=0;
	float w;
	float outReg=0;
	static float i1v=0, i2v=0;
	static int lastbitout=0;
	for (x=0; x<oversample; x++) {
		val<<=1; //next bit
		w=s;
		if (lastbitout) w-=1.0; else w+=1.0; //Difference 1
		outReg=s+i1v+i2v;

		i2v = i1v;
		i1v = w;		

		//register
		if (outReg>0) {
			lastbitout = 1;
			val|=1; //comparator
		} else {
			lastbitout = 0;
		}

	}
	return val;
}


void main()
{
	int samples = 3900;
	float samplerate = 3.9e6;
	float frequency1 = 1e6;
	float frequency2 = 250e3;
	float frequency3 = 500e3;

	float s,max=0;
	int s_int;
	int sigmadelta;	
	int i,j;
	
	for(i=0; i < samples; i++)
	{
		s = sin(2.0*M_PI*(float)i/samplerate*frequency1);
		s += sin(2.0*M_PI*(float)i/samplerate*frequency2);
		s += sin(2.0*M_PI*(float)i/samplerate*frequency3);

		if(s>max) max = s;
	}

	fprintf(stderr,"Max. amplitude=%f\n",max);
		
	for(i=0; i < samples; i++)
	{
		s = sin(2.0*M_PI*(float)i/samplerate*frequency1);
		s += sin(2.0*M_PI*(float)i/samplerate*frequency2);
		s += sin(2.0*M_PI*(float)i/samplerate*frequency3);

		s /= max;
		s *= 0.9;


		if(s > 1.0) s = 1.0;
		if(s < -1.0) s = -1.0;
//		if(s > 0) s_int = s*(float)1024; else s_int = s*(float)1024;
		s_int = s*1024;
		sigmadelta = sigmadelta_func(s_int);
//		sigmadelta = sampToI2sDeltaSigma(s*(float)32767);

		for(j=0; j < oversample; j++)
		{
			if(sigmadelta & 0x80000000)
			{
				printf("%f, 1\n",s);
			} else {
				printf("%f, -1\n",s);
			}

			sigmadelta <<= 1;

		}
	}
}


