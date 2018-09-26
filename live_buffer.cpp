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

//define function to only keep those frames with certain number of detections:
int NdetsFilter(const int Ndets,const int ImgSize, std::vector<int> & NdetsCoords, unsigned char* & buffercopy)
{
	//make sure to pass NdetsCoords as reference
	//make sure buffer copy is correct at input
	int detsThisFrame = 0;
	int output = 0;
	
	for (int i = 0; i < ImgSize; i++)
	{
		if (*buffercopy > 1) {
			// multiple dets on same pixel (should never be possible for int time < dead time)
			output = 3;
			return output;
		}
		else if (*buffercopy == 1) {
			detsThisFrame++;
			if (detsThisFrame > Ndets) {
				// more than Ndets detections
				output = 2;
				return output;
			}
			NdetsCoords[detsThisFrame - 1] = i;
		}
		buffercopy++;
	}
	if (detsThisFrame < Ndets) {
		output = 1;
	}
	
	return output;
}

//define crude printing loop function (as sanity check):
int sanityCheckImages(const int imgWidth, const int imgHeight, unsigned char* buffer, const int numFrames, const int imgs2print) {
	
	for (int i = 0; i < numFrames; i++)
	{
		if (i < imgs2print) {
			//printf("Image %d: \n", i + 1);
			for (int j = 0; j < imgHeight; j++)
			{
				for (int k = 0; k < imgWidth; k++)
				{
					printf("%d ", *buffer);
					buffer++;
				}
				printf("\n");
			}
			printf("\n");
		}
		
		else
			break;
	}

	return 0;
}

int main(void)
{
	// Initialise variables
	//-----------------------------
	SPC3_H spc3 = NULL;
	UInt16* Img = NULL;
	Img = (UInt16*)calloc(1, 2048 * sizeof(UInt16));

	double read_bytes = 0, total_bytes = 0;
	int i = 0, j = 0, k = 0, out = 0;
	short trig = 0;
	double gateoff = 0;
	double* data = NULL;
	char c = 0, *fname = NULL;
	BUFFER_H buffer = NULL;
	FILE* f = NULL;

	//define pointer where we can duplicate buffer:
	unsigned char* dupBuffer_p = NULL;
		
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

	const int coincWanted = 2; //how many detections to make a coincidence (ie 2, for 20+02 state)
	int sanityCheckCounter = 0;
	
	const int imgWidth = 64, imgHeight = 32; //how many pixels is the image wide/high; hardcoded for now - adjust as necessary
	const int pixelsPerFrame = imgWidth*imgHeight, bytesPerPixel = 1; //hardcoded for now - adjust as necessary
	const int bytesPerFrame = pixelsPerFrame * bytesPerPixel;
	std::vector<int> coordsCoinc(coincWanted);
	std::vector<int> coordsAll2fold; // for now just make more of these vectors if we want higher N-fold coincs
	


	//start continuous acquisition
	SPC3_Start_ContAcq_in_Memory(spc3);
	for (i = 1; i < 11; i++)
	{
		if (SPC3_Get_Memory_Buffer(spc3, &read_bytes, &buffer) == OK)
		{
			total_bytes = total_bytes + read_bytes;
			printf("Acquired %f bytes in %d readout operation\n", total_bytes, i);

			double numFramesdbl = read_bytes / bytesPerFrame;
			numFramesdbl += 0.5;
			int numFrames = (int)numFramesdbl; //cast numFramesRead as integer
			printf("Acquired %d frames this iteration\n", numFrames);

			if (read_bytes != 0)
			{
				dupBuffer_p = buffer; //check if duplicating buffer works - yes it does
				//sanityCheckImages(imgWidth, imgHeight, dupBuffer_p, numFrames, 3);
				
				for (j = 0; j < numFrames; j++) {
					dupBuffer_p = buffer + j * pixelsPerFrame;
					if (NdetsFilter(coincWanted, pixelsPerFrame, coordsCoinc, dupBuffer_p) == 0) {

						if (sanityCheckCounter < 0) { //only do sanity check few times
							printf("condition satisfied at frame %d: \n", j + 1);
							sanityCheckImages(imgWidth, imgHeight, buffer + j * pixelsPerFrame, numFrames, 1);
							sanityCheckCounter++;
						}

						for (k = 0; k < coincWanted; k++) {
							coordsAll2fold.push_back(coordsCoinc[k]); //can add higher N-fold coinc vectors here later
						}
						//break;
					}
				}
			}
			SLEEP(1 * MILLIS);

			//insert code here that processes coords, upsamples and into OCM
		}
		else
			break;
	}
	SPC3_Stop_ContAcq_in_Memory(spc3);

	printf("2-fold coords vector is %d elements long. \n ", coordsAll2fold.size());

	// Destructors
	//----------------------------
	if (spc3)
		SPC3_Destr(spc3);
	//free(dupBuffer_p);
	spc3 = NULL;
	printf("Press ENTER to continue\n");
	getchar();
	return 0;
}