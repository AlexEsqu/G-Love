#ifndef TWI_H
#define TWI_H

#include<stdint.h>


/* ********************************************************************************* */
/*										REGISTERS									 */
/* ********************************************************************************* */

#define TW_BIT_RATE_REG TWBR				// Two wire bit rate register
#define TW_STATUS_REG TWSR					// Two wire status register
#define TW_CONTROL_REG TWCR					// Two wire control register
#define TW_DATA_REG TWDR					// Two wire data register


/* ********************************************************************************* */
/*										FLAGS										 */
/* ********************************************************************************* */

#define TW_INTERRUPT_BIT (1 << TWINT)   	// Interrupt flag
#define TW_START_BIT (1 << TWSTA)       	// Start flag
#define TW_STOP_BIT (1 << TWSTO)        	// Stop flag
#define TW_ENABLE_BIT (1 << TWEN)       	// Enable flag
#define TW_ACKNOWLEDGE_BIT (1 << TWEA)		// Enable acknowledge flag


/* ********************************************************************************* */
/*										STATUS										 */
/* ********************************************************************************* */

#define TW_STATUS (TW_STATUS_REG & 0xF8)	// Status register + status mask

#define START         0x08					// Start status
#define REP_START     0x10					// Repeated start

/*---------------------- Master Transmitter -------------------*/

#define MT_SLA_ACK       0x18				// Slave + write transmitted & acknowledge received
#define MT_SLA_NACK      0x20				// Slave + write transmitted & not acknowledge received
#define MT_DATA_ACK      0x28				// Data byte transmitted & acknowledge received
#define MT_DATA_NACK     0x30				// Data byte transmitted & not acknowledge received

/*---------------------- Master Receiver -------------------*/

#define MR_SLA_ACK       0x40				// Slave + read transmitted & acknowledge received
#define MR_SLA_NACK      0x48				// Slave + read transmitted & not acknowledge received
#define MR_DATA_ACK      0x50				// Data byte received & acknowledge returned
#define MR_DATA_NACK     0x58				// DAta byte received & not acknowledge returned


/* ********************************************************************************* */
/*										FUNCTIONS									 */
/* ********************************************************************************* */

void 	tw_init(void);
void 	tw_start(void);
void 	tw_stop(void);
void 	tw_write(uint8_t data);
uint8_t tw_read(uint8_t ack_bit);

#endif