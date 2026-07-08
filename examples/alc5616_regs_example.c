// gcc -O2 alc5616_regs_example.c -I./../

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void my_i2c_write_reg16 (uint8_t i2c_addr, uint8_t reg_addr, uint16_t val)
{
  // ...
}

uint16_t my_i2c_read_reg16 (uint8_t i2c_addr, uint8_t reg_addr)
{
  // ...
  return 0;
}

#define alc5616_write_reg16(reg_addr, val) my_i2c_write_reg16 (0x36, reg_addr, val)
#define alc5616_read_reg16(reg_addr) my_i2c_read_reg16 (0x36, reg_addr)

#include "alc5616_regs.h"



int main (void)
{
  return EXIT_SUCCESS;
}