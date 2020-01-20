/*!
 * \file EE24xx.cpp
 * Functions for accessing the 24LCxx I2C EEPROMs from Microchip et al.
 *
 * \author Andreas Czechanowski, DL4SDC
 * \date 2012-11-25  Created
 * \date 2012-12-26  Changed from Wire to I2C library
 * \date 2019-10-06  Added 24C08
 */

#include <I2C.h>
#include "EE24xx.h"

/*!
 * set to 1 to have debug output sent to Serial
 */
#define EE24xx_DBG  0

/*!
 * the class constructor, setting the I2C bus address and the type of EEPROM.
 * Currently, models from 32kbit to 512kbit are supported.
 *
 * @param i2c_address  7-bit device address on bus, usually 0x50 to 0x57
 * @param ee_model     specifies EEPROM model, see enum ee24Type 
 */
EE24xx::EE24xx(uint8_t i2c_address = 0x50, ee24Type ee_model = EE24x512)
{
  // Initialization of I2C library, should possibly take place in setup()
  // I2c.begin();

  switch(ee_model)
  {
	case EE24x08:
	  ee_pagemask = 0x0f;  // 16 bytes per page
	  ee_maxaddress = 0x03ffL;
	  break;
	  
	case EE24x16:
	  ee_pagemask = 0x0f;  // 16 bytes per page
	  ee_maxaddress = 0x07ffL;
	  break;
	  
    case EE24x32:
      ee_pagemask = 0x1f;  // 32 bytes per page
      ee_maxaddress = 0x0fffL;
      break;

    case EE24x64:
      ee_pagemask = 0x1f;
      ee_maxaddress = 0x1fffL;
      break;

    case EE24x128:
      ee_pagemask = 0x3f;  // 64 bytes per page
      ee_maxaddress = 0x3fffL;
      break;

    case EE24x256:
      ee_pagemask = 0x3f;
      ee_maxaddress = 0x7fffL;
      break;

    case EE24x512:
      ee_pagemask = 0x7f;  // 128 bytes per page
      ee_maxaddress = 0xffffL;
      break;

    default:
      ee_pagemask = 0x01;
      ee_maxaddress = 0xffffL;
      break;
  }
  ee_address = i2c_address;
}


/*!
 * reads a number of bytes at given address from the I2C-EEPROM.
 * The I2C-bus address was set by argument to the constructor.
 *
 * @param addr  memory start address within EEPROM
 * @param buf   array of bytes to read into
 * @param cnt   number of bytes in buf to read
 */
void EE24xx::read(uint16_t addr, uint8_t *buf, uint16_t cnt)
{
#if 1 // I2C library

  // CAUTION: The I2C library knows a number of overloaded methods
  // for read() and write(). Be sure to take the correct ones.
  if (ee_maxaddress < 0x400)
  {
    I2c.write(ee_address | ((addr >> 8) & 0x7), (uint8_t)(addr & 0xff));	  
  } else {
    I2c.write(ee_address, (uint8_t)((addr >> 8) & 0xff), (uint8_t)(addr & 0xff));
  }
  I2c.read(ee_address, cnt, buf);
#if (EE24xx_DBG)
  Serial.print("EE24xx::read I2C=0x");
  Serial.print(ee_address, 16);
  Serial.print(", A=0x");
  Serial.print(addr, 16);
  Serial.print(", N=0x");
  Serial.println(cnt, 16);
#endif

#else // Wire library

  uint8_t n;
  uint8_t rcnt;

  Wire.beginTransmission(ee_address);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xff);
  Wire.endTransmission();
  // CAUTION: The Wire library defines an internal BUFFER_LENGTH (== 32)
  // for all read and write operations. Transferring more Bytes
  // in one transfer will result in erratic behaviour.
  do
  {
    rcnt = (cnt > 32 ? 32 : cnt);
    Wire.requestFrom(ee_address, rcnt);
    for (n = 0; n < rcnt; n++)
    {
      *buf++ = Wire.read();
    }
    cnt -= rcnt;
  } while (cnt > 0);

#endif
}


/*!
 * writes a number of bytes to the I2C-EEPROM at given address.
 * The I2C-bus address was set by argument to the constructor.
 * Write accesses are segmented to honor page boundaries.
 * This function may take some time since it waits for the EEPROM to
 * complete writing.
 *
 * @param addr  memory start address within EEPROM
 * @param buf   array of bytes to write from
 * @param cnt   number of bytes in buf to write
 */
void EE24xx::write(uint16_t addr, uint8_t *buf, uint16_t cnt)
{
  uint8_t n;
  uint16_t wcnt;
#if (EE24xx_DBG)
  uint16_t polls;
#endif

#if 1 // I2C library

  do
  {
    // maximum number of bytes to write per chunk: up to page end
    wcnt = (addr | ee_pagemask) - addr + 1;
    // if we need less, this is also OK.
    if (cnt < wcnt) {
      wcnt = cnt;
    }
#if (EE24xx_DBG)
  Serial.print("EE24xx::write I2C=0x");
  Serial.print(ee_address, 16);
  Serial.print(", Ai=0x");
  Serial.print(addr, 16);
  Serial.print(", Ni=0x");
  Serial.println(wcnt, 16);
#endif
  if (ee_maxaddress < 0x400)
  {
    I2c.write(ee_address | ((addr >> 8) & 0x7), (uint8_t)(addr & 0xff), buf, wcnt);
  } else {
    I2c.write(ee_address, (uint8_t)((addr >> 8) & 0xff), (uint8_t)(addr & 0xff), buf, wcnt);
  }
  addr += wcnt;
  buf += wcnt;
  cnt -= wcnt;
    // poll until EEPROM has completed writing data
#if (EE24xx_DBG)
    polls = 0;
#endif
    do
    {
#if (EE24xx_DBG)
      polls++;
#endif
      // dummy write one address byte to check busy state (no ACK)
      // during internal write cycle
      n = I2c.write(ee_address, (uint8_t)0);
    } while (n != 0);
#if (EE24xx_DBG)
    Serial.print("EE24xx::write P=");
    Serial.println(polls);
#endif
  } while (cnt > 0);

#else // Wire library

  // CAUTION: The Wire library defines an internal BUFFER_LENGTH (== 32)
  // for all read and write operations. Transferring more Bytes
  // in one transfer will result in erratic behaviour.
  // This also means loss of performance since one page must be written
  // in pieces when ee_pagesize is 32 or above.
  do
  {
    Wire.beginTransmission(ee_address);
    // send memory address of first byte
    Wire.write(addr >> 8);
    Wire.write(addr & 0xff);
    wcnt = (cnt > 30 ? 30 : cnt);
    do // for (n = 0; n < cnt; n++)
    {
      Wire.write(*buf++);
      addr++;
      wcnt--;
      cnt--;
      // loop until last address of a page (128 bytes) was written or finished
    } while ((wcnt > 0) && ((addr & ee_pagemask) != 0));
    Wire.endTransmission();

    // poll until EEPROM has completed writing data
    do
    {
      Wire.beginTransmission(ee_address);
      n = Wire.endTransmission();
    } while (n != 0);
  } while (cnt > 0);

#endif
}

