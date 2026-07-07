
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

// read-only memory mapped 32-bit register.  only a getter is generated, since
// writing a read-only register makes no sense (e.g. an input capture register).
#define mem_reg32_ro(reg_name, addr, ...) \
enum { reg_name ## _ADDR = addr }; \
reg_def_inline uint32_t get_ ## reg_name ## _raw_reg_value (void) { return *(volatile uint32_t*)addr; } \
expand_define_reg(reg_name, uint32_t, __VA_ARGS__)

// ------------------------------------------------------------------------
// multi-instance memory mapped register access.
//
// several device instances share the same register layout but live at
// 'base + instance * stride'.  these macros emit the '_1' low level accessors
// that reg_def.h dispatches to for the '(reg_name, instance)' tuple form, with
// the instance passed through as the channel index.

#define mem_reg16_n(reg_name, base, stride, ...) \
enum { reg_name ## _ADDR = base, reg_name ## _STRIDE = stride }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value_1 (unsigned inst, uint16_t v) { *(volatile uint16_t*)(uintptr_t)((base) + (inst) * (stride)) = v; } \
reg_def_inline uint16_t get_ ## reg_name ## _raw_reg_value_1 (unsigned inst) { return *(volatile uint16_t*)(uintptr_t)((base) + (inst) * (stride)); } \
reg_def_inline void rmw_ ## reg_name ## _1 (unsigned inst, uint16_t a, uint16_t o) { volatile uint16_t* p = (volatile uint16_t*)(uintptr_t)((base) + (inst) * (stride)); *p = (*p & a) | o; } \
expand_define_reg(reg_name, uint16_t, __VA_ARGS__)

#define mem_reg32_n(reg_name, base, stride, ...) \
enum { reg_name ## _ADDR = base, reg_name ## _STRIDE = stride }; \
reg_def_inline void set_ ## reg_name ## _raw_reg_value_1 (unsigned inst, uint32_t v) { *(volatile uint32_t*)(uintptr_t)((base) + (inst) * (stride)) = v; } \
reg_def_inline uint32_t get_ ## reg_name ## _raw_reg_value_1 (unsigned inst) { return *(volatile uint32_t*)(uintptr_t)((base) + (inst) * (stride)); } \
reg_def_inline void rmw_ ## reg_name ## _1 (unsigned inst, uint32_t a, uint32_t o) { volatile uint32_t* p = (volatile uint32_t*)(uintptr_t)((base) + (inst) * (stride)); *p = (*p & a) | o; } \
expand_define_reg(reg_name, uint32_t, __VA_ARGS__)


// ------------------------------------------------------------------------
// SH7091 TMU (timer unit) register description  (hardware manual section 12)
//
// the TMU has three 32-bit auto-reload timer channels.  each channel has a
// timer constant register (TCOR), a timer counter (TCNT) and a timer control
// register (TCR), laid out at 'base + channel * 0x0C':
//
//   channel   TCOR         TCNT         TCR
//   0         0xFFD80008   0xFFD8000C   0xFFD80010
//   1         0xFFD80014   0xFFD80018   0xFFD8001C
//   2         0xFFD80020   0xFFD80024   0xFFD80028
//
// TCOR and TCNT are identical across all channels, so they are described once
// and accessed per channel via the '(reg, channel)' tuple form, e.g.
// 'get_reg ((TCNT, 1))'.
//
// TCR channels 0 and 1 share a layout; channel 2's TCR additionally has the
// input capture flag (ICPF) and control (ICPE) bits.  channel 2 therefore has
// its own dedicated 'TCR2' definition (its counter / constant still use the
// shared '(TCOR, 2)' / '(TCNT, 2)').  channel 2 also has the read-only input
// capture register TCPR2 at 0xFFD8002C.

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

// TCOR / TCNT: same layout on all three channels -> multi-instance access.
mem_reg32_n (TCOR, 0xFFD80008, 0x0C
  , reg_bits (value, 31, 0, uint32_t)
)

mem_reg32_n (TCNT, 0xFFD8000C, 0x0C
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

// input capture control (TCR2 channel 2 only).  0b01 is reserved.
enum tcr_icpe_t
{
  icpe_input_capture_disabled = 0b00,
  icpe_input_capture_no_interrupt = 0b10,
  icpe_input_capture_interrupt = 0b11
};

// TCR channels 0 and 1 -> multi-instance access via '(TCR, 0)' / '(TCR, 1)'.
mem_reg16_n (TCR, 0xFFD80010, 0x0C
  , reg_bits (reserved0, 15, 9, uint8_t)
  , reg_bits (unf, 8, 8, bool)
  , reg_bits (reserved1, 7, 6, uint8_t)
  , reg_bits (unie, 5, 5, bool)
  , reg_bits (ckeg1, 4, 3, enum tcr_ckeg_t)
  , reg_bits (tpsc2, 2, 0, enum tcr_tpsc_t)
)

// TCR channel 2: adds the input capture flag / control bits.
mem_reg16 (TCR2, 0xFFD80028
  , reg_bits (reserved0, 15, 10, uint8_t)
  , reg_bits (icpf, 9, 9, bool)
  , reg_bits (unf, 8, 8, bool)
  , reg_bits (icpe1, 7, 6, enum tcr_icpe_t)
  , reg_bits (unie, 5, 5, bool)
  , reg_bits (ckeg1, 4, 3, enum tcr_ckeg_t)
  , reg_bits (tpsc2, 2, 0, enum tcr_tpsc_t)
)

// TCPR2 channel 2 input capture register (read only).
mem_reg32_ro (TCPR2, 0xFFD8002C
  , reg_bits (value, 31, 0, uint32_t)
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

// configure and start any of the three TMU channels.  the channel is passed
// as the instance value of the '(reg, channel)' tuple, so the same code drives
// all three timers.
void start_tmu (unsigned ch, uint32_t count)
{
  set_reg ((TCR, ch),
    (tpsc2, tpsc_p_256),
    (unie, true)
  );
  reset_reg ((TCOR, ch), (value, count));   // auto-reload value
  reset_reg ((TCNT, ch), (value, count));   // initial counter value

  // start channel 'ch' by setting its STR bit in TSTR without disturbing the
  // other channels.
  rmw_reg (TSTR, TSTR_all_bits, 1u << ch);
}

// set up channel 2 to capture TCNT2 into TCPR2 on the rising edge of the
// external TCLK signal.
void start_tmu2_input_capture (uint32_t reload)
{
  reset_reg (TOCR, (tcoe, false));          // TCLK = input capture control input

  reset_reg ((TCOR, 2), (value, reload));
  reset_reg ((TCNT, 2), (value, reload));

  reset_reg (TCR2,
    (tpsc2, tpsc_ext_clock),
    (ckeg1, ckeg_count_capture_rising_edge),
    (icpe1, icpe_input_capture_no_interrupt),
    (unie,  false)
  );

  rmw_reg (TSTR, TSTR_all_bits, 1u << 2);   // start channel 2
}

uint32_t read_tmu2_capture (void)
{
  return get_reg (TCPR2);
}

// read a field of a given channel's control register via the tuple form.
bool tmu_underflowed (unsigned ch)
{
  return get_reg ((TCR, ch), unf);
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