#ifndef TWI_H
#define TWI_H

#include <stdint.h>
#include <avr/io.h>

/* ********************************************************************************* */
/*										STATUS										 */
/* ********************************************************************************* */

#define TW_STATUS (TWSR & 0xF8)    // Status register + status mask

enum twi_status_code {
	START = 0x08,      /* Start status */
	REP_START = 0x10,  /* Repeated start status */

	/* Master Transmitter */
	MT_SLA_ACK = 0x18,    /* Slave + write transmitted & acknowledge received */
	MT_SLA_NACK = 0x20,   /* Slave + write transmitted & not acknowledge received */
	MT_DATA_ACK = 0x28,   /* Data byte transmitted & acknowledge received */
	MT_DATA_NACK = 0x30,  /* Data byte transmitted & not acknowledge received */

	/* Master Receiver */
	MR_SLA_ACK = 0x40,    /* Slave + read transmitted & acknowledge received */
	MR_SLA_NACK = 0x48,   /* Slave + read transmitted & not acknowledge received */
	MR_DATA_ACK = 0x50,   /* Data byte received & acknowledge returned */
	MR_DATA_NACK = 0x58   /* Data byte received & not acknowledge returned */
};


/* ********************************************************************************* */
/*										FUNCTIONS									 */
/* ********************************************************************************* */

void 	    twi_init(void);
void 	    twi_start(void);
void 	    twi_stop(void);
void 	    twi_write(uint8_t data);
uint8_t     twi_read(uint8_t ack);
int	    twi_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val);
int        twi_write_burst(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t size);
uint8_t	    twi_read_reg(uint8_t dev_addr, uint8_t reg);
void	    twi_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif