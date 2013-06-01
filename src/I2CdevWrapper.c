/*
 * I2CdevWrapper.c
 *
 * Created: 1/9/2013 9:09:06 PM
 *  Author: ViDAR
 */ 

#include "I2CdevWrapper.h"
#include <asf.h>
//#include <math.h>
//#include "math.h"
#define TWI_SPEED             (400000ul)       //!< TWI data transfer rate
#define TWI_EXAMPLE             TWI0

/** Read single byte from an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    return I2Cdev_readBytes(devAddr, regAddr, 1, data);
}

/** Read multiple bytes from an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data BMA150_buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {

	twi_master_options_t opt = {
		.speed = TWI_SPEED,
		.chip  = devAddr
	};
	
	twi_master_setup(TWI0, &opt);

	twi_package_t packet_received = {
		.addr[0]      = regAddr,      // TWI slave memory address data MSB
		.addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
		.chip         = devAddr,      // TWI slave bus address
		.buffer       = data,        // transfer data destination buffer
		.length       = length   // transfer data size (bytes)
	};
	
	  while (twi_master_read(TWI0, &packet_received) != TWI_SUCCESS);
	
	return length;
}

/** Read a single bit from an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = I2Cdev_readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = I2Cdev_readByte(devAddr, regAddr, &b)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** write a single bit in an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    I2Cdev_readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2Cdev_writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (I2Cdev_readByte(devAddr, regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return I2Cdev_writeByte(devAddr, regAddr, b);
    } else {
        return false;
    }
}

/** Write single byte to an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return I2Cdev_writeBytes(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param BMA150_devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data BMA150_buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
	
	twi_master_options_t opt = {
		.speed = TWI_SPEED,
		.chip  = devAddr
	};
	
	twi_master_setup(TWI0, &opt);
	
	twi_package_t packet_received = {
		.addr[0]      = regAddr,      // TWI slave memory address data MSB
		.addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
		.chip         = devAddr,      // TWI slave bus address
		.buffer       = data,        // transfer data destination buffer
		.length       = length   // transfer data size (bytes)
	};
	
	while (twi_master_write(TWI0, &packet_received) != TWI_SUCCESS);
	
    return true;
}