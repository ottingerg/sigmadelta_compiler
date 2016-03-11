/*
 * optimized SigmaDelta Function Compiler for ARM
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

#define INPUTSAMPLE "r1"
#define INTEGRATOR1 "r2"
#define INTEGRATOR2 "r3"
#define OUTPUTVALUE "r4"

#define SAMPLEMAXVALUE "1024"
#define SAMPLEMINVALUE_NEG "1024"

#define OVERSAMPLEVALUE 32


void main()
{
	int x=16;

	printf("int sigmadelta_func(int s)\n");
	printf("{\n\n");
	printf("\tstatic int integrator1_store=0, integrator2_store=0;\n");

#ifdef ARM_ASM
	printf("\tregister int inputsample asm(\""INPUTSAMPLE"\");\n");
	printf("\tregister int integrator1 asm(\""INTEGRATOR1"\");\n");
	printf("\tregister int integrator2 asm(\""INTEGRATOR2"\");\n");
	printf("\tregister int outputvalue asm(\""OUTPUTVALUE"\");\n\n");
#else
	printf("\tint test;\n\n");

	printf("\tregister int inputsample;\n");
	printf("\tregister int integrator1;\n");
	printf("\tregister int integrator2;\n");
	printf("\tregister int outputvalue;\n\n");
#endif

	printf("\t//PROLOGUE\n");
	printf("\toutputvalue = 0;\n");
	printf("\tinputsample = s;\n");
	printf("\tintegrator1 = integrator1_store;\n");
	printf("\tintegrator2 = integrator2_store;\n\n");

	printf("\t//WORKING SECTION\n");

#ifdef ARM_ASM
	printf("\t__asm__ (\n");

	printf("\t\"tst "INTEGRATOR2", #0\\n\\t\"\n\n");
	
	for(x = 0; x < OVERSAMPLEVALUE; x++)
	{
		printf("\n\t\"bit%d_test:\\n\\t\"\n",x);
		printf("\t\"bpl bit%d_set\\n\\t\"\n\n",x);

		printf("\t\"bit%d_clr:\\n\\t\"\n",x);
		printf("\t\"add "INTEGRATOR1","INTEGRATOR1","INPUTSAMPLE" //integrator1 += inputsample;\\n\\t\"\n");
		printf("\t\"add "INTEGRATOR1","INTEGRATOR1",#"SAMPLEMAXVALUE" //integrator1 += SAMPLEMAXVALUE;\\n\\t\"\n");
		printf("\t\"add "INTEGRATOR2","INTEGRATOR2",#"SAMPLEMAXVALUE" //integrator2 += SAMPLEMAXVALUE;\\n\\t\"\n");
		printf("\t\"adds "INTEGRATOR2","INTEGRATOR2","INTEGRATOR1" //integrator2 += integrator1;\\n\\t\"\n");

		if( (x+1) != OVERSAMPLEVALUE ) {
			printf("\t\"b bit%d_test\\n\\t\"\n",x+1);
		} else {
			printf("\t\"b done\\n\\t\"\n");
		}

		printf("\n\t\"bit%d_set:\\n\\t\"\n",x);
		printf("\t\"orr "OUTPUTVALUE","OUTPUTVALUE",#0x%08X //integrator1 += inputsample;\\n\\t\"\n",1<<(OVERSAMPLEVALUE-1-x));
		printf("\t\"add "INTEGRATOR1","INTEGRATOR1","INPUTSAMPLE" //integrator1 += inputsample;\\n\\t\"\n");
		printf("\t\"sub "INTEGRATOR1","INTEGRATOR1",#"SAMPLEMINVALUE_NEG" //integrator1 -= SAMPLEMINVALUE_NEG;\\n\\t\"\n");
		printf("\t\"sub "INTEGRATOR2","INTEGRATOR2",#"SAMPLEMINVALUE_NEG" //integrator2 -= SAMPLEMINVALUE_NEG;\\n\\t\"\n");
		printf("\t\"adds "INTEGRATOR2","INTEGRATOR2","INTEGRATOR1" //integrator2 += integrator1;\\n\\t\"\n\n");


	}

	printf("\n\t\"done:\\n\\t\"\n");
	printf("\t);\n\n");	
#else
    printf("\tif(integrator2 > 0) test = 1; else test = 0;\n");
	for(x = 0; x < OVERSAMPLEVALUE; x++)
	{
		printf("\tif(test) {\n");
		printf("\t\toutputvalue	|= 0x%08X;\n",1<<(OVERSAMPLEVALUE-1-x));
		printf("\t\tintegrator1 += inputsample;\n");
		printf("\t\tintegrator1 -= "SAMPLEMINVALUE_NEG";\n");
		printf("\t\tintegrator2 -= "SAMPLEMINVALUE_NEG";\n");
		printf("\t\tintegrator2 += integrator1;\n");
		
		printf("\t} else {\n");

		printf("\t\tintegrator1 += inputsample;\n");
		printf("\t\tintegrator1 += "SAMPLEMAXVALUE";\n");
		printf("\t\tintegrator2 += "SAMPLEMAXVALUE";\n");
		printf("\t\tintegrator2 += integrator1;\n");
	
		printf("\t}\n");
		
		printf("\tif(integrator2 > 0) test = 1; else test = 0;\n");
	}
#endif

	printf("\t//EPILOGUE\n");
	printf("\tintegrator1_store = integrator1;\n");
	printf("\tintegrator2_store = integrator2;\n\n");

	printf("\treturn outputvalue;\n");
	printf("}\n");
	


}
