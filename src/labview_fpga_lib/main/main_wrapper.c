/*
 * FPGA Interface C API example for GCC for computers running Linux.
 *
 * NOTE: In order to run this example, you must compile a LabVIEW FPGA bitfile
 *       and generate a C API for it. For more information about using this
 *       example, refer to the Examples topic of the FPGA Interface C API Help,
 *       located under
 *       Start>>All Programs>>National Instruments>>FPGA Interface C API.
 */
 
#include "NiFpga_main_FPGA.h"

#include <stdio.h>
#include <stdlib.h>

// overwrite the definition of the header file created by Labview because FPGA bitcode will be located in the subdir lib
// #define "NiFpga_main_FPGA.lvbitx" 

/*
IMPORTANT need to replace

#define NiFpga_main_FPGA_Bitfile "NiFpga_main_FPGA.lvbitx" 

with

#define NiFpga_main_FPGA_Bitfile "C:\\Users\\Experiment\\PycharmProjects\\b26_toolkit\\src\\labview_fpga_lib\\main\\NiFpga_main_FPGA.lvbitx" 

in NiFpga_main_FPGA.h (which is generated by labview C API)

*/

void start_fpga(NiFpga_Session* session, NiFpga_Status* status)
{
	// must be called before any other calls 
	*status = NiFpga_Initialize();
	printf("initializing FPGA main \n");
	printf("bitfile expected at:\n");
	printf(NiFpga_main_FPGA_Bitfile);
	printf("\n");
	
	if (NiFpga_IsNotError(*status))
	{
		// opens a session, downloads the bitstream, and runs the FPGA 
		NiFpga_MergeStatus(status, NiFpga_Open(NiFpga_main_FPGA_Bitfile,
												NiFpga_main_FPGA_Signature,
												"RIO0",
												NiFpga_OpenAttribute_NoRun,
												session));
												
		// reset
		NiFpga_MergeStatus(status, NiFpga_Reset(*session));
		
		if (NiFpga_IsNotError(*status))
		{
			// run the FPGA application 
			NiFpga_MergeStatus(status, NiFpga_Run(*session, 0));
		}
		else{
			// print warning
			printf("error occurred at FPGA open");
			printf("%d", *status);
			
		}
	}
	
	fflush(stdout);
	
}

void stop_fpga(NiFpga_Session* session, NiFpga_Status* status)
{
	// close the session now that we're done 
	NiFpga_MergeStatus(status, NiFpga_Close(*session, 0));

	// must be called after all other calls 
	NiFpga_MergeStatus(status, NiFpga_Finalize());
}

void reset_fpga(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_Reset(*session));
}



// =====================================================================================
// main control parameters
 // =====================================================================================
void set_run_mode(uint16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU16(*session,NiFpga_main_FPGA_ControlU16_run_mode,value));
}
uint16_t read_run_mode(NiFpga_Session* session, NiFpga_Status* status)
{
	uint16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadU16(*session,NiFpga_main_FPGA_ControlU16_run_mode,&value));
	return value;
}
void set_stop_all(_Bool state, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteBool(*session,NiFpga_main_FPGA_ControlBool_StopAll, state));
}
_Bool read_stop_all(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;
	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_ControlBool_StopAll,&state));
	return state;
}
_Bool read_executing_subvi(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;
	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_IndicatorBool_executingsubscript,&state));
	return state;
}
void set_count_ms(uint32_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU32(*session,NiFpga_main_FPGA_ControlU32_CountmSec,value));
}
int32_t read_elements_written_to_dma(NiFpga_Session* session, NiFpga_Status* status)
{
	int32_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI32(*session,NiFpga_main_FPGA_IndicatorI32_datasenttoDMA,&value));
	return value;
}

// =====================================================================================
// gavlo scan parameters
// =====================================================================================
void set_Nx(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_N_x,value));
}
int16_t read_Nx(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_N_x,&value));
	return value;
}

void set_Vmin_x(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Vmin_x,value));
}

void set_dVmin_x(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_dVmin_x,value));
}

void set_Ny(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_N_y,value));
}
int16_t read_Ny(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_N_y,&value));
	return value;
}
void set_Vmin_y(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Vmin_y,value));
}

void set_dVmin_y(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_dVmin_y,value));
}

void set_scanmode_x(uint8_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU8(*session,NiFpga_main_FPGA_ControlU8_scanmodex,value));
}

void set_scanmode_y(uint8_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU8(*session,NiFpga_main_FPGA_ControlU8_scanmodey,value));
}

void set_detector_mode(uint8_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU8(*session,NiFpga_main_FPGA_ControlU8_detector_mode,value));
}

void set_settle_time(uint16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU16(*session,NiFpga_main_FPGA_ControlU16_settle_time_us,value));
}
uint16_t read_settle_time(NiFpga_Session* session, NiFpga_Status* status)
{
	uint16_t value;
	NiFpga_MergeStatus(status, NiFpga_ReadU16(*session,NiFpga_main_FPGA_ControlU16_settle_time_us,&value));
	return value;
}

void set_meas_per_pt(uint16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteU16(*session,NiFpga_main_FPGA_ControlU16_measurements_per_pt,value));
}
uint16_t read_meas_per_pt(NiFpga_Session* session, NiFpga_Status* status)
{
	uint16_t value;
	NiFpga_MergeStatus(status, NiFpga_ReadU16(*session,NiFpga_main_FPGA_ControlU16_measurements_per_pt,&value));
	return value;
}


// =====================================================================================
// read inputs and outputs
// =====================================================================================


int16_t read_AI0(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI0,&value));
	return value;
}

int16_t read_AI1(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI1,&value));
	return value;
}

int16_t read_AI2(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI2,&value));
	return value;
}

int16_t read_AI3(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI3,&value));
	return value;
}

int16_t read_AI4(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI4,&value));
	return value;
}

int16_t read_AI5(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI5,&value));
	return value;
}

int16_t read_AI6(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI6,&value));
	return value;
}

int16_t read_AI7(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_IndicatorI16_Connector1AI7,&value));
	return value;
}

int16_t read_AO0(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO0,&value));
	return value;
}

int16_t read_AO1(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO1,&value));
	return value;
}

int16_t read_AO2(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO2,&value));
	return value;
}

int16_t read_AO3(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO3,&value));
	return value;
}

int16_t read_AO4(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO4,&value));
	return value;
}

int16_t read_AO5(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO5,&value));
	return value;
}

int16_t read_AO6(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO6,&value));
	return value;
}

int16_t read_AO7(NiFpga_Session* session, NiFpga_Status* status)
{
	int16_t value;

	NiFpga_MergeStatus(status, NiFpga_ReadI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO7,&value));
	return value;
}


void set_AO0(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO0,value));
}


void set_AO1(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO1,value));
}

void set_AO2(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO2,value));
}

void set_AO3(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO3,value));
}

void set_AO4(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO4,value));
}

void set_AO5(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO5,value));
}

void set_AO6(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO6,value));
}

void set_AO7(int16_t value, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteI16(*session,NiFpga_main_FPGA_ControlI16_Connector1AO7,value));
}


_Bool read_DIO0(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;

	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_IndicatorBool_Connector1DIO0,&state));
	return state;
}

_Bool read_DIO1(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;

	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_IndicatorBool_Connector1DIO1,&state));
	return state;
}

_Bool read_DIO2(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;

	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_IndicatorBool_Connector1DIO2,&state));
	return state;
}

_Bool read_DIO3(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_Bool state;

	NiFpga_MergeStatus(status, NiFpga_ReadBool(*session,NiFpga_main_FPGA_IndicatorBool_Connector1DIO3,&state));
	return state;
}


void set_DIO4(_Bool state, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteBool(*session,NiFpga_main_FPGA_ControlBool_Connector1DIO4,state));
}

void set_DIO5(_Bool state, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteBool(*session,NiFpga_main_FPGA_ControlBool_Connector1DIO5,state));
}

void set_DIO6(_Bool state, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteBool(*session,NiFpga_main_FPGA_ControlBool_Connector1DIO6,state));
}

void set_DIO7(_Bool state, NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_WriteBool(*session,NiFpga_main_FPGA_ControlBool_Connector1DIO7,state));
}





// =========================================================
// ============ FIFO =======================================
// =========================================================
size_t configure_FIFO(size_t requestedDepth, NiFpga_Session* session, NiFpga_Status* status)
{
	size_t actualDepth;
	NiFpga_MergeStatus(status, NiFpga_ConfigureFifo2(*session, NiFpga_main_FPGA_TargetToHostFifoI32_DMA, requestedDepth, &actualDepth));
	return actualDepth;
}

void start_FIFO(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_StartFifo(*session, NiFpga_main_FPGA_TargetToHostFifoI32_DMA));
}

void stop_FIFO(NiFpga_Session* session, NiFpga_Status* status)
{
	NiFpga_MergeStatus(status, NiFpga_StopFifo(*session, NiFpga_main_FPGA_TargetToHostFifoI32_DMA));
}


void read_FIFO(int32_t* input, size_t size, NiFpga_Session* session, NiFpga_Status* status,size_t* elementsRemaining)
{
	/* copy FIFO data from the FPGA */
	NiFpga_MergeStatus(status,
					   NiFpga_ReadFifoI32(*session,
							   	   	   	  NiFpga_main_FPGA_TargetToHostFifoI32_DMA,
										  input,
										  size,
										  NiFpga_InfiniteTimeout,
										  elementsRemaining));
}


