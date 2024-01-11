/*
Copyright (c) 2023 Oleg Endo

see also LICENSE file.


C preprocessor based register definition utilities which should help in
describing a larger MCU in a homogeneous style, with automatic code generation
for register access and manipulation.

an alternative to all of this would be using structs and bitfields and designated
initializers, but SDCC does not support passing structs as arguments or return
values, even if those can be evaluated at compile time.

since SDCC is not a very sophisticated, try doing as many things as possible
in the preprocessor as that guarantees compile-time evaluation.



example register definition
----------------------------

xfr_reg8 is a macro that expands the register definition by using the 
expand_define_reg macro.  it implements only raw value register read and
write functions.

xfr_reg8 (ADC_B_CONTROL, 0xFF0E,
  reg_bits (start_running,   7, 7, bool),         // (field name, end bit, start bit, extracted type)
  reg_bits (test,            6, 6, bool),
  reg_bits (mode,            5, 5, enum adc_b_mode),
  reg_bits (en_interrupt,    4, 4, bool),
  reg_bits (en_debounce,     3, 3, bool),
  reg_bits (bias_adj,        2, 1, enum adc_bias_adj_val),
  reg_bits (ck_sel,          0, 0, enum adc_b_ck_sel_val)
)

a list of 'reg_bits' declarations is used to describe a bitfield in the register.
the declaration format is chosen to be similar to what can be commonly found in
MCU data sheets.  it will automatically generate functions to access that field,
like in this case
   enum ADC_B_CONTROL_bits
   {
     ADC_B_CONTROL_start_running_value_mask = < bit mask value >,
     ADC_B_CONTROL_start_running_mask = < bit mask value >,

     ADC_B_CONTROL_test_value_mask = ...,

     ....
   };

   void set_ADC_B_CONTROL_start_running (bool val)
   bool get_ADC_B_CONTROL_start_running (void)
   void set_ADC_B_CONTROL_test (bool val)
   bool get_ADC_B_CONTROL_test (void)
   ...

these are then accessible via a generic get/set/reset wrapper macros, to make
it look more functional.  these generic warpper macros can also be used to
access special case implementations of register access.  the register
implementation just has to follow the above naming scheme.

the enum is not really required but reduces the compile time significantly
on SDCC.

in order to represent scattered bitfields easier to deal with, 'reg_bits_2'
can be used to define a split bitfield:

xfr_reg8 (EXAMPLE_REG, 0xFF00,
  reg_bits_2 (scattered, 7, 5, 1, 0, uint8_t)
)

This defines a scattered bitfield of a total 5 bits with the layout xxx...yy.
It will generate code to get/set the 'scattered' field as a contiguous
5 bit value 'xxxyy'.


available access methods
------------------------

  get_reg ( < reg name > [, field ] )

     read the hardware register and extract the specified field from the value.
     if no field is specified, the whole register raw value is returned.


  reset_reg ( < reg name >, (raw_reg_value, < value >) )
  reset_reg ( < reg name >, < value > )

     overwrite the hardware register with the specified raw value.


  reset_reg ( < reg name >, (field, field value) [, (field, field value), ... ] )

     construct a register raw value from the specified fields and their values,
     then overwrite the hardware register with the resulting raw value.


  set_reg ( < reg name >, (field, field value) [, (field, field value), ... ] )

     read-modify the hardware register to set only the specified fields to
     the new values.  other fields will be written back as they were read
     from the register.


  make_reg_value ( < reg name >, (field, field value) [, (field, field value), ... ] )

     construct a register raw value from the specified fields and their values.
     does not modify the hardware register.


  get_reg_value (value, < reg name > [, field ] )

     extract the specified field from the value.
     does not modify the hardware register


  set_reg_value (value, < reg name >, (field, field value) [, (field, field value), ... ] > )

     modify the specified value variable by setting the specified fields to their
     respective values.  does not modify the hardware register.


example register read access
----------------------------

// read the full raw register contents

  uint8_t r0 = get_reg (ADC_B_CONTROL);

  uint8_t r1 = get_reg (ADC_B_CONTROL, raw_reg_value);


// read a field

  uint8_t r2 = get_reg (ADC_B_CONTROL, en_interrupt);



example register write access
----------------------------

// reset a register completey, overwriting all the register contents.
// the register is not read, only written.

  reset_reg (ADC_B_CONTROL,
    (start_running, true),
    (mode,          adc_b_continuous),
    (en_interrupt,  false),
    (bias_adj,      adc_bias_25u),
    (ck_sel,        adc_ck_pos)
  );


  reset_reg (ADC_B_CONTROL, (raw_reg_value, 0x11));

  reset_reg (ADC_B_CONTROL, 0x11);

  reset_reg (ADC_B_CONTROL, make_reg_value (
    (start_running, true),
    (mode,          adc_b_continuous),
    (en_interrupt,  false),
    (bias_adj,      adc_bias_25u),
    (ck_sel,        adc_ck_pos)
  ))


// partically set one or more register field.
// the register is read, modified and written back

  set_reg (ADC_B_CONTROL,
    (en_interrupt, false),
    (mode, adc_b_continuous),
    (start_running, false)
  );

  set_reg (ADC_B_CONTROL,
    (start_running, true)
  );

*/


#ifndef includeguard_reg_def_h_includeguard
#define includeguard_reg_def_h_includeguard

#include <stdbool.h>
#include <stdint.h>

#include "pp_for_each.h"

#define make_bitmask(t, n) (t)((uint64_t)(1ll << (n)) - 1ll)

#define reg_bits(enum_name, high_bit, low_bit, type) (enum_name, type, high_bit, low_bit)
#define reg_bits_2(enum_name, high_bit0, low_bit0, high_bit1, low_bit1, type) (enum_name, type, high_bit0, low_bit0, high_bit1, low_bit1)

// for single contiguous bitfield
#define expand_reg_bits_for_enum_____6(reg_name, reg_type, enum_name, type, high_bit, low_bit) \
   reg_name ## _ ## enum_name ## _value_mask = make_bitmask(reg_type, (high_bit) - (low_bit) + 1), \
   reg_name ## _ ## enum_name ## _mask = reg_name ## _ ## enum_name ## _value_mask << (low_bit), \

// for split/scattered bitfield
//     xxx...yy
//      ^    ^--- high_bit1 = 1, low_bit1 = 0
//      |
//       high_bit0 = 7, low_bit = 5
//
//  enum value / mask = 0b11100011
//  value_mask        = 0b00011111
//  value_mask0       = 0b11100000
//  value_mask1       = 0b00000011
#define expand_reg_bits_for_enum_____8(reg_name, reg_type, enum_name, type, high_bit0, low_bit0, high_bit1, low_bit1) \
   reg_name ## _ ## enum_name ## _mask = make_bitmask(reg_type, (high_bit0) - (low_bit0) + 1) << (low_bit0) \
                                | make_bitmask(reg_type, (high_bit1) - (low_bit1) + 1) << (low_bit1), \
   reg_name ## _ ## enum_name ## _value_mask = make_bitmask(reg_type, ((high_bit0) - (low_bit0) + 1) + ((high_bit1) - (low_bit1) + 1)), \
   reg_name ## _ ## enum_name ## _value_mask0 = make_bitmask(reg_type, (high_bit0) - (low_bit0) + 1) << ((high_bit1) - (low_bit1) + 1), \
   reg_name ## _ ## enum_name ## _value_mask1 = make_bitmask(reg_type, (high_bit1) - (low_bit1) + 1),

#define expand_reg_bits_for_enum___(...) pp_concat (expand_reg_bits_for_enum_____, pp_args_size (__VA_ARGS__)) (__VA_ARGS__)
#define expand_reg_bits_for_enum__(...) __VA_ARGS__
#define expand_reg_bits_for_enum_(reg_name, ...) expand_reg_bits_for_enum___ (reg_name, expand_reg_bits_for_enum__ __VA_ARGS__)
#define expand_reg_bits_for_enum(reg_name_type, x) expand_reg_bits_for_enum_(expand_reg_bits_for_enum__ reg_name_type, x)


#define expand_reg_bits_funcs____6(reg, reg_type, field, type, high_bit, low_bit) \
inline reg_type make_ ## reg ## _ ## field (type v) \
{ \
  return ((reg_type)v << (low_bit)) & reg ## _ ## field ## _mask;\
}\
inline reg_type set1_ ## reg ## _ ## field (type v, reg_type r)\
{\
  return (r & ~( reg ## _ ## field ## _mask)) | make_ ## reg ## _ ## field (v); \
}\
inline void set_ ## reg ## _ ## field (type v)\
{\
  set_ ## reg ## _raw_reg_value (set1_ ## reg ## _ ## field (v, get_ ## reg ## _raw_reg_value ()));\
}\
inline reg_type get_ ## reg ## _ ## field ## _mask (void)\
{\
  return reg ## _ ## field ## _mask;\
} \
inline type get_ ## reg ## _ ## field (void) \
{ \
  static_assert ((high_bit) >= (low_bit), "high bit < low bit"); \
  static_assert (( (high_bit) - (low_bit) + 1) <= sizeof (reg_type) * 8, "high bit - low bit > register type bits"); \
  static_assert (((high_bit) - (low_bit) + 1) <= sizeof (type) * 8, "high bit - low bit > field type bits"); \
  return (type) (( get_ ## reg ## _raw_reg_value () >> (low_bit) ) & reg ## _ ## field ## _value_mask ); \
} \
inline type get_ ## reg ## _value_ ## field (reg_type val) \
{ \
  static_assert ((high_bit) >= (low_bit), "high bit < low bit"); \
  static_assert (( (high_bit) - (low_bit) + 1) <= sizeof (reg_type) * 8, "high bit - low bit > register type bits"); \
  static_assert (((high_bit) - (low_bit) + 1) <= sizeof (type) * 8, "high bit - low bit > field type bits"); \
  return (type) (( val >> (low_bit) ) & reg ## _ ## field ## _value_mask ); \
}


#define expand_reg_bits_funcs____8(reg, reg_type, field, type, high_bit0, low_bit0, high_bit1, low_bit1) \
inline reg_type make_ ## reg ## _ ## field (type v) \
{\
  const reg_type vv = (reg_type)v; \
  return (type)0 \
    | (((vv & reg ## _ ## field ## _value_mask0) >> ((high_bit1) - (low_bit1) + 1)) << (low_bit0)) \
    | (((vv & reg ## _ ## field ## _value_mask1) >> (0)) << (low_bit1)); \
}\
inline reg_type set1_ ## reg ## _ ## field (type v, reg_type r)\
{\
  return (r & ~( reg ## _ ## field ## _mask)) | make_ ## reg ## _ ## field (v); \
}\
inline void set_ ## reg ## _ ## field (type v)\
{\
  set_ ## reg ## _raw_reg_value (set1_ ## reg ## _ ## field (v, get_ ## reg ## _raw_reg_value ()));\
}\
inline reg_type get_ ## reg ## _ ## field ## _mask (void)\
{\
  return reg ## _ ## field ## _mask;\
}\
inline type get_ ## reg ## _ ## field (void) \
{ \
  static_assert ((high_bit0) >= (low_bit0), "high_bit0 >= low_bit0"); \
  static_assert ((high_bit1) >= (low_bit1), "high_bit1 >= low_bit1"); \
  static_assert ((low_bit0) > (low_bit1), "low_bit0 > low_bit1"); \
  static_assert (((high_bit1) - (low_bit1) + 1) < sizeof (reg_type) * 8); \
  static_assert (((high_bit1) - (low_bit1) + 1) < sizeof (type) * 8); \
  const reg_type r = get_ ## reg ## _raw_reg_value (); \
  const reg_type r0 = (r >> (low_bit0)) << ((high_bit1) - (low_bit1) + 1); \
  const reg_type r1 = (r >> (low_bit1)); \
  return (r0 & reg ## _ ## field ## _value_mask0) | (r1 & reg ## _ ## field ## _value_mask1); \
}\
inline type get_ ## reg ## _value_ ## field (reg_type val) \
{ \
  static_assert ((high_bit0) >= (low_bit0), "high_bit0 >= low_bit0"); \
  static_assert ((high_bit1) >= (low_bit1), "high_bit1 >= low_bit1"); \
  static_assert ((low_bit0) > (low_bit1), "low_bit0 > low_bit1"); \
  static_assert (((high_bit1) - (low_bit1) + 1) < sizeof (reg_type) * 8); \
  static_assert (((high_bit1) - (low_bit1) + 1) < sizeof (type) * 8); \
  const reg_type r = val; \
  const reg_type r0 = (r >> (low_bit0)) << ((high_bit1) - (low_bit1) + 1); \
  const reg_type r1 = (r >> (low_bit1)); \
  return (r0 & reg ## _ ## field ## _value_mask0) | (r1 & reg ## _ ## field ## _value_mask1); \
}


#define expand_reg_bits_funcs___(...) pp_concat (expand_reg_bits_funcs____, pp_args_size (__VA_ARGS__)) (__VA_ARGS__)

#define expand_reg_bits_funcs__(...) __VA_ARGS__
#define expand_reg_bits_funcs_(reg_name, ...) expand_reg_bits_funcs___ (reg_name, expand_reg_bits_funcs__ __VA_ARGS__)

#define expand_reg_bits_funcs(reg_name_type, x) expand_reg_bits_funcs_(expand_reg_bits_funcs__ reg_name_type, x)


// do not expand an empty "enum { };".
// it's rejected as an error by the compiler.
#define expand_reg_bits_enum_1(...)

#define expand_reg_bits_enum_0(reg_name, reg_raw_type, ...)\
enum reg_name ## _bits \
{ \
  reg_name ## _all_bits = make_bitmask(reg_raw_type, sizeof(reg_raw_type)*8), \
  pp_for_each_i (expand_reg_bits_for_enum, (reg_name, reg_raw_type), __VA_ARGS__) \
};

#define expand_reg_bits_enum_(reg_name, reg_raw_type, is_empty, ...) expand_reg_bits_enum_ ## is_empty (reg_name, reg_raw_type, __VA_ARGS__)
#define expand_reg_bits_enum__(...) expand_reg_bits_enum_ (__VA_ARGS__)
#define expand_reg_bits_enum(reg_name, reg_raw_type, ...) expand_reg_bits_enum__(reg_name, reg_raw_type, pp_args_empty(__VA_ARGS__), __VA_ARGS__)


#define expand_define_reg(reg_name, reg_raw_type, ...) \
  expand_reg_bits_enum (reg_name, reg_raw_type, __VA_ARGS__) \
  pp_for_each_i (expand_reg_bits_funcs, (reg_name, reg_raw_type), __VA_ARGS__) \
  inline reg_raw_type make_ ## reg_name ## _raw_reg_value (reg_raw_type v) { return v; }

// expand '(name, value)' or 'value' that came from
//     reset_reg (field, value)
// or
//     reset_reg (value)

// expand_set_all_expand_value_pair
// invoked with '(field, value)':
//      -> expand_set_all_expand_value_pair (field, value) -> field, value
//      -> pp_args_size will evaluate to '2'
//
// invoked with 'value':
//      -> expand_set_all_expand_value_pair 123 -> expand_set_all_expand_value_pair 123
//      -> pp_args_size will evaluate to '1' (the following 123 value is garbage and will be ignored)
#define expand_set_all_expand_value_pair(...) __VA_ARGS__

#define expand_set_all__2__(reg, field, val)  | make_ ## reg ## _ ## field (val)
#define expand_set_all__2_(...) expand_set_all__2__ (__VA_ARGS__)
#define expand_set_all__2(reg, val) expand_set_all__2_ (reg, expand_set_all_expand_value_pair val)

#define expand_set_all__1(reg, val) | make_ ## reg ## _raw_reg_value (val)

#define expand_set_all__n(reg, n, ...) pp_concat (expand_set_all__, n) (reg, __VA_ARGS__)
#define expand_set_all_(reg, ...) expand_set_all__n (reg, __VA_ARGS__)
#define expand_set_all(reg, x) expand_set_all_ (reg, pp_args_size (expand_set_all_expand_value_pair x), x)


#define reset_reg_1(reg, ...) set_ ## reg ## _raw_reg_value (0 pp_for_each_i (expand_set_all, reg, __VA_ARGS__ ))
#define reset_reg(reg, ...) reset_reg_1(reg, __VA_ARGS__)

#define reset_reg_raw_1(reg, val) set_ ## reg ## _raw_reg_value (val)
#define reset_reg_raw(reg, ...) reset_reg_raw_1(reg, __VA_ARGS__)

#define expand_set_multi_get_mask___(reg, field, val) | reg ## _ ## field ## _mask
#define expand_set_multi_get_mask_(...) expand_set_multi_get_mask___ (__VA_ARGS__)
#define expand_set_multi_get_mask__(...) __VA_ARGS__
#define expand_set_multi_get_mask(reg, x) expand_set_multi_get_mask_ (reg, expand_set_multi_get_mask__  x)


#define set_reg_1(reg, ...) rmw_ ## reg ( \
  reg ## _all_bits & ~(0u pp_for_each_i (expand_set_multi_get_mask, reg, __VA_ARGS__)), \
  (0u pp_for_each_i (expand_set_all, reg, __VA_ARGS__)))

#define set_reg(reg, ...) set_reg_1 (reg, __VA_ARGS__)

#define get_reg_n__1(reg,field,...) get_ ## reg ## _ ## field ()
#define get_reg_n__0(reg,...) get_ ## reg ## _raw_reg_value ()
#define get_reg_n_(n, ...) get_reg_n__ ## n (__VA_ARGS__)
#define get_reg_n(n, ...) get_reg_n_(n, __VA_ARGS__)
#define get_reg(reg, ...) get_reg_n ( pp_args_size(__VA_ARGS__), reg, __VA_ARGS__)

#define make_reg_value(reg, ...) (0 pp_for_each_i (expand_set_all, reg, __VA_ARGS__ ))


#define get_reg_value_n__1(val, reg,field,...) get_ ## reg ## _value_ ## field (val)
#define get_reg_value_n__0(val, reg,...) (val)
#define get_reg_value_n_(val, n, ...) get_reg_value_n__ ## n (val, __VA_ARGS__)
#define get_reg_value_n(val, n, ...) get_reg_value_n_(val, n, __VA_ARGS__)
#define get_reg_value(val, reg, ...) get_reg_value_n (val, pp_args_size(__VA_ARGS__), reg, __VA_ARGS__)


#define set_reg_value(val, reg, ...) \
  do { val = (val & ~(0u pp_for_each_i (expand_set_multi_get_mask, reg, __VA_ARGS__))) \
             | (0u pp_for_each_i (expand_set_all, reg, __VA_ARGS__)); } while (0)


// FIXME: problem of nested expansion of pp_for_each / pp_for_each_i
// for some weird reason, the nested pp_for_each_i that comes out of 'reset_reg'
// it seems to be related to the way arguments are expanded and concatenated.
// if a pp_for_each_i is used with all the macros named differently (e.g. prefixed with '_')
// the nested expansion works as expected.

// because of that, 'combined_reg' is not useful at the moment.
// perhaps the better way to solve this is with an external code generator tool.

#define expand_set_combined_subreg_3(f,g,reg_name, field_name, ...) pp_concat (f, g) (reg_name, (field_name, __VA_ARGS__))

#define expand_set_combined_subreg_2(val_type, val, high_bit, low_bit, set_or_reset_cmd, reg_name, field_name)\
  expand_set_combined_subreg_3 (set_or_reset_cmd, _reg, reg_name, field_name, (val >> low_bit) & bit_count_to_mask(val_type, high_bit - low_bit + 1));

#define expand_set_combined_subreg_1(...) expand_set_combined_subreg_2(__VA_ARGS__)
#define expand_set_combined_subreg__(...) __VA_ARGS__
#define expand_set_combined_subreg(val_type, subreg) expand_set_combined_subreg_1(expand_set_combined_subreg__ val_type, expand_set_combined_subreg__ subreg)

#define combined_reg(reg_name, raw_reg_type, field, ...) \
inline void set_ ## reg_name ## _raw_reg_value (raw_reg_type val) \
{ \
  _pp_for_each_i (expand_set_combined_subreg, (raw_reg_type, val), __VA_ARGS__) \
}


#endif // includeguard_reg_def_h_includeguard
