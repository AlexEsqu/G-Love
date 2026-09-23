#ifndef MUX_H
 #define MUX_H

#include "../../includes/main.h"

#ifdef PROTO
	// define multiplexer selector pins for arduino uno
	#define S0 (2)
	#define S1 (3)
	#define S2 (4)
	#define S3 (5)
	#define EN (7)
	// define multiplxer reading pin
	#define SIG (14)
#else
	// define multiplexer pin on PCB
	#define PIN_SEL_A (2) // 25 (PC2)
	#define PIN_SEL_B (1) // 24 (PC0)
	#define PIN_SEL_C (0) // 23 (PC1)

	#define PIN_ADC_X (7) // 22 (ÀDC7)
	#define PIN_ADC_Y (6) // 19 (ADC6)
#endif

#define MAX_CHANNEL 8

// CD4097 multiplexer: 4select and 16channel
void	selectChannel(int channel);
int		readChannel(int channel);
void	readAllChannels(int max);

void	mux_setup();
void	mux_loop();

#endif