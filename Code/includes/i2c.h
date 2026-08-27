#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>

/* ********************************************************************************* */
/*										STATUS										 */
/* ********************************************************************************* */

#define I2C_STATUS (TWSR & 0xF8)    // Status register + status mask

enum i2c_status_code {
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

void 	    i2c_init(void);
void 	    i2c_start(void);
void 	    i2c_stop(void);
void 	    i2c_write(uint8_t data);
uint8_t     i2c_read(uint8_t ack);
int	    i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t val);
int        i2c_write_burst(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t size);
uint8_t	    i2c_read_reg(uint8_t dev_addr, uint8_t reg);
void	    i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif