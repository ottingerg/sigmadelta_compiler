/*
 * Test for optimized SigmaDelta Function Compiler for ARM
 *
 * Author: Georg Ottinger <georg.ottinger@oteloegen.at>
 * Copyright 2016 - Georg Ottinger
 *
 * Inspired by work from Jeroen Domburg (Spritetm)
 * https://github.com/espressif/ESP8266_MP3_DECODER/blob/master/mp3/user/user_main.c
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <stdio.h>
#include <math.h>

#include "sigmadelta_func.h"

const int oversample = 32;
#define M_PI 3.14159265358979323846


int main()
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


	//find max amplitude for normalization process
	
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


		s /= max; //normalize sample
		s *= 0.9; //90% max amplitude


		if(s > 1.0) s = 1.0; // check upper range
		if(s < -1.0) s = -1.0; // check lower range
		s_int = s*1024; //convert float to integer

		sigmadelta = sigmadelta_func(s_int);

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

	return 0;
}


