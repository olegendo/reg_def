
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "reg_def.h"

// ------------------------------------------------------------------------
// simple memory mapped register access

#define mem_reg8(reg_name, addr, ...) \
enum { reg_name ## _ADDR = addr }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value (uint8_t v) { *(volatile uint8_t*)addr = v; } \
reg_def_inline uint8_t get_ ## reg_name ## _raw_reg_value (void) { return *(volatile uint8_t*)addr; } \
reg_def_inline void rmw_ ## reg_name (uint8_t a, uint8_t o) { *(volatile uint8_t*)addr = ((*(volatile uint8_t*)addr) & a) | o; } \
expand_define_reg(reg_name, uint8_t, __VA_ARGS__)

#define mem_reg16(reg_name, addr, ...) \
enum { reg_name ## _ADDR = addr }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value (uint16_t v) { *(volatile uint16_t*)addr = v; } \
reg_def_inline uint16_t get_ ## reg_name ## _raw_reg_value (void) { return *(volatile uint16_t*)addr; } \
reg_def_inline void rmw_ ## reg_name(uint16_t a, uint16_t o) { *(volatile uint16_t*)addr = ((*(volatile uint16_t*)addr) & a) | o; } \
expand_define_reg(reg_name, uint16_t, __VA_ARGS__)

#define mem_reg32(reg_name, addr, ...) \
enum { reg_name ## _ADDR = addr }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value (uint32_t v) { *(volatile uint32_t*)addr = v; } \
reg_def_inline uint32_t get_ ## reg_name ## _raw_reg_value (void) { return *(volatile uint32_t*)addr; } \
reg_def_inline void rmw_ ## reg_name(uint32_t a, uint32_t o) { *(volatile uint32_t*)addr = ((*(volatile uint32_t*)addr) & a) | o; } \
expand_define_reg(reg_name, uint32_t, __VA_ARGS__)


// ------------------------------------------------------------------------
// SH7091 TMU0 register description

mem_reg8 (TOCR, 0xFFD80000
  , reg_bits (reserved, 7, 1, uint8_t)
  , reg_bits (tcoe, 0, 0, bool)
)

mem_reg8 (TSTR, 0xFFD80004
  , reg_bits (reserved, 7, 3, uint8_t)
  , reg_bits (str2, 2, 2, bool)
  , reg_bits (str1, 1, 1, bool)
  , reg_bits (str0, 0, 0, bool)
)

mem_reg32 (TCOR0, 0xFFD80008
  , reg_bits (value, 31, 0, uint32_t)
)

mem_reg32 (TCNT0, 0xFFD8000C
  , reg_bits (value, 31, 0, uint32_t)
)

enum tcr_ckeg_t
{
  ckeg_count_capture_rising_edge = 0b00,
  ckeg_count_capture_falling_edge = 0b01,
  ckeg_count_capture_any_edge = 0b10
};

enum tcr_tpsc_t
{
  tpsc_p_4 = 0b000,
  tpsc_p_16 = 0b001,
  tpsc_p_64 = 0b010,
  tpsc_p_256 = 0b011,
  tpsc_p_1024 = 0b100,
  // tpsc_reserved = 0b101
  tpsc_rtc_clock = 0b110,
  tpsc_ext_clock = 0b111
};

mem_reg16 (TCR0, 0xFFD80010
  , reg_bits (reserved0, 15, 9, uint8_t)
  , reg_bits (unf, 8, 8, bool)
  , reg_bits (reserved1, 7, 6, uint8_t)
  , reg_bits (unie, 5, 5, bool)
  
  , reg_bits (ckeg1, 4, 3, enum tcr_ckeg_t)

  , reg_bits (tpsc2, 2, 0, enum tcr_tpsc_t)
)

// ------------------------------------------------------------------------
// SH7091 WDT registers

// the WDT registers need to be written as 16-bit with the high-byte set to
// 0x5A or 0xA5

enum wdt_cks_t
{
  wdt_cks_1_32 = 0b000,
  wdt_cks_1_64 = 0b001,
  wdt_cks_1_128 = 0b010,
  wdt_cks_1_256 = 0b011,
  wdt_cks_1_512 = 0b100,
  wdt_cks_1_1024 = 0b101,
  wdt_cks_1_2048 = 0b110,
  wdt_cks_1_4096 = 0b111
};


#if 0
enum { WTCNT_ADDR = 0xFFC00008 };
inline void set_WTCNT_raw_reg_value (uint8_t v) { *(volatile uint16_t*)0xFFC00008 = v | 0x5A00; }
inline uint8_t get_WTCNT_raw_reg_value (void) { return *(volatile uint8_t*)0xFFC00008; }
inline void rmw_WTCNT (uint8_t a, uint8_t o) { set_WTCNT_raw_reg_value ((get_WTCNT_raw_reg_value () & a) | o); }

expand_define_reg (WTCNT, uint8_t
  , reg_bits (counter, 7, 0, uint8_t)
)

inline void set_WTCSR_raw_reg_value (uint8_t v) { *(volatile uint16_t*)0xFFC0000C = v | 0xA500; }
inline uint8_t get_WTCSR_raw_reg_value (void) { return *(volatile uint8_t*)0xFFC0000C; }
inline void rmw_WTCSR (uint8_t a, uint8_t o) { set_WTCSR_raw_reg_value ((get_WTCSR_raw_reg_value () & a) | o); }

expand_define_reg (WTCSR, uint8_t
  , reg_bits (tme, 7, 7, bool)
  , reg_bits (wt_it, 6, 6, bool)
  , reg_bits (rsts, 5, 5, bool)
  , reg_bits (wovf, 4, 4, bool)
  , reg_bits (iovf, 3, 3, bool)
  , reg_bits (cks, 2, 0, enum wdt_cks_t)
)
#endif


#define wdt_reg8(reg_name, addr, write_magic_value, ...) \
enum { reg_name ## _ADDR = addr }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value (uint8_t v) { *(volatile uint16_t*)addr = v | write_magic_value; } \
reg_def_inline uint8_t get_ ## reg_name ## _raw_reg_value (void) { return *(volatile uint8_t*)addr; } \
reg_def_inline void rmw_ ## reg_name (uint8_t a, uint8_t o) { *(volatile uint8_t*)addr = ((*(volatile uint8_t*)addr) & a) | o; } \
expand_define_reg(reg_name, uint8_t, __VA_ARGS__)

wdt_reg8 (WTCNT, 0xFFC00008, 0x5A00
  , reg_bits (counter, 7, 0, uint8_t)
)

wdt_reg8 (WTCSR, 0xFFC0000C, 0xA500
  , reg_bits (tme, 7, 7, bool)
  , reg_bits (wt_it, 6, 6, bool)
  , reg_bits (rsts, 5, 5, bool)
  , reg_bits (wovf, 4, 4, bool)
  , reg_bits (iovf, 3, 3, bool)
  , reg_bits (cks, 2, 0, enum wdt_cks_t)
)

// ------------------------------------------------------------------------
// example

void start_tmu0 (uint32_t count)
{
  reset_reg (TCNT0, (value, count));
  set_reg (TSTR, (str0, true));
}

void start_wdt (void)
{
  set_reg (WTCSR, (cks, wdt_cks_1_256));
  reset_reg (WTCNT, (counter, 0xFF));
}


int main (void)
{
  return EXIT_SUCCESS;
}