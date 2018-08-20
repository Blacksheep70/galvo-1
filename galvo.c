#include <errno.h>

#include <bcm2835.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "galvo.h"

#define PI 3.14159265

#define LDAC_PIN      RPI_V2_GPIO_P1_15
#define CLR_PIN       RPI_V2_GPIO_P1_13
#define COLOR_R_PIN   RPI_BPLUS_GPIO_J8_37
#define COLOR_G_PIN   RPI_BPLUS_GPIO_J8_35
#define COLOR_B_PIN   RPI_BPLUS_GPIO_J8_33 

/* AD5754R Register Map */
#define AD5754R_REG_DAC             0x00 // DAC register
#define AD5754R_REG_RANGE_SELECT    0x01 // Output range select register
#define AD5754R_REG_POWER_CONTROL   0x02 // Power control register
#define AD5754R_REG_CONTROL         0x03 // Control register

/* AD5754R Channel Address */
#define AD5754R_DAC_A               0x00 // Address of channel A
#define AD5754R_DAC_B               0x01 // Address of channel B
#define AD5754R_DAC_C               0x02 // Address of channel C
#define AD5754R_DAC_D               0x03 // Address of channel D
#define AD5754R_DAC_ALL             0x04 // All four DACs

/* AD5754R Range Bits */
#define AD5754R_UNIPOLAR_5_RANGE    0x00 // 0..+5(V)
#define AD5754R_UNIPOLAR_10_RANGE   0x01 // 0..+10(V)
#define AD5754R_UNIPOLAR_10_8_RANGE 0x02 // 0..+10.8(V)
#define AD5754R_BIPOLAR_5_RANGE     0x03 // -5..+5(V)
#define AD5754R_BIPOLAR_10_RANGE    0x04 // -10...+10(V)
#define AD5754R_BIPOLAR_10_8_RANGE  0x05 // -10.8...+10.8(V)

/* Control Functions */
#define AD5754R_NOP_OPERATION       0x00 // No operation instruction used in readback operations.
#define AD5754R_SDO_DISABLE         0x11 // Set by the user to disable the SDO output.
#define AD5754R_CLR_SELECT          0x12 // Selects the clear code: 0V, Midscale or Negative full scale.
#define AD5754R_CLAMP_ENABLE        0x14 // Set by the user to enable the current-limit clamp.
#define AD5754R_TSD_ENABLE          0x18 // Set by the user to enable the thermal shutdown feature.
#define AD5754R_CLEAR               0x40 // Sets the DAC registers to the clear code and updates the outputs.
#define AD5754R_LOAD                0x50 // Updates the DAC registers and, consequently, the DAC outputs.

#define MAX_VALUE                   65535;

int activeColor = 0;
void printBits(size_t const size, void const * const ptr) {
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  int i, j;

  for (i=0;i<=size-1;i++) {
    for (j=7;j>=0;j--) {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
    }
  }
  puts("");
}

/*unsigned short GetRegisterValue(unsigned char regBits, unsigned char adrBits) {
  unsigned char registerWord[4]   = {0};
  unsigned short data             = 0;
 
  registerWord[0] = 0x01;   // AD5754R CS.
  registerWord[1] = (1 << 7) + (regBits << 3) + adrBits;
  registerWord[2] = 0;
  registerWord[3] = 0;
  SPI_Write(registerWord,3);
  registerWord[0] = 0x01; // AD5754R CS.
  
  registerWord[1] = 0x18; // NOP instruction used in readback operations.
  registerWord[2] = 0;
  registerWord[3] = 0;
  SPI_Read(registerWord,3);
  data = (registerWord[1] << 8) + registerWord[2];

  return(data);
}

void AD5754R_SetPower(unsigned char dacChannel, unsigned char onOff) {
  unsigned short oldPowerCtl = 0;
  unsigned short newPowerCtl = 0;
  
  if(dacChannel == AD5754R_DAC_ALL) {
    dacChannel = 0x0F;
  } else {
    dacChannel = (1 << dacChannel);
  }

  oldPowerCtl = getRegisterValue(AD5754R_REG_POWER_CONTROL,0x00);
  newPowerCtl = oldPowerCtl & ~dacChannel;
  newPowerCtl = newPowerCtl | (onOff * dacChannel);
  
  setRegisterValue(AD5754R_REG_POWER_CONTROL,0x00,newPowerCtl);
}*/

void setRegisterValue(unsigned char regBits, unsigned char adrBits, unsigned short registerValue) {
  unsigned char registerWord[3] = {0};
  registerWord[0] = (regBits << 3) + adrBits; //| (1<<7);
  registerWord[1] = (registerValue & 0xFF00) >> 8;
  registerWord[2] = (registerValue & 0x00FF) >> 0;
  
  //printBits(3, &registerWord); 
  bcm2835_spi_writenb(registerWord, 3);
}

