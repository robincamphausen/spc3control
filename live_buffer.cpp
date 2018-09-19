// Live mode - accessing buffer
//--------------------------------

#include "SPC3_SDK.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
//#include <iostream>

#if defined(__linux__)
#define SLEEP usleep
#define MILLIS 1000
#elif defined(__APPLE__)
#define SLEEP usleep
#define MILLIS 1000
#include <unistd.h>
#elif defined(_WIN32)
#include <Windows.h>
#define SLEEP Sleep
#define MILLIS 1
#endif

int NdetsFilter(int Ndets, int ImgSize, int* NdetsCoords, unsigned char* buffercopy)
{
	int detsThisFrame = 0;
	int output = 0;

	return output;
}


int main(void)
{
	// Initialise variables
	//-----------------------------
	SPC3_H spc3 = NULL;
	UInt16* Img = NULL;// , hist[65535], AppliedDT = 0;
	char header[1024] = "";
	int integFrames = 10;
	double read_bytes = 0, total_bytes = 0;
	int i = 0, j = 0, k = 0, out = 0;
	short trig = 0;
	double gateoff = 0;
	double *x = NULL, *y = NULL, *Imgd = NULL;
	double* data = NULL;
	char c = 0, *fname = NULL;
	BUFFER_H buffer = NULL;
	FILE* f = NULL;
	//time_t start, stop;
	//int* intBuffer;

	Img = (UInt16*)calloc(1, 2048 * sizeof(UInt16));

	//SPC3 constructor and parameter setting (make sure you check the SPC3 ID)
	char spc3ID[] = "1805000L20";
	char * spc3ID_p = spc3ID;
	out = (int)SPC3_Constr(&spc3, Advanced, spc3ID_p);

	/*
	SPC3_Set_Camera_Par 	( 	SPC3_H  	spc3,
								UInt16  	Exposure - in units of 10ns,
								UInt32  	NFrames - number of frames to be acquired in SNAP mode (in CONT ACQ this parameter doesn't do anything),
								UInt16  	NIntegFrames -  number of individual frames summed together to make image,
											(i.e. effective exposure time is NIntegFrames*Exposure),
								UInt16  	NCounters,
								State  	Force8bit,
								State  	Half_array,
								State  	Signed_data
		)
		*/
	SPC3_Set_Camera_Par(spc3, 10, 20, 1, 1, Disabled, Disabled, Disabled);
	SPC3_Set_DeadTime(spc3, 100);
	SPC3_Apply_settings(spc3);

	printf("Continuous acquisition will be started and 10 memory dumps performed.\n");

	//define pointer where we can duplicate buffer:
	unsigned char* dupBuffer_p = NULL;
	unsigned char* dupBuffer_p2 = NULL; //make another buffer pointer for printing out values (can get rid of this later)

	//start continuous acquisition
	
	SPC3_Start_ContAcq_in_Memory(spc3);
	for (i = 1; i < 11; i++)
	{
		if (SPC3_Get_Memory_Buffer(spc3, &read_bytes, &buffer) == OK)
		{
			total_bytes = total_bytes + read_bytes;
			printf("Acquired %f bytes in %d readout operation\n", total_bytes, i);


			//for now assume that pixel values are 8bit, and that always full sensor is acquired i.e. bytes/frame = 2048
			double numFramesdbl = read_bytes / 2048;
			numFramesdbl += 0.5;
			int numFrames = (int)numFramesdbl; //cast numFramesRead as integer
			printf("Acquired %d frames this iteration\n", numFrames, i);

			//check if buffer overwrites on each call of SPC3_Get_Memory_Buffer() - yes it does
			if (read_bytes != 0)
			{
				dupBuffer_p = buffer; //check if duplicating buffer works - yes it does
				
				for (int m = 0; m < numFrames; m++)
				{
					int sumFrame = 0;

					printf("Cycle %d image %d dupBuffer: \n \n", i,m+1);
					//print image using dupBuffer pointer
					for (j = 0; j < 32; j++)
					{
						for (k = 0; k < 64; k++)
						{
							printf("%d ", *dupBuffer_p);
							sumFrame += *dupBuffer_p;
							dupBuffer_p++;
						}
						printf("\n");
					}
					printf("Sum of this frame = %d \n \n", sumFrame);

				}
				break;

			}

			SLEEP(1 * MILLIS);
			
		}
		else
			break;
	}
	SPC3_Stop_ContAcq_in_Memory(spc3);

	// Destructors
	//----------------------------
	if (spc3)
		SPC3_Destr(spc3);
	free(Img);
	//free(dupBuffer_p);
//	free(Imgd);
//	free(data);
//	free(y);
//	free(x);
	spc3 = NULL;
	printf("Press ENTER to continue\n");
	getchar();
	return 0;
}