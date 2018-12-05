#include "stdfix_emu.h"

#define BLOCK_SIZE 16
#define MAX_NUM_CHANNEL 8
#define CHANNEL_NUMBER 5
#define MINUS_TWO_DB FRACT_NUM(0.794328)
#define MINUS_SIX_DB FRACT_NUM(0.501187)

/* DSP type definitions */
typedef short DSPshort;					/* DSP integer */
typedef unsigned short DSPushort;		/* DSP unsigned integer */
typedef int DSPint;						/* native integer */
typedef fract DSPfract;					/* DSP fixed-point fractional */
typedef long_accum DSPaccum;			/* DSP fixed-point fractional */
/* Initialization constants */

