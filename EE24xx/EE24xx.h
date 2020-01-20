/*!
 * \file EE24xx.cpp
 * Functions for accessing the 24LCxx I2C EEPROMs from Microchip et al.
 *
 * \author Andreas Czechanowski, DL4SDC
 * \date 2012-11-25  Created
 * \date 2012-12-26  Changed from Wire to I2C library
 * \date 2019-10-06  Added EE24x08 and EE24x16
 */

/*!
 * \mainpage
 *
 * \section Datasheets
 * - Microchip 24xxnnn ( http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=2697 )
 * - Atmel AT24Cnnn ( http://www.atmel.com/products/Memories/serial/i2c.aspx )
 */

#ifndef __EE24xx_H__
#define __EE24xx_H__

enum ee24Type {
  EE24x08 = 10,
  EE24x16 = 11,
  EE24x32 = 12,
  EE24x64 = 13,
  EE24x128 = 14,
  EE24x256 = 15,
  EE24x512 = 16
};

class EE24xx
{
  // accessable from anywhere outside
  public:
    EE24xx(uint8_t ee_address, ee24Type ee_model);

    void read(uint16_t addr, uint8_t *buf, uint16_t cnt);

    void write(uint16_t addr, uint8_t *buf, uint16_t cnt);

  // only accessible by own class functions
  private:
    uint8_t  ee_address;
    uint8_t  ee_pagemask;
    uint32_t ee_maxaddress;

  // accessible also by derived classes
  protected:
};

#endif /* __EE24xx_H__ */
