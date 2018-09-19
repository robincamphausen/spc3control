/*
#######################################
                                                 
Copyright 2017-2018 Micro-Photon-Devices s.r.l.                                 
                                                 
SOFTWARE PRODUCT: SPC3_SDK 1.1.6                                    
                                                 
Micro-Photon-Devices (MPD) expressly disclaims any warranty for the SOFTWARE PRODUCT.          
The SOFTWARE PRODUCT is provided 'As Is' without any express or implied warranty of any kind,      
including but not limited to any warranties of merchantability, noninfringement, or          
fitness of a particular purpose. MPD does not warrant or assume responsibility for the          
accuracy or completeness of any information, text, graphics, links or other items contained      
within the SOFTWARE PRODUCT. MPD further expressly disclaims any warranty or representation      
to Authorized Users or to any third party.                             
In no event shall MPD be liable for any damages (including, without limitation, lost profits,    
business interruption, or lost information) rising out of 'Authorized Users' use of or inability 
to use the SOFTWARE PRODUCT, even if MPD has been advised of the possibility of such damages.    
In no event will MPD be liable for loss of data or for indirect, special, incidental,          
consequential (including lost profit), or other damages based in contract, tort          
or otherwise. MPD shall have no liability with respect to the content of the              
SOFTWARE PRODUCT or any part thereof, including but not limited to errors or omissions contained 
therein, libel, infringements of rights of publicity, privacy, trademark rights, business      
interruption, personal injury, loss of privacy, moral rights or the disclosure of confidential      
information.                                             

#######################################
*/

#ifndef __SPC3_SDK_h__
#define __SPC3_SDK_h__

#ifdef __cplusplus
 extern "C" {
#endif
/**
\mainpage Single Photon Counting Camera Software Development Kit (SPC3-SDK).
SPC3 is a 2D imaging chip based on a 64 x 32 array of smart pixels. Each pixel comprises a single-photon avalanche diode detector,
an analog front-end and a digital processing electronics. This on-chip integrated device provides single-photon sensitivity,
high electronic noise immunity, and fast readout speed. The imager can be operated at a maximum of about 100.000 frame per second with negligible
dead-time between frames. It features high photon-detection efficiency in the visible spectral region, and low dark-counting rates,
even at room temperature. The imager is easily integrated into different applications thanks to the input optical adapter and a high-speed
USB 3.0 computer interface.The camera differs from conventional CCD or CMOS sensors because it performs a fully digital
acquisition of the light signal. Each pixel effectively counts the number of photons which are detected by the sensor during the acquisition time.

\b IMPORTANT
In order to execute a program which links to the SDK libraries, a set of DLL should be placed in the same directory as the executable. The list of the required files is:
\verbatim
SPC3_SDK.dll       Software development kit interface
okFrontPanel.dll            Low-level interface	
\endverbatim
*/

/**\file SPC3_SDK.h
SPC3 software development kit.
This C header contains all the functions to operate the SPC3 camera in user defined applications.
*/
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef DllSDKExport 
	#if defined(_WIN32)   
        #define DllSDKExport __declspec(dllexport)
	#else
		#define DllSDKExport __attribute__ ((visibility("default")))
	#endif
#endif

/** Unsigned short integer (16 bit)*/
typedef unsigned short UInt16;

/** Short integer (16 bit)*/
typedef short Int16;

/** Unsigned integer (32 bit)*/
typedef unsigned UInt32;

/** Minimum allowed dead-time in nanoseconds.
The darkcounts rate and after-pulsing probability depend on the used dead-time setting: the lower the dead-time, the higher the darkcounts rate and after-pulsing probability.
However, a long dead-time limits the maximum number of photons per second detected by the matrix of avalanche photodiodes. It is recommended to set this parameter 
to short values as e.g. 50 ns.
*/
#define MIN_DEAD_TIME 30
/** Maximum allowed dead-time in nanoseconds.
The dead-time is set to MAX_DEAD_TIME, when higher values are requested.
*/
#define MAX_DEAD_TIME 150

/** Maximum gate-width for internal gate generation, expressed in percentage of 20ns gate period.
*/
#define MAX_GATE_WIDTH 100

/** Mimimum gate-width for internal gate generation, expressed in percentage of 20ns gate period.
*/
#define MIN_GATE_WIDTH 0

/** Maximum gate shift for internal gate generation, expressed in thousenths of 20ns gate period.
*/
#define MAX_GATE_SHIFT +400

/** Minimum gate shift for internal gate generation, expressed in thousenths of 20ns gate period.
*/
#define MIN_GATE_SHIFT -400


/**
 * \defgroup Enum SPC3-SDK custom Types
 Custom types used by the SDK
 */
/*@{*/
/**Error table.
Error code returned by the SPC3 functions.
*/
typedef enum{
    OK = 0,                                   /**<The function returned successfully.*/
    USB_DEVICE_NOT_RECOGNIZED= -1,            /**<The USB device driver has not been initialized. Is there any device connected?*/
    CAMERA_NOT_POWERING_UP=-3,				  /**<Internal power supply is not powering up. Check connections and restart the camera. If problems persists contact MPD.*/
	COMMUNICATION_ERROR=-5,					  /**<Communication error during readout. Check USB connection.*/
    OUT_OF_BOUND = -6,                        /**<One or more parameters passed to the function are outside the valid boundaries.*/
    MISSING_DLL  = -7,                        /**<One or more SPC3 libraries are missing.*/
    EMPTY_BUFFER = -8,                        /**<An empty buffer image has been provided to the function.*/
    NOT_EN_MEMORY = -9,                       /**<Not enough memory is available to operate the camera.*/
    NULL_POINTER = -10,                       /**<A null pointer has been provided to the function.*/
    INVALID_OP = -11,                         /**<The required function can not be executed. The device could be still in "Live mode".*/
    UNABLE_CREATE_FILE = -12,                 /**<An output file can not be created.*/
    UNABLE_READ_FILE = -13,                   /**<The provided file can not be accessed.*/
    FIRMWARE_NOT_COMPATIBLE=-14,              /**<The camera firmware is not compatible with the current software.*/
    POWER_SUPPLY_ERROR = -15,				  /**<Voltage drop on internal power supply. Check connections and restart the camera. If problems persists contact MPD.*/
    TOO_MUCH_LIGHT = -16,                     /**<Too much light was detected by the camera. The protection mechanism has been enabled. 
											      Decrease the amount of light on the sensor. Then, disconnect and reconnect the camera to the USB port.*/   
	INVALID_NIMG_CORRELATION = -17,			  /**<The acquired number of images is not sufficient to calculate the required correlation function.*/	
	SPC3_MEMORY_FULL = -18						/**<The SPC3 internal memory got full during continous acquisition. Possible data loss.*/	
} SPC3Return;

/**Output file format.
Table of the available output file formats for the saved images
*/
typedef enum{
    SPC3_FILEFORMAT = 0,        /**< SPC3 custom file format: the first byte contains the value 8 or 16 to define whether the image has 8 or 16 bit per pixel.
                                     Then the pixel values follow in row-major order. The byte order is little-endian for the 16 bit images.*/
    TIFF_NO_COMPRESSION = 2     /**< Multipage TIFF without compression. The file follows the OME-TIFF specification. It may be read with any reader able to open TIFF file,
								but OME-TIFF compatible reader will also show embedded metadata on acquisition parameters. For more information, see the OME-TIFF web site:
								http://www.openmicroscopy.org/site/support/ome-model/ome-tiff/
								\b WARNING the creation of TIFF files might require long execution times.*/
} OutFileFormat;

///Gate setting
/** Enable and disable the software gating. When the setting is Pulsed, the SPC3 discards the detected photons by the SPAD matrix if measured outside a valid gate signal.*/
typedef enum{
    Continuous = 0,   /**<The gate signal is always valid.*/
    Pulsed = 1        /**<The gate signal is a a square wave. The photons, which are detected when the gate signal is "ON", are counted. Otherwise they are discarded.*/
} GateMode;

/**SPC3 working mode.
The camera contains for each pixel an 8-bit binary counter. If the exposure time is too long, the counter can overflow and generate a distorted image.
Therefore, two operating modes have been implemented: a "normal" one which prevents the overflow of the counters, and an advanced one which gives full control
of the camera to the user.
*/
typedef enum{
    Normal = 0,     /**<The camera settings are tuned by the software to avoid the overflow of the counters. In this working mode, the exposure time of each image is
                    fixed to a multiple of 10.40 microseconds. Longer exposures are obtained by integrating more frames.*/
    Advanced = 1    /**<The user has full control of the camera settings. \b WARNING: the counters can overflow.*/
} CameraMode;

/**Type of synchronization output.
The Synch-out SMA port can output different signals*/
typedef enum{
    None = 0,		/**<No output signal.*/
    Gate_Clk = 1,   /**<A square wave of 50 MHz and 50% duty cycle synchronized with the software gate signal and the camera clock.*/
    Frame = 2       /**<A 60 ns pulse every time a new frame is acquired.*/
} TriggerMode;

/**Disable or enable a specific function*/
typedef enum{
    Disabled = 0,
    Enabled = 1
} State;

/**Type of correlation function.
The SDK implements two autocorrelation algorithms which can be applied to the acquired sequence of images. The multi-tau autocorrelation
has been implemented according to Culbertson and Burden "A distributed algorithm for multi-tau autocorrelation.", Rev Sci Instrum 78, 044102 (2007)
(standard version) and the linear one similar to Press, Teukolsky, Vetterling and Flannery,"Numerical Recipes 3rd Edition: The Art of Scientific Computing.", (2007) "autocor.cpp".
*/
typedef enum{
    Linear = 0,        /**<Selects the linear correlation algorithm*/
    MultiTau = 1    /**<Selects the linear multi-tau algorithm*/
} CorrelationMode;


/**Handle to the SPC3 structure.*/
typedef struct _SPC3_H * SPC3_H;

/**Handle to the SPC3 buffer.*/
typedef  unsigned char * BUFFER_H;
/*@}*/
//------------ Constructor -----------------------------------------
/**
 * \defgroup ConDes Constructr, destructor and error handling
 Functions to construct and destruct SPC3 objects, and for error handling
 */
/*@{*/
/**Constructor.
It allocates a memory block to contain all the information and buffers required by the SPC3. If multiple devices are connected to the computer,
a unique camera ID should be provided to correctly identify the camera. The camera ID can be found in the camera documentation (9 numbers and a letter) 
and a list of connected device is printed on the screen upon calling this function. An empty string is accepted too. In this case, the first device on the list will be connected.
\param spc3_in Pointer to SPC3 handle
\param m Camera Working mode
\param Device_ID Unique ID to identify the connected device
\return OK
\return INVALID_OP The SPC3_H points to an occupied memory location
\return FIRMWARE_NOT_COMPATIBLE The SDK and Firmware versions are not compatible
\return NOT_EN_MEMORY There is not enough memory to run the camera
*/
DllSDKExport SPC3Return SPC3_Constr(SPC3_H* spc3_in, CameraMode m, char* Device_ID);

/**Destructor.
It deallocates the memory block which contains all the information and buffers required by the SPC3. \b WARNING the user must call the destructor before the end of the program
to avoid memory leakages.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Destr(SPC3_H spc3);

//------------ Error handling --------------------------------------
/**Print an error message.
All the SDK functions return an error code to inform the user whether the issued command was successfully executed or not. The result of the execution of a function
can be redirect to a text file by providing a valid file pointer.
\param fout Output text file
\param FunName Additional text to define the warning/error. Usually the name of the calling function is provided.
\param retcode Error code returned by a SDK command
*/
DllSDKExport void PrintErrorCode(FILE* fout, const char* FunName, SPC3Return retcode);

/*@}*/
//------------ Set methods -----------------------------------------
/**
 * \defgroup Set Set methods
 Functions to set parameters of the SPC3 camera
 */
/*@{*/
/**Set the acquisition parameters for the camera.
This function behaves differently depending on the operating mode setting. In case of Normal working mode, the exposure time is fixed to 10.40 microseconds.
Therefore, the parameter Exposure is not considered. Longer exposures are obtained by summing multiple frames (i.e. by setting NIntegFrames).
This operating mode does not degrade the signal to noise ratio. In fact, the camera does not have any read-out noise. 
In case of Advanced mode, all the parameters are controlled by the user which can set very long exposure times.
The time unit of the Exposure parameter is clock cycles i.e. the exposure time is an integer number of internal clock cycles of 10 ns periode. 
For example, the value of 10 means 100 ns exposure.
\param spc3 SPC3 handle.
\param Exposure Exposure time for a single frame. The time unit is 10 ns. Meaningful only for Advanced mode. Accepted values: 1 ... 65534
\param NFrames Number of frames per acquisition. Meaningful only for Snap acquistion. Accepted values: 1 ... 65534
\param NIntegFrames Number of integrated frames. Each output frame is the result of the sum of NIntegFrames.  Accepted values: 1 ... 65534
\param NCounters Number of counters per pixels to be used. Accepted values: 1 ... 3
\param Force8bit Force 8 bit per pixel acquisition. Counts are trunked. Meaningful only for Advanced mode.
\param Half_array Acquire only a 32x32 array.
\param Signed_data If enabled, data from counters 2 and 3 are signed data with 8bit integer part and 1 bit sign.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return OUT_OF_BOUND Exposure, NFrames and NIntegFrames must be all greater than zero and smaller than 65535
*/
DllSDKExport SPC3Return SPC3_Set_Camera_Par(SPC3_H spc3, UInt16 Exposure, UInt32 NFrames, UInt16 NIntegFrames, UInt16 NCounters, State Force8bit, State Half_array, State Signed_data);

/**Update the dead-time setting.
Every time a photon is detected in a pixel, that pixel remains blind for a fix amount of time which is called dead-time. This setting is user-defined and it ranges
from MIN_DEAD_TIME and MAX_DEAD_TIME. Only a sub-set of this range is practically selectable: a dead-time calibration is performed during the production of the device.
This function will set the dead-time to the closest calibrated value to Val.
The default dead-time value is 50 ns.
\param spc3 SPC3 handle
\param Val  New dead-time value in nanoseconds
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP Unable to change the dead-time when the live-mode is ON
*/
DllSDKExport SPC3Return SPC3_Set_DeadTime(SPC3_H spc3, UInt16 Val);

/**Enable or disable the dead-time correction.
The default setting is disabled.
\param spc3 SPC3 handle
\param s New state for the dead-time corrector
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Set_DeadTime_Correction(SPC3_H spc3, State s);

/**Change the operating mode.
Set the operating mode to Normal or Advanced. Normal mode is the default setting.
\param spc3 SPC3 handle
\param s Enable or disable the advanced mode
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location*/
DllSDKExport SPC3Return SPC3_Set_Advanced_Mode(SPC3_H spc3, State s);

/**Load a background image to perform hardware background subtraction.
The control electronics is capable of performing real-time background subtraction. A background image is loaded into the internal camera memory.
\param spc3 SPC3 handle
\param Img  Pointer to a 2048 UInt16 array containing the background image. \b WARNING The user should check the array size to avoid the corruption of the memory heap.
\return OK
\return NULL_POINTER The provided SPC3_H or Img point to an empty memory location
\return INVALID_OP Unable to set the background image when the live-mode is ON
*/
DllSDKExport SPC3Return SPC3_Set_Background_Img(SPC3_H spc3, UInt16* Img);

/**Enable or disable the hardware background subtraction.
\param spc3 SPC3 handle
\param s Enable or disable the background subtraction
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Set_Background_Subtraction(SPC3_H spc3, State s);

/**Change the Gate settings.
A gate signal is generated within the control electronics to select valid photons, i.e. only photons which arrives when the Gate is ON
are counted. The gate signal is a 50 MHz square wave: shift and length define the phase and duty-cycle of the signal.
\param spc3 SPC3 handle
\param Shift Phase shift of the gate signal in the ON state. The unit is thousenths, i.e. 10 means a delay time of 0.01 times a 20 ns periodic signal,
 which is equal to 200ps. Accepted values: -400 ... +400
\param Length Duration of the ON gate signal. The unit is percentage.  Accepted values: 0 ... 100
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return OUT_OF_BOUND Shift or length are outside the valid values
*/
DllSDKExport SPC3Return SPC3_Set_Gate_Values(SPC3_H spc3, Int16 Shift, Int16 Length);

/**Set the gate mode either continuous or pulsed
\param spc3 SPC3 handle
\param Mode New gate mode
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Set_Gate_Mode(SPC3_H spc3, GateMode Mode);

/**Select the output signal.
\param spc3 Pointer to the SPC3 handle
\param Mode New trigger mode
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Set_Trigger_Out_State(SPC3_H spc3, TriggerMode Mode);

/**Set the sync-in state.
Set the camera to wait for an input trigger signal before starting an acquisition.
\param spc3 SPC3 handle
\param s Enable or disable the synchronization input
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Set_Sync_In_State(SPC3_H spc3, State s);

/**Turn on the Live mode.
The camera is set in the Live mode, i.e. it continuously acquires images (free-running mode). The frames which are not transferred to the computer are discarded.
Therefore, the time-laps between two frames is not constant and it will depend on the transfer speed between the host computer and the camera.
This mode is very useful to adjust optical components or to align the camera position. When the camera is in Live mode, no acquisition of images by SPC3_Get_Snap() 
or SPC3_Get_Memory() can be performed.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP The live mode has been already started
@see  SPC3_Set_Live_Mode_OFF()
@see  SPC3_Get_Memory()
@see  SPC3_Get_Snap()*/
DllSDKExport SPC3Return SPC3_Set_Live_Mode_ON(SPC3_H spc3);

/**Turn off the Live mode.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP The live mode is already inactive
@see  SPC3_Set_Live_Mode_ON()
*/
DllSDKExport SPC3Return SPC3_Set_Live_Mode_OFF(SPC3_H spc3);

/**Set FLIM parameters.
The camera can perform automatic time-gated FLIM measurements emplyoing the embedded gate generator. Call this function to setup the FLIM acquisition parameters. Each "FLIM acquisition" is composed by FLIM_steps frames,
each one consisting of an acquisition with Exposure and NIntegFrames as set with SPC3_Set_Camera_Par(). The total time required to perform each FLIM acquisition is passed back to the caller through the referenced FLIM_frame_time variable.
\param spc3 SPC3 handle
\param FLIM_steps Number of gate delay steps to be performed. Accepted values: 1 ... 800
\param FLIM_shift Delay shift between steps in thousenths of gate period (20ns). Accepted values: 1 ... 800
\param FLIM_start Start delay for FLIM sequence in thousenths of gate period (20ns). Accepted values: -400 ... +400
\param Length Duration of the ON gate signal. The unit is percentage.  Accepted values: 0 ... 100
\param FLIM_frame_time Total time required to perform each FLIM acquisition in multiples of 10ns. Value is referenced.
\param FLIM_bin_width Calibrated bin-width in ps for the specific camera. Nominal value is 20ps, i.e. 1/1000 of gate period. Value is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return OUT_OF_BOUND Parameters are out of bound. Please note that the function not only checks if the single parameters are acceptable, but also checks if the combination of parameters would result in an invalid gate
setting. E.g. FLIM_steps=100, FLIM_start=0, FLIM_shift=15 are not allowed, since they would result in a final gate shift of +1500 thousenths of period
@see SPC3_Set_Camera_Par()
*/
DllSDKExport SPC3Return SPC3_Set_FLIM_Par(SPC3_H spc3, UInt16 FLIM_steps, UInt16 FLIM_shift, Int16 FLIM_start, UInt16 Length, int* FLIM_frame_time,double* FLIM_bin_width);

/**Enable or disable FLIM mode. FLIM mode automatically set the number of used counters to 1. FLIM mode cannot be enabled if Exposure time is set to a value lower than 1040.
\param spc3 SPC3 handle
\param FLIM_State Enable or disable the FLIM mode
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location.
\return INVALID_OP Exposure time is lower than 1040.
@see SPC3_Set_Camera_Par()
@see SPC3_Set_FLIM_Par()
*/
DllSDKExport SPC3Return SPC3_Set_FLIM_State(SPC3_H spc3, State FLIM_State);

/**Apply settings to the camera.
This function must be called after any Set function, except SPC3_Set_Live_Mode_ON() and SPC3_Set_Live_Mode_OFF(), in order to apply the settings to the camera.
If several Set functions need to be called, there is no need to call this function after each Set function. A single call to this function at the end is enough to apply all the settings.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Apply_settings(SPC3_H spc3);

/*@}*/
//------------ Get methods -----------------------------------------
/**
 * \defgroup Get Get methods
 Functions to get status or data from SPC3 camera
 */
/*@{*/
/**Get a Live image.
Acquire a live image and store the data into the Img array. This command is working only when the Live mode is turned on by the SPC3_Set_Live_Mode_ON() function.
\param spc3 SPC3 handle
\param Img Pointer to the output image array. The size of the array must be at least 2 KB.
\return OK
\return NULL_POINTER The provided SPC3_H or Img point to an empty memory location
\return INVALID_OP The live-mode has not been started yet
@see  SPC3_Set_Live_Mode_ON()
*/
DllSDKExport SPC3Return SPC3_Get_Live_Img(SPC3_H spc3, UInt16* Img);

/**Prepare the camera to the acquistion of a snap.
This command configures the camera to acquire a snap of NFrames images, as set by the SPC3_Set_Camera_Par() function. In FLIM mode NFrames "FLIM acquisitions" of a FLIM sequence will be acquired.
If an External Sync is required, the camera will wait for a pulse on the Sync input before acquiring the images and saving them to the
internal memory, otherwise they are acquired and saved immediately. Once acquired, snap must then be transferred to the PC using the SPC3_Get_Snap() function.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP Unable to acquire images when the live mode is ON. Use instead SPC3_Get_Live_Img().
\return INVALID_OP When the background subtraction, dead-time correction or normal acquisition mode are enabled,
						a maximum of 65536 images can be acquired
@see SPC3_Set_Camera_Par()
@see SPC3_Get_Snap()
@see SPC3_Set_Sync_In_State()
*/
DllSDKExport SPC3Return SPC3_Prepare_Snap(SPC3_H spc3);

/**Get a selected number of images.
Acquire a set of images according to the parameters defined by SPC3_Set_Camera_Par(). In FLIM mode NFrames "FLIM acquisitions" will be acquired.
This command works only when SPC3_Prepare_Snap() has already been called.
This function will not exit until the required number of images has been downloaded. For this reason,
if the camera is configured for waiting and External Sync, before calling this function it could be useful to pool the camera for the trigger state,
using the SPC3_IsTriggered() function.
\param spc3 PC2 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP Unable to acquire images when the live mode is ON. Use instead SPC3_Get_Live_Img().
\return INVALID_OP When the background subtraction, dead-time correction or normal acquisition mode are enabled,
						a maximum of 65536 images can be acquired
@see SPC3_Set_Camera_Par()
@see SPC3_Prepare_Snap()
@see SPC3_Set_Sync_In_State()
@see SPC3_IsTriggered()
*/
DllSDKExport SPC3Return SPC3_Get_Snap(SPC3_H spc3);

/**Get the pointer to the image buffer in which snap acquisition is stored. The first byte indicates if data is 8 or 16 bit. \b WARNING User must pay attention not to exceed the dimension of the buffer (2*1024*65534 + 1 bytes) when accessing it.
\param spc3 SPC3 handle
\param buffer Pointer to the buffer Handle in which the function will save reference to the camera image buffer
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
@see SPC3_Get_Snap()
*/
DllSDKExport SPC3Return SPC3_Get_Image_Buffer(SPC3_H spc3, BUFFER_H* buffer);

/**Export an acquired image to an user allocated memory array.
Once a set of images have been acquired by SPC3_Get_Snap(), a single image can be exported from the SDK image buffer and saved
in the memory (Img array).
\param spc3 SPC3 handle
\param Img Pointer to the output image array. The size of the array must be at least 2 KB.
\param Position Index of the image to save.  Accepted values: 1 ... Number of acquired images
\param counter Number of the desired counter. Accepted values: 1 ... Number of used counters
\return OK
\return NULL_POINTER The provided SPC3_H or Img point to an empty memory location
\return OUT_OF_BOUND Parameters are out of bound.
@see SPC3_Get_Snap()
*/
DllSDKExport SPC3Return SPC3_Get_Img_Position(SPC3_H spc3, UInt16* Img, UInt32 Position, UInt16 counter);


/**Put the camera in "continuos acquisition" mode. Compatible with FLIM mode. If the camera was set to wait for an external sync, the acquistion will start as soon as a pulse is detected on the Sync input, otherwise it
will start immediately.  The output file name must be provided when calling this fuction. Data are stored in the camera internal memory and must be downloaded calling the SPC3_Get_Memory() function
as soon as possible, in order to avoid data loss.
\param spc3 SPC3 handle
\param filename Name of output file.
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
\return UNABLE_CREATE_FILE It was not possible to create the output file.
@see SPC3_Get_Memory()
@see SPC3_SPC3_Stop_ContAcq()
*/
DllSDKExport SPC3Return SPC3_Start_ContAcq(SPC3_H spc3, char filename[256]);

/**Dump the camera memory to the PC and save data to the file specified with the SPC3_Start_ContAcq() function in SPC3 file format (for details on the format see function SPC3_Save_Img_Disk).
This function must be repeatedly called, as fast as possible, in order to free the camera
internal memory and keep the acquisition going. If the internal camera memory get full during acquisition an error is generated. \b WARNING The camera can generate data with very high throughput,
up to about 205MB/s. Be sure to have enough disk space for your measurement.
\param spc3 SPC3 handle
\param total_bytes Total number of bytes read. Value is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
\return UNABLE_CREATE_FILE It was not possible to access the output file.
\return INVALID_OP Continuos acquistion was not yet started. Use SPC3_SPC3_Start_ContAcq() before calling this function.
\return COMMUNICATION_ERROR Communication error during data download.
\return SPC3_MEMORY_FULL Camera internal memory got full during data download. Datta loss occurred. Reduce frame-rate or optimize your software to reduce deadtime between subsequent calling of the function.
@see SPC3_SPC3_Start_ContAcq()
@see SPC3_SPC3_Stop_ContAcq()
@see SPC3_Save_Img_Disk();
*/
DllSDKExport SPC3Return SPC3_Get_Memory(SPC3_H spc3, double* total_bytes);

/**Stop the continuos acquisition of data and close the output file. This function must be called at the end of the continuos acquisition, in order to properly close the file. \b WARNING If not called,
the output file may be unreadable, and camera may have unexepected behaeviour if other functions are called.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
\return UNABLE_CREATE_FILE It was not possible to access the output file.
@see SPC3_SPC3_Start_ContAcq()
@see SPC3_Get_Memory()
*/
DllSDKExport SPC3Return SPC3_Stop_ContAcq(SPC3_H spc3);

/**Put the camera in "continuos acquisition" mode. Compatible with FLIM mode. If the camera was set to wait for an external sync, the acquistion will start as soon as a pulse is detected on the Sync input, otherwise it
will start immediately. Data are stored in the camera internal memory and must be downloaded calling the SPC3_Get_Memory_Buffer() function as soon as possible, in order to avoid data loss.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
@see SPC3_Get_Memory_Buffer()
@see SPC3_SPC3_Stop_ContAcq_in_Memory()
*/
DllSDKExport SPC3Return SPC3_Start_ContAcq_in_Memory(SPC3_H spc3);

/**Dump the camera memory to the PC and pass the pointer to the image buffer in which acquisition is stored. This function must be repeatedly called, as fast as possible, in order to free the camera internal memory and keep the acquisition going.
If the internal camera memory get full during acquisition an error is generated. \b WARNING User must pay attention not to exceed the dimension of the buffer when accessing it. Refer to the value saved into the parameter total_bytes.
\param spc3 SPC3 handle
\param total_bytes Total number of bytes read. Value is referenced
\param buffer Pointer to the buffer Handle in which the function will save reference to the camera buffer. Value is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H or BUFFER_H point to an empty memory location
\return INVALID_OP Continuos acquistion was not yet started. Use SPC3_SPC3_Start_ContAcq() before calling this function.
\return COMMUNICATION_ERROR Communication error during data download.
\return SPC3_MEMORY_FULL Camera internal memory got full during data download. Datta loss occurred. Reduce frame-rate or optimize your software to reduce deadtime between subsequent calling of the function.
@see SPC3_SPC3_Start_ContAcq_in_Memory()
@see SPC3_SPC3_Stop_ContAcq_in_Memory()
*/
DllSDKExport SPC3Return SPC3_Get_Memory_Buffer(SPC3_H spc3, double* total_bytes, BUFFER_H* buffer);

/**Stop the continuos acquisition of data. This function must be called at the end of the continuos acquisition. \b WARNING If not called, camera may have unexepected behaeviour if other functions are called.
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H point to an empty memory location
@see SPC3_SPC3_Start_ContAcq_in_Memory()
@see SPC3_Get_Memory_Buffer()
*/
DllSDKExport SPC3Return SPC3_Stop_ContAcq_in_Memory(SPC3_H spc3);

/**Get the calibrated dead-time value.
This function provides the closest calibrated dead-time value to Val.
\param spc3 SPC3 handle
\param Val Desired dead-time value in ns. No error is generated when the value is above MAX_DEAD_TIME.
\param ReturnVal Closest dead-time value possible. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H or ReturnVal point to an empty memory location
@see SPC3_Set_DeadTime()
*/
DllSDKExport SPC3Return SPC3_Get_DeadTime(SPC3_H spc3, UInt16 Val, UInt16* ReturnVal);

/**Get the calibrated gate width value.
This function provides the closest calibrated gate-width value to Val.
\param spc3 SPC3 handle
\param Val Desired gate-width value in percentage of 20ns. No error is generated when the value out of range, instead the real boundaries are forced on ReturnVal.
\param ReturnVal Closest gate-width value possible. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H or ReturnVal point to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Get_GateWidth(SPC3_H spc3, Int16 Val, double* ReturnVal);

/**Get the calibrated gate shift value.
This function provides the closest calibrated gate shift value to Val.
\param spc3 SPC3 handle
\param Val Desired gate shift value in thousenths of 20ns. No error is generated when the value out of range, instead the real boundaries are forced on ReturnVal.
\param ReturnVal Closest gate-shift value possible. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H or ReturnVal point to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Get_GateShift(SPC3_H spc3, Int16 Val, Int16* ReturnVal);

/**Get the actual bitdepth of acquired data.
Data from the camera will be 16bit per pixel, if NFramesInteg > 1, or DTC is enabled, or background subtraction is enabled, or 8bit per pixel otherwise.
This function provides actual bitdepth with the current settings.
\param spc3 SPC3 handle
\param is16bit Actual status. The value is 0 if bitdepth is 8bit and 1 if bitdepth is 16bit. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_Is16Bit(SPC3_H spc3, short* is16bit);

/**Pool the camera for external trigger staus.
Pool the camera in order to know if an external sync pulse was detected. The result is meaningfull only if the camera was previously set to wait for an external sync.
\param spc3 SPC3 handle
\param isTriggered Actual status. The value is 0 if no sync pulse was detected so far, 1 otherwise. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_IsTriggered (SPC3_H spc3, short* isTriggered);

/**Get the SDK and camera firmware version
\param spc3 SPC3 handle
\param Firmware_Version Version of the camera firmare in the format x.xx. This parameter is referenced.
\param Software_Version Version of the SDK in the format x.xx. This parameter is referenced.
\param Custom_version Customization version of the firmware and SDK. For standard model " " is returned. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_GetVersion(SPC3_H spc3, double* Firmware_Version, double* Software_Version, char* Custom_version);

/**Get the camera serial number and ID
\param spc3 SPC3 handle
\param Camera_ID Unique camera ID. A string of at least 11 character is required as parameter. This parameter is referenced.
\param Camera_serial SPC3 camera serial number.  A string of at least 33 character is required as parameter. This parameter is referenced.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
*/
DllSDKExport SPC3Return SPC3_GetSerial(SPC3_H spc3, char* Camera_ID, char* Camera_serial);

/**Get device info.
It gets device serial number, Unique ID, version and SDK version, without constructing an SPC3 object. Useful when constructor fails due to incompatible firmware and SDK versions or for powering issues.
\param Device_ID SPC3 camera Unique ID. A string of at least 11 character is required as parameter. If multiple devices are connected to the computer, a unique camera ID should be provided to correctly identify the camera. The camera ID can be found in the camera documentation.
An empty string is accepted too. In this case, the first available device will be connected and the function with write in this variable the camera ID. This parameter is referenced.
A string of at least 11 character is required as parameter. This parameter is referenced.
\param Camera_serial SPC3 camera serial number.  A string of at least 33 character is required as parameter. This parameter is referenced.
\param Firmware_Version Version of the camera firmare in the format x.xx. This parameter is referenced.
\param Software_Version Version of the SDK in the format x.xx. This parameter is referenced.
\param Firmware_Custom_Version Customization version of the firmware. For standard model " " is returned. This parameter is referenced.
\param Software_Custom_Version Customization version of the software. For standard model " " is returned. This parameter is referenced.
\return OK
*/
DllSDKExport SPC3Return SPC3_DeviceInfo(char* Device_ID, char* Camera_serial, double* Firmware_Version, double* Software_Version, char* Firmware_Custom_Version, char* Software_Custom_Version);

/*@}*/
//------------ Utilities -------------------------------------------
/**
 * \defgroup AddM Additional methods
 Additional utility functions
 */
/*@{*/
/**Save the selected images on the hard disk.
This function saves the acquired images on the hard disk. The output file format can be either a multipage TIFF with embedded acquisition metadata according to the OME-TIFF format or the proprietary SPC3 format. For FLIM measurements, use the SPC3_Save_FLIM_Disk() function.
If TIFF format is selected, the desired images will be saved in a file for each enabled counter. If SPC3 format is selected a single SPC3 file will be created for all the counters.
OME-TIFF file could be opened with any image reader compatible with TIFF file, since metadata are saved into the Image Description tag in XML format.
In order to decode OME-TIFF metadata, it is possible to use free OME-TIFF reader, such as OMERO or the Bio-Formats plugin for ImageJ. For more details see the OME-TIFF web site:
http://www.openmicroscopy.org/site/support/ome-model/ome-tiff/.
SPC3 file are binary files composed by a header with acquisition metadata followed by raw image data, containg the 8/16 bit pixel values in row-major order. The byte order is little-endian for the 16 bit images.
In case more counters are used, data are interlaced, i.e. the sequence of frames is the following:
1st frame of 1st counter, 1st frame of 2nd counter, 1st frame of 3rd counter, 2nd frame of 1st counter, etc.
The header is composed by a signature of 8 byte (0x4d5044ff03000001, starting with 4d on byte 0), and a metadata section of 1024 byte, as follows (multibyte fields are little-endian):

| Byte offset   | Number of bytes   | Description									|
| --------------|-------------------|-----------------------------------------------|
| 0             | 10	   		    | Unique camera ID (string)						|
| 10			| 32				| SPC3 serial number (string)					|
| 42			| 2					| Firmware version (1.11 saved as 111)			|
| 44			| 1					| Firmware custom version (standard = 0)		|
| 45			| 20				| Acquisition data&time (string)				|
| 65			| 35				| Unused										|
| 100           | 1                 | Number of rows								|
| 101           | 1                 | Number of colums								|
| 102           | 1                 | Bit per pixel									|
| 103           | 1                 | Counters in use								|
| 104           | 2                 | Hardware integration time	(multiples of 10ns)	|
| 106           | 2                 | Summed frames					 				|
| 108           | 1                 | Dead time correction enabled					|
| 109           | 1                 | Internal gate duty-cycle (0-100%)				|
| 110           | 2                 | Holdoff time (ns)								|
| 112		    | 1                 | Background subtraction enabled				|
| 113           | 1                 | Data for counters 1 and 2 are signed			|
| 114			| 4					| Number of frames in the file					|
| 118			| 1					| Image is averaged								|
| 119			| 1					| Counter which is averaged						|
| 120			| 2					| Number of averaged images						|
| 122			| 78				| Unused										|
| 200           | 1                 | FLIM enabled									|
| 201           | 2                 | FLIM shift %									|
| 203           | 2                 | FLIM steps									|
| 205           | 4                 | FLIM frame length (multiples of 10ns)			|
| 209           | 2                 | FLIM bin width (fs)                           |
| 211			| 89				| Unused										|
| 300           | 1                 | PDE measurement								|
| 301           | 2                 | Start wavelength (nm)							|
| 303           | 2                 | Stop wavelength (nm)							|
| 305           | 2                 | Step (nm)										|
| 307           | 717               | unused										|

SPC3 file can be read using the provided ImageJ/Fiji plugin v1.11. File generated with SDK v1.01 has a different header format and file signature,
which is incompatible with plugin v1.11. A command line tool to convert SPC3 file from 1.01 format to 1.11 format is provided.

\param spc3 SPC3 handle
\param Start_Img Index of the first image to save.  Accepted values: 1 ... Number of acquired images
\param End_Img Index of the last image to save.  Accepted values: Start_Img ... Number of acquired images
\param filename Name of the output file. Value is referenced.
\param mode File format of the output images
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired or the selected range of images is not valid 
\return UNABLE_CREATE_FILE Unable to create the output file
*/
DllSDKExport SPC3Return SPC3_Save_Img_Disk(SPC3_H spc3, UInt32 Start_Img, UInt32 End_Img, char* filename, OutFileFormat mode);

/**Save the selected images on the hard disk.
This function saves the average of the images acquired by a specified counter on the hard disk. File format can be proprietary SPC3 or TIFF, as explained in SPC3_Save_Img_Disk() function.
\param spc3 SPC3 handle
\param counter Number of the counter to be saved
\param filename Name of the output file. Value is referenced.
\param mode File format of the output images
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired or the selected range of images is not valid 
\return UNABLE_CREATE_FILE Unable to create the output file
*/
DllSDKExport SPC3Return SPC3_Save_Averaged_Img_Disk(SPC3_H spc3, int counter, char* filename, OutFileFormat mode);

/**Save the FLIM acquisition on the hard disk.
This function saves the acquired FLIM images on the hard disk. The output file format can be either a multipage TIFF with embedded acquisition metadata according to the OME-TIFF format or the proprietary SPC3 format. For standard measurements, use the SPC3_Save_Img__Disk() function.
For both formats, image data is composed by a set of images following a "FLIM first, time second scheme", i.e. with the following frame sequence:
1st gate shift of 1st FLIM measurement, 2nd gate shift of 1st FLIM measurement,...,nth gate shift of 1st FLIM measurement,1st gate shift of 2nd FLIM measurement, 2nd gate shift of 2nd FLIM measurement,...,nth gate shift of 2nd FLIM measurement,etc.
OME-TIFF file could be opened with any image reader compatible with TIFF file, since metadata are saved into the Image Description tag in XML format.
In order to decode OME-TIFF metadata, it is possible to use free OME-TIFF reader, such as OMERO or the Bio-Formats plugin for ImageJ. For more details see the OME-TIFF web site:
http://www.openmicroscopy.org/site/support/ome-model/ome-tiff/. OME-TIFF metadata include the ModuloAlongT tag, which allows the processing of FLIM data with dedicated FLIM software such as FLIMfit (see http://www.openmicroscopy.org/site/products/partner/flimfit).
SPC3 file are binary files composed by a header with acquisition metadata followed by raw image data, containg the 8/16 bit pixel values in row-major order. The byte order is little-endian for the 16 bit images. 
The header is composed by a signature of 8 byte (0x4d5044ff03000001, starting with 4d on byte 0), and a metadata section of 1024 byte, as follows (multibyte fields are little-endian):

| Byte offset   | Number of bytes   | Description									|
| --------------|-------------------|-----------------------------------------------|
| 0             | 10	   		    | Unique camera ID (string)						|
| 10			| 32				| SPC3 serial number (string)					|
| 42			| 2					| Firmware version (1.11 saved as 111)			|
| 44			| 1					| Firmware custom version (standard = 0)		|
| 45			| 20				| Acquisition data&time (string)				|
| 65			| 35				| Unused										|
| 100           | 1                 | Number of rows								|
| 101           | 1                 | Number of colums								|
| 102           | 1                 | Bit per pixel									|
| 103           | 1                 | Counters in use								|
| 104           | 2                 | Hardware integration time	(multiples of 10ns)	|
| 106           | 2                 | Summed frames					 				|
| 108           | 1                 | Dead time correction enabled					|
| 109           | 1                 | Internal gate duty-cycle (0-100%)				|
| 110           | 2                 | Holdoff time (ns)								|
| 112		    | 1                 | Background subtraction enabled				|
| 113           | 1                 | Data for counters 1 and 2 are signed			|
| 114			| 4					| Number of frames in the file					|
| 118			| 1					| Image is averaged								|
| 119			| 1					| Counter which is averaged						|
| 120			| 2					| Number of averaged images						|
| 122			| 78				| Unused										|
| 200           | 1                 | FLIM enabled									|
| 201           | 2                 | FLIM shift %									|
| 203           | 2                 | FLIM steps									|
| 205           | 4                 | FLIM frame length (multiples of 10ns)			|
| 209           | 2                 | FLIM bin width (fs)                           |
| 211			| 89				| Unused										|
| 300           | 1                 | PDE measurement								|
| 301           | 2                 | Start wavelength (nm)							|
| 303           | 2                 | Stop wavelength (nm)							|
| 305           | 2                 | Step (nm)										|
| 307           | 717               | unused										|

SPC3 file can be read using the provided ImageJ/Fiji plugin v1.11. File generated with SDK v1.01 has a different header format and file signature,
which is incompatible with plugin v1.11. A command line tool to convert SPC3 file from 1.01 format to 1.11 format is provided.

\param spc3 SPC3 handle
\param filename Name of the output file. Value is referenced.
\param mode File format of the output images
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired or the selected range of images is not valid 
\return UNABLE_CREATE_FILE Unable to create the output file
*/
DllSDKExport SPC3Return SPC3_Save_FLIM_Disk(SPC3_H spc3, char* filename, OutFileFormat mode);


/**Read a spc3 image from file.
Read the image at the ImgIdx position and for desired counter in the given spc3 file from the hard disk.
\param filename Name of the output file
\param ImgIdx    Image index in the file. Accepted values: 1 ... 65534
\param counter Desired counter. Accepted values: 1 ... 3
\param Img Pointer to the output image array. The size of the array must be at least 2 kB.
\param header Array in which the header of SPC3 file is saved.
\return OK
\return UNABLE_READ_FILE Unable to read the input file. Is it a SPC3 file?
\return OUT_OF_BOUND The desired counter or image exceeds the file size.
\return NOT_EN_MEMORY Not enough memory to store the data contained in the file
*/
DllSDKExport SPC3Return SPC3_ReadSPC3FileFormatImage(char* filename, UInt32 ImgIdx, UInt16 counter, UInt16* Img, char header[1024]);

/**Calculate the average image.
Once a set of images have been acquired by SPC3_Get_Snap(), an image which contains for each pixel the average value over all the acquired images is
calculated. This is stored in the Img array.
\param spc3 SPC3 handle.
\param Img Pointer to the output double image array. The size of the array must be at least 8 kB.
\param counter Desired counter.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired
*/
DllSDKExport SPC3Return SPC3_Average_Img(SPC3_H spc3, double* Img, int counter);

/**Calculate the standard deviation image.
Once a set of images have been acquired by SPC3_Get_Snap(), an image which contains for each pixel the standard deviation over all the acquired images is
calculated. This is stored in the Img array.
\param spc3 Pointer to the SPC3 handle
\param Img Pointer to the output double image array. The size of the array must be at least 8 KB.
\param counter Desired counter.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired
*/
DllSDKExport SPC3Return SPC3_StDev_Img(SPC3_H spc3, double* Img, int counter);

/** Enable the correlation mode.
This function must be called before invoking SPC3_Correlation_Img(). When this function is called, the memory required to save the
new data is allocated in the heap and the previously stored data are cancelled. The deallocation of this memory is automatically performed
when the SPC3_destr() function is called or by setting the State s equal to Disabled.
\param spc3 SPC3 handle
\param CM Selected autocorrelation algorithm
\param NCorrChannels Number of global lag channels. When the linear correlation algorithm is selected, the first NChannel lags are calculated, where NChannel
must be greater than 2. This algorithm accepts only a number of images which is a power of 2. For example, if 1025 images were acquired, only 1024 images are 
used to calculate the autocorrelation function. In case of Multi-tau algorithm, it defines the number of channel groups. The first group has 16 lags of duration equal to the exposure time of a frame.
The following groups have 8 lags each, spaced at 2^i * Exposure time.
\param s Enable or Disable the correlation mode
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return OUT_OF_BOUND NCorrChannels must be greater than zero for the Multi-tau algorithm and greater than 2 for the Linear one
\return NOT_EN_MEMORY There is not enough memory to enable the correlation mode
@see SPC3_Correlation_Img()*/
DllSDKExport SPC3Return SPC3_Set_Correlation_Mode(SPC3_H spc3, CorrelationMode CM, int NCorrChannels, State s);

/** Calculate the autocorrelation function.
The autocorrelation function is estimated for each pixel. This function requires that a set of
images have been previously acquired by SPC3_Get_Snap() and that the correlation mode is set to Enabled. Depending on the selected algorithm and the
total number of collected images, this function can take several tens of seconds.
\param spc3 SPC3 handle
\param counter Desired counter.
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP No images were acquired or the correlation mode was not enabled
\return NOT_EN_MEMORY Not enough memory to calculate the correlation function
\return INVALID_NIMG_CORRELATION The required number of time lags of the correlation function can not be calculated from the available number of images
@see SPC3_Set_Correlation_Mode()
@see SPC3_Get_Snap()
*/
DllSDKExport SPC3Return SPC3_Correlation_Img(SPC3_H spc3,int counter);

/** Save the autocorrelation functions on the hard disk.
This function requires that SPC3_Set_Correlation_Mode() has been previously called. The autocorrelation data are stored in a .spcc binary
file.
The spcc binary file is organized as follows:

Byte offset               | Type           |Number of bytes | Description											|
--------------------------|----------------|----------------|-------------------------------------------------------|
0                         | int            |4               | Number of lag-times (NLag)							|
4                         | int            |4               | Number of pixels. This value must be 1024 (NPix)		|
8                         | int            |4               | Selected algorithm: 0 Linear, 1 Multi-tau				|
12                        | double         |8 * NLag        | Autocorrelation values of the first pixel				|
12 + 8 * NLag             | double         |8 * NLag        | Autocorrelation values of the second pixel			|
...                       | double         |8 * NLag        | Autocorrelation values of the  N<SUP>th</SUP> pixel	|
12 + 8 * (NPix-1) * NLag  | double         |8 * NLag        | Autocorrelation values of the last pixel				|
12 + 8 * NPix * NLag      | double         |8 * NLag        | Lag times												|

A simple Matlab script can be used to read the data for further processing or visualization.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.mat}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
MPD .SPCC file reader
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function data = Read_SPCC(fname)
f = fopen(fname,'rb');
buf = fread(f,3,'int32');
data.NChannel = buf(1);
data.NPixel = buf(2);
data.IsMultiTau = (buf(3) == 1);

data.CorrelationImage = reshape(fread(f,data.NPixel*data.NChannel,'float64'), ...
            data.NChannel,32,32);
data.CorrelationImage = permute(data.CorrelationImage,[2 3 1]);
data.t=fread(f, data.NChannel,'float64');
fclose(f);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\param spc3 SPC3 handle
\param filename Name of the output file
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return INVALID_OP The autocorrelation, which has been calculated, is not valid
\return UNABLE_CREATE_FILE Unable to create the output file
@see SPC3_Set_Correlation_Mode()
*/
DllSDKExport SPC3Return SPC3_Save_Correlation_Img(SPC3_H spc3, char* filename);

/*@}*/
//------------ MPD Internal Use -------------------------------------------
/**
 * \defgroup CalP MPD only - Calibration functions
 Functions for internal MPD use
 */
/*@{*/
/**Calibrate the dead-time time. Only for MPD use
Calibrate the dead time of the device. The function outputs a calibration file "Out.dat".
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return UNABLE_CREATE_FILE Unable to create the output file
*/
DllSDKExport SPC3Return SPC3_Calibrate_DeadTime(SPC3_H spc3);

/**Calibrate the gate-width. Only for MPD use
Calibrate the gate-width of the device. The function outputs a calibration file "GateCalib.dat".
\param spc3 SPC3 handle
\return OK
\return NULL_POINTER The provided SPC3_H points to an empty memory location
\return UNABLE_CREATE_FILE Unable to create the output file
*/
DllSDKExport SPC3Return SPC3_Calibrate_Gate(SPC3_H spc3);


/*@}*/

/**@example SDK_Example.c*/
#ifdef __cplusplus
}
#endif

#endif //__SPC3_SDK_h__