// turn on all DAC's
// set output voltage range
// 
// 

// 0 = -10V
// 65535 = +10V

double step = 2 * PI/360;
double startRadius = 65535 / 4;
double centerX = 65535 / 2;
double centerY = 65535 / 2;

unsigned short scale(double value) {
  return (value / 65535.0) * 65535;
}

unsigned short swap(unsigned short input) {
  return (input>>8) | (input<<8);
}

void setOutput(unsigned short a, unsigned short b, unsigned short c, unsigned short d) {
  bcm2835_gpio_write(LDAC_PIN, HIGH);

  setRegisterValue(AD5754R_REG_DAC, AD5754R_DAC_A, a);
  setRegisterValue(AD5754R_REG_DAC, AD5754R_DAC_B, b);
  setRegisterValue(AD5754R_REG_DAC, AD5754R_DAC_C, c);
  setRegisterValue(AD5754R_REG_DAC, AD5754R_DAC_D, d);

  bcm2835_gpio_write(LDAC_PIN, LOW);
}

void setXY(unsigned short x, unsigned short y) {
  unsigned short x1 = abs(x - 65535);
  unsigned short y1 = abs(y - 65535);

  //printf("x, %d, x1, %d, y, %d, y1, %d", x, x1, y, y1);

  setOutput(x, x1, y, y1);
}

void setColor() {
  if(activeColor == 0) {
    activeColor = 1;
    bcm2835_gpio_write(COLOR_B_PIN, LOW);
    bcm2835_gpio_write(COLOR_G_PIN, LOW);
    bcm2835_gpio_write(COLOR_R_PIN, HIGH);
  } else if(activeColor == 1) {
    activeColor = 2;
    bcm2835_gpio_write(COLOR_B_PIN, LOW);
    bcm2835_gpio_write(COLOR_G_PIN, HIGH);
    bcm2835_gpio_write(COLOR_R_PIN, LOW);
  } else if(activeColor == 2) {
    activeColor = 0;
    bcm2835_gpio_write(COLOR_B_PIN, HIGH);
    bcm2835_gpio_write(COLOR_G_PIN, LOW);
    bcm2835_gpio_write(COLOR_R_PIN, LOW);
  }
}

void drawCircle(double radius, int direction) {
  for(double theta = 0; theta < 2*PI; theta += step) {
    double x = centerX + (radius * cos(theta));
    double y = centerY + (radius * sin(theta));
   
    //usleep(100);
    setXY(scale(x), scale(y)); 
  }

  double newRadius;
  int newDirection = direction;
  if(direction == 0) {
    newRadius = radius - 100;
    if(newRadius <= 0) {
      newDirection = 1;
      setColor();
    }
    drawCircle(newRadius, newDirection);
  } else { 
    newRadius = radius + 100;
    if(newRadius >= startRadius) {
      newDirection = 0;
      setColor();
    }
    drawCircle(newRadius, newDirection);
  }
}

int initGalvo() {
  if (!bcm2835_init()) {
    return 1;
  }

  bcm2835_gpio_fsel(LDAC_PIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(CLR_PIN, BCM2835_GPIO_FSEL_OUTP);

  bcm2835_gpio_fsel(COLOR_R_PIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(COLOR_G_PIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(COLOR_B_PIN, BCM2835_GPIO_FSEL_OUTP);

  bcm2835_gpio_write(COLOR_R_PIN, LOW);
  bcm2835_gpio_write(COLOR_G_PIN, LOW);
  bcm2835_gpio_write(COLOR_B_PIN, LOW);

  bcm2835_gpio_write(LDAC_PIN, LOW);
  bcm2835_gpio_write(CLR_PIN, HIGH);

  int result = bcm2835_spi_begin();
  
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE2);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

  // turn on all four DAC's
  setRegisterValue(AD5754R_REG_POWER_CONTROL, 0, 0x0f);

  // set them all to +/- 10V
  setRegisterValue(AD5754R_REG_RANGE_SELECT, AD5754R_DAC_ALL, AD5754R_BIPOLAR_10_RANGE);

  usleep(100);

  // set output to all 0V
  unsigned short initialValue = 65535 / 2;
  setOutput(initialValue, initialValue, initialValue, initialValue);

  //unsigned short out = scale(65535 / 2);
  //unsigned short out2 = swap(out);

 // printBits(2, &out2);
  //printf("x, %d", out);
  setColor();
  drawCircle(startRadius, 0);

  bcm2835_close();
  return 0;
}

void closeGalvo() {
  bcm2835_close();
}
