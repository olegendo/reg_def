
#ifndef includeguard_dev_alc5616_reg_h_includeguard
#define includeguard_dev_alc5616_reg_h_includeguard

// 16-bit registers read/write the high-byte first and low-byte second
// if possible do it as one transaction for more efficient access.

#ifndef alc5616_write_reg16
  #error alc5616_write_reg16 not defined
#endif

#ifndef alc5616_read_reg16
  #error alc5616_read_reg16 not defined
#endif


#include "reg_def.h"

#define alc5616_reg16(reg_name, addr, ...)\
enum { reg_name##_ADDR = addr }; \
inline void set_ ## reg_name ## _raw_reg_value (uint16_t v) { alc5616_write_reg16 (addr, v); } \
inline uint16_t get_ ## reg_name ## _raw_reg_value (void) { return alc5616_read_reg16 (addr); } \
inline void rmw_ ## reg_name (uint16_t a, uint16_t o) { alc5616_write_reg16 (addr, (alc5616_read_reg16 (addr) & a) | o); } \
expand_define_reg(reg_name, uint16_t, __VA_ARGS__)


alc5616_reg16 (ALC5616_MX6A_PR_INDEX, 0x6A)
alc5616_reg16 (ALC5616_MX6C_PR_DATA, 0x6C)

#define alc5616_pr_reg16(reg_name, addr, ...)\
enum { reg_name##_ADDR = addr }; \
inline void set_ ## reg_name ## _raw_reg_value (uint16_t v)\
{\
  reset_reg (ALC5616_MX6A_PR_INDEX, (raw_reg_value, addr)); \
  reset_reg (ALC5616_MX6C_PR_DATA, (raw_reg_value, v)); \
}\
inline uint16_t get_ ## reg_name ## _raw_reg_value (void)\
{\
  reset_reg (ALC5616_MX6A_PR_INDEX, (raw_reg_value, addr)); \
  return get_reg (ALC5616_MX6C_PR_DATA); \
}\
inline void rmw_ ## reg_name (uint16_t a, uint16_t o)\
{\
  reset_reg (ALC5616_MX6A_PR_INDEX, (raw_reg_value, addr)); \
  uint16_t v = get_reg (ALC5616_MX6C_PR_DATA); \
\
  v = (v & a) | o; \
\
  reset_reg (ALC5616_MX6A_PR_INDEX, (raw_reg_value, addr)); \
  reset_reg (ALC5616_MX6C_PR_DATA, (raw_reg_value, v)); \
}\
expand_define_reg(reg_name, uint16_t, __VA_ARGS__)



// seems writing 0x0020 and 0x0000 does a device soft-reset ...
alc5616_reg16 (ALC5616_MX00_SW_RESET, 0x00)

alc5616_reg16 (ALC5616_MX02_HPOUT, 0x02,
  reg_bits (mu_hpo_l,      15, 15, bool),
  reg_bits (mu_hpovoll_in, 14, 14, bool),
  reg_bits (vol_hpol,      13,  8, uint8_t),
  reg_bits (mu_hpo_r,       7,  7, bool),
  reg_bits (mu_hpovolr_in,  6,  6, bool),
  reg_bits (vol_hpor,       5,  0, uint8_t)
)

alc5616_reg16 (ALC5616_MX03_LINEOUT_1, 0x03,
  reg_bits (mu_lout_l,      15, 15, bool),
  reg_bits (mu_outvoll_in,  14, 14, bool),
  reg_bits (vol_outl,       13,  8, uint8_t),
  reg_bits (mu_lout_r,       7,  7, bool),
  reg_bits (mu_outvolr_in,   6,  6, bool),
  reg_bits (vol_outr,        5,  0, uint8_t)
)

alc5616_reg16 (ALC5616_MX05_LINEOUT_2, 0x05,
  reg_bits (en_dfo, 15, 15, bool)
)

enum in_boost_val_t
{
  in_boost_bypass = 0b0000,
  in_boost_20_db  = 0b0001,
  in_boost_24_db  = 0b0010,
  in_boost_30_db  = 0b0011,
  in_boost_35_db  = 0b0100,
  in_boost_40_db  = 0b0101,
  in_boost_44_db  = 0b0110,
  in_boost_50_db  = 0b0111,
  in_boost_52_db  = 0b1000
};

alc5616_reg16 (ALC5616_MX0D_MICIN, 0x0D,
  reg_bits (sel_bst1,   15, 12, enum in_boost_val_t),
  reg_bits (sel_bst2,   11,  8, enum in_boost_val_t),
  reg_bits (en_in1_df,   7,  7, bool),
  reg_bits (en_in2_df,   6,  6, bool)
)

alc5616_reg16 (ALC5616_MX0F_LINEIN, 0x0F,
  reg_bits (vol_inl,    12,  8, uint8_t),
  reg_bits (vol_inr,     4,  0, uint8_t)
)

alc5616_reg16 (ALC5616_MX19_DACL1_R1, 0x19,
  reg_bits (vol_dac1_l, 15, 8, uint8_t),
  reg_bits (vol_dac1_r,  7, 0, uint8_t)
)

alc5616_reg16 (ALC5616_MX1C_ADCL_R1, 0x1C,
  reg_bits (mu_adc_vol_l, 15, 15, bool),
  reg_bits (vol_adc1_l,   14,  8, uint8_t),
  reg_bits (mu_adc_vol_r,  7,  7, bool),
  reg_bits (vol_adc1_r,    6,  0, uint8_t)
)

alc5616_reg16 (ALC5616_MX1E_ADCL_R2, 0x1E,
  reg_bits (ad_boost_gain_l, 15, 14, uint8_t),
  reg_bits (ad_boost_gain_r, 13, 12, uint8_t)
)

alc5616_reg16 (ALC5616_MX27_ADC_1, 0x27,
  reg_bits (mu_stereo1_adcl1, 14, 14, bool),
  reg_bits (reserved_0,       13,  7, uint8_t),  // default 0x70
  reg_bits (mu_stereo1_adcr1,  6,  6, bool),
  reg_bits (reserved_1,        5,  0, uint8_t)   // default 0x20
)

alc5616_reg16 (ALC5616_MX29_ADC_2, 0x29,
  reg_bits (mu_stereo1_adc_mixer_l, 15, 15, bool),
  reg_bits (mu_dacl_l,              14, 14, bool),
  reg_bits (mu_stereo1_adc_mixer_r,  7,  7, bool),
  reg_bits (mu_dacl_r,               6,  6, bool)
)

alc5616_reg16 (ALC5616_MX2A_DAC_1, 0x2A,
  reg_bits (mu_stereo_dacl1_mixl,   14, 14, bool),
  reg_bits (gain_dacl1_to_stereo_l, 13, 13, bool),
  reg_bits (reserved_0,             12, 10, uint8_t), // default 0x04
  reg_bits (mu_stereo_dacr1_mixl,    9,  9, bool),
  reg_bits (gain_dacr1_to_stereo_l,  8,  8, bool),
  reg_bits (mu_stereo_dacr1_mixr,    6,  6, bool),
  reg_bits (gain_dacr1_to_stereo_r,  5,  5, bool),
  reg_bits (reserved_1,              4,  2, uint8_t), // default 0x04
  reg_bits (mu_stereo_dacl1_mixr,    1,  1, bool),
  reg_bits (gain_dacl1_to_stereo_r,  0,  0, bool)
)

alc5616_reg16 (ALC5616_MX3B_RECMIXL_1, 0x3B,
  reg_bits (gain_inl_recmixl,  12, 10, uint8_t),
  reg_bits (gain_bst2_recmixl,  3,  1, uint8_t)
)

alc5616_reg16 (ALC5616_MX3C_RECMIXL_2, 0x3C,
  reg_bits (gain_bst1_recmixl, 15, 13, uint8_t),
  reg_bits (reserved_0,        12,  6, uint8_t),  // default 0x01
  reg_bits (mu_inl_rexmixl,     5,  5, bool),
  reg_bits (reserved_1,         4,  3, uint8_t),  // default 0x01
  reg_bits (mu_bst2_recmixl,    2,  2, bool),
  reg_bits (mu_bst1_recmixl,    1,  1, bool),
  reg_bits (reserved_2,         0,  0, uint8_t)   // default: 0x01
)

alc5616_reg16 (ALC5616_MX3D_RECMIXR_1, 0x3D,
  reg_bits (reserved_1, 15, 13, uint8_t),
  reg_bits (gain_inr_recmixr, 12, 10, uint8_t),
  reg_bits (reserved_2, 9, 4, uint8_t),
  reg_bits (gain_bst2_recmixr, 3, 1, uint8_t),
  reg_bits (reserved_3, 0, 0, uint8_t)
)

alc5616_reg16 (ALC5616_MX3E_RECMIXR_2, 0x3E,
  reg_bits (gain_bst1_recmixr, 15, 13, uint8_t),
  reg_bits (reserved_0,        12,  6, uint8_t),  // default 0x01
  reg_bits (mu_inr_rexmixr,     5,  5, bool),
  reg_bits (reserved_1,         4,  3, uint8_t),  // default 0x01
  reg_bits (mu_bst2_recmixr,    2,  2, bool),
  reg_bits (mu_bst1_recmixr,    1,  1, bool),
  reg_bits (reserved_2,         0,  0, uint8_t)   // default: 0x01
)

alc5616_reg16 (ALC5616_MX45_HPOMIX, 0x45,
  reg_bits (mu_dac1_hpomix,   14, 14, bool),
  reg_bits (mu_hpovol_hpomix, 13, 13, bool),
  reg_bits (gain_hpomix,      12, 12, bool)
)

alc5616_reg16 (ALC5616_MX4D_OUTMIXL_1, 0x4D,
  reg_bits (gain_bst2_outmixl,   12, 10, uint8_t),
  reg_bits (gain_bst1_outmixl,    9,  7, uint8_t),
  reg_bits (gain_inl_outmixl,     6,  4, uint8_t),
  reg_bits (gain_recmixl_outmixl, 3,  1, uint8_t)
)

alc5616_reg16 (ALC5616_MX4E_OUTMIXL_2, 0x4E,
  reg_bits (gain_dacl1_outmixl, 9, 7, uint8_t)
)

alc5616_reg16 (ALC5616_MX4F_OUTMIXL_3, 0x4F,
  reg_bits (reserved_0,        15, 7, uint16_t),  // default: 0x04
  reg_bits (mu_bst2_outmixl,    6, 6, bool),
  reg_bits (mu_bst1_outmixl,    5, 5, bool),
  reg_bits (mu_inl_outmixl,     4, 4, bool),
  reg_bits (mu_recmixl_outmixl, 3, 3, bool),
  reg_bits (reserved_1,         2, 1, uint8_t),  // default: 0x00
  reg_bits (mu_dacl1_outmixl,   0, 0, bool)
)

alc5616_reg16 (ALC5616_MX50_OUTMIXR_1, 0x50,
  reg_bits (gain_bst2_outmixr,   12, 10, uint8_t),
  reg_bits (gain_bst1_outmixr,    9,  7, uint8_t),
  reg_bits (gain_inr_outmixr,     6,  4, uint8_t),
  reg_bits (gain_recmixr_outmixr, 3,  1, uint8_t)
)

alc5616_reg16 (ALC5616_MX51_OUTMIXR_2, 0x51,
  reg_bits (gain_dacr1_outmixr,  9, 7, uint8_t)
)

alc5616_reg16 (ALC5616_MX52_OUTMIXR_3, 0x52,
  reg_bits (mu_bst2_outmixr,    6, 6, bool),
  reg_bits (mu_bst1_outmixr,    5, 5, bool),
  reg_bits (mu_inr_outmixr,     4, 4, bool),
  reg_bits (mu_recmixr_outmixr, 3, 3, bool),
  reg_bits (mu_dacr1_outmixr,   0, 0, bool)
)

alc5616_reg16 (ALC5616_MX53_LOUTMIX, 0x53,
  reg_bits (mu_dacl1_lout,   15, 15, bool),
  reg_bits (mu_dacr1_lout,   14, 14, bool),
  reg_bits (mu_outvoll_lout, 13, 13, bool),
  reg_bits (mu_outvolr_lout, 12, 12, bool),
  reg_bits (gain_lout,       11, 11, bool)
)

alc5616_reg16 (ALC5616_MX61_MANAGEMENT_1, 0x61,
  reg_bits (en_i2s1,     15, 15, bool),
  reg_bits (reserved_1,  14, 13, uint8_t),
  reg_bits (pow_dac_l_1, 12, 12, bool),
  reg_bits (pow_dac_r_1, 11, 11, bool),
  reg_bits (reserved_2,  10,  3, uint8_t),
  reg_bits (pow_adc_l,    2,  2, bool),
  reg_bits (pow_adc_r,    1,  1, bool)
)

alc5616_reg16 (ALC5616_MX62_MANAGEMENT_2, 0x62,
  reg_bits (pow_adc_stereo1_filter, 15, 15, bool),
  reg_bits (reserved_1,             14, 12, uint8_t),
  reg_bits (pow_dac_stereo1_filter, 11, 11, bool),
  reg_bits (reserved_2,             10,  0, uint16_t)
)

enum alc5616_ldo_out_sel_t
{
  alc5616_ldo_out_1_1V = 0b00,
  alc5616_ldo_out_1_2V = 0b01,
  alc5616_ldo_out_1_3V = 0b10,
  alc5616_ldo_out_1_4V = 0b11
};

alc5616_reg16 (ALC5616_MX63_MANAGEMENT_3, 0x63,
  reg_bits (pow_vref1,     15, 15, bool),
  reg_bits (en_fastb1,     14, 14, bool),
  reg_bits (pow_main_bias, 13, 13, bool),
  reg_bits (pow_lout,      12, 12, bool),
  reg_bits (pow_bg_bias,   11, 11, bool),
  reg_bits (reserved_1,    10,  8, uint8_t),
  reg_bits (en_l_hp,        7,  7, bool),
  reg_bits (en_r_hp,        6,  6, bool),
  reg_bits (en_amp_hp,      5,  5, bool),
  reg_bits (pow_vref2,      4,  4, bool),
  reg_bits (en_fastb2,      3,  3, bool),
  reg_bits (reserved_2,     2,  2, bool),
  reg_bits (ldo_dvo,        1,  0, enum alc5616_ldo_out_sel_t)
)

alc5616_reg16 (ALC5616_MX64_MANAGEMENT_4, 0x64,
  reg_bits (pow_bst1,     15, 15, bool),
  reg_bits (pow_bst2,     14, 14, bool),
  reg_bits (pow_micbias1, 11, 11, bool),
  reg_bits (pow_pll,       9,  9, bool),
  reg_bits (pow_bst1_op2,  5,  5, bool),
  reg_bits (pow_bst2_op2,  4,  4, bool),
  reg_bits (pow_jd_m,      2,  2, bool),
  reg_bits (pow_jd2,       1,  1, bool)
)

alc5616_reg16 (ALC5616_MX65_MANAGEMENT_5, 0x65,
  reg_bits (pow_outmixl,  15, 15, bool),
  reg_bits (pow_outmixr,  14, 14, bool),
  reg_bits (pow_recmixl,  11, 11, bool),
  reg_bits (pow_recmixr,  10, 10, bool)
)

alc5616_reg16 (ALC5616_MX66_MANAGEMENT_6, 0x66,
  reg_bits (pow_outvoll, 13, 13, bool),
  reg_bits (pow_outvolr, 12, 12, bool),
  reg_bits (pow_hpovoll, 11, 11, bool),
  reg_bits (pow_hpovolr, 10, 10, bool),
  reg_bits (pow_inlvol,   9,  9, bool),
  reg_bits (pow_inrvol,   8,  8, bool),
  reg_bits (reserved,     7,  0, uint8_t)
)

enum alc5616_i2s_mode_t
{
  alc5616_i2s_master = 0,
  alc5616_i2s_slave = 1
};

enum alc5616_i2s_compression_t
{
  alc5616_no_compression = 0b00,
  alc5616_u_law = 0b01,
  alc5616_a_law = 0b10
};

enum alc5616_i2s_data_length_t
{
  alc5616_i2s_16_bit = 0b00,
  alc5616_i2s_20_bit = 0b01,
  alc5616_i2s_24_bit = 0b10,
  alc5616_i2s_8_bit  = 0b11
};

enum alc5616_i2s_data_format_t
{
  alc5616_i2s_i2s = 0b00,
  alc5616_i2s_left_justified = 0b01,
  alc5616_i2s_pcm_mode_a = 0b10,
  alc5616_i2s_pcm_mode_b = 0b11
};

alc5616_reg16 (ALC5616_MX70_I2S1_PORT_CTRL, 0x70,
  reg_bits (sel_i2s1_ms,      15, 15, enum alc5616_i2s_mode_t),
  reg_bits (en_i2s1_out_comp, 11, 10, enum alc5616_i2s_compression_t),
  reg_bits (en_i2s1_in_comp,   9,  8, enum alc5616_i2s_compression_t),
  reg_bits (inv_i2s1_bclk,     7,  7, bool),
  reg_bits (sel_i2s1_len,      3,  2, enum alc5616_i2s_data_length_t),
  reg_bits (sel_i2s1_format,   1,  0, enum alc5616_i2s_data_format_t)
)


enum alc5616_oversample_rate_t
{
  alc5616_oversample_128fs   = 0b00,
  alc5616_oversample_64fs    = 0b01,
  alc5616_oversample_32fs    = 0b10,
  alc5616_oversample_128fs_3 = 0b11,
};

alc5616_reg16 (ALC5616_MX73_ADC_DAC_CLOCK_1, 0x73,
  reg_bits (reserved,         15, 15, bool),
  reg_bits (sel_i2s_pre_div1, 14, 12, uint8_t),
  reg_bits (sel_dac_osr,       3,  2, enum alc5616_oversample_rate_t),
  reg_bits (sel_adc_osr,       1,  0, enum alc5616_oversample_rate_t)
)

alc5616_reg16 (ALC5616_MX74_ADC_DAC_CLOCK_2, 0x74,
  reg_bits (dahpf_en, 11, 11, bool),
  reg_bits (adhpf_en, 10, 10, bool)
)

// undocumented
alc5616_reg16 (ALC5616_MX77, 0x77)

enum alc5616_sysclk_t
{
  alc5616_sysclk_mclk = 0b00,
  alc5616_sysclk_pll  = 0b01
};

enum acl5616_pll_src_t
{
  alc5616_pll_src_mclk  = 0b00,
  alc5616_pll_src_bclk1 = 0b01
};

alc5616_reg16 (ALC5616_MX80_GLOBAL_CLOCK, 0x80,
  reg_bits (sel_sysclk1,    15, 14, enum alc5616_sysclk_t),
  reg_bits (sel_pll_sour,   13, 12, enum acl5616_pll_src_t),
  reg_bits (sel_pll_pre_div, 3,  3, uint8_t)
)

alc5616_reg16 (ALC5616_MX81_PLL_1, 0x81,
  reg_bits (pll_n_code, 15, 7, uint16_t),
  reg_bits (pll_k_code,  4, 0, uint8_t)
)

alc5616_reg16 (ALC5616_MX82_PLL_2, 0x82,
  reg_bits (pll_m_code,   15, 12, uint8_t),
  reg_bits (pll_m_bypass, 11, 11, bool)
)

// undocumented
alc5616_reg16 (ALC5616_MX83, 0x83)

// undocumented
alc5616_reg16 (ALC5616_MX84, 0x84)


alc5616_reg16 (ALC5616_MX8E_HP_AMP_1, 0x8E,
  reg_bits (smttrig_hp,  15, 15, bool),
  reg_bits (en_smt_l_hp,   9, 9, bool),
  reg_bits (en_smt_r_hp,   8, 8, bool),
  reg_bits (pdn_hp,        7, 7, bool),
  reg_bits (softgen_rstn,  6, 6, bool),
  reg_bits (softgen_rstp,  5, 5, bool),
  reg_bits (en_out_hp,     4, 4, bool),
  reg_bits (pow_pump_hp,   3, 3, bool),
  reg_bits (en_softgen_hp, 2, 2, bool),
  reg_bits (pow_capless,   0, 0, bool)
)

enum alc5616_hp_depop_t
{
  alc5616_hp_depop_mode1 = 0,
  alc5616_hp_depop_mode2 = 1
};

alc5616_reg16 (ALC5616_MX8F_HP_AMP_2, 0x8F,
  reg_bits (depop_mode_hp,  13, 13, enum alc5616_hp_depop_t),
  reg_bits (reserved,       12,  7, uint8_t), // default: 0x22
  reg_bits (en_depop_mode1,  6,  6, bool)
)

enum alc5616_micbias_t
{
  alc5616_micbias_0_9_micvdd = 0,
  alc5616_micbias_0_75_micvdd = 1
};

enum alc5616_micbias_short_detect_t
{
  alc5616_micbias_short_600uA  = 0b00,
  alc5616_micbias_short_1500uA = 0b01,
  alc5616_micbias_short_2000uA = 0b10
};

// undocumented
alc5616_reg16 (ALC5616_MX91, 0x91)

alc5616_reg16 (ALC5616_MX93_MICBIAS, 0x93,
  reg_bits (sel_micbias1,     15, 15, enum alc5616_micbias_t),
  reg_bits (pow_mic1_ovcd,    11, 11, bool),
  reg_bits (mic1_ovcd_th_sel, 10,  9, enum alc5616_micbias_short_detect_t)
)

alc5616_reg16 (ALC5616_MX94_JD, 0x94,
  reg_bits (jad_cmp,      13, 13, bool),
  reg_bits (pullup_jd,    11, 11, bool),
  reg_bits (jd_m_cmp,      6, 4, uint8_t),
  reg_bits (pullup_jd_m,   2, 2, bool),     // it seems the hw doc is wrong and up/down is swapped
  reg_bits (pulldown_jd_m, 3, 3, bool)
)

enum alc5616_eq_path_t
{
  alc5616_eq_dac_path = 0,
  alc5616_eq_adc_path = 1
};

alc5616_reg16 (ALC5616_MXB0_EQ_1, 0xB0,
  reg_bits (eq_sour,        15, 15, enum alc5616_eq_path_t),
  reg_bits (eq_para_update, 14, 14, bool),
  reg_bits (sta_hpf2,       6, 6, bool),
  reg_bits (sta_hpf1,       5, 5, bool),
  reg_bits (sta_bpf4,       4, 4, bool),
  reg_bits (sta_bpf3,       3, 3, bool),
  reg_bits (sta_bpf2,       2, 2, bool),
  reg_bits (sta_bpf1,       1, 1, bool),
  reg_bits (sta_lpf,        0, 0, bool)
)

alc5616_reg16 (ALC5616_MXB1_EQ_2, 0xB1,
  reg_bits (reg_typ_hpf_en, 8, 8, bool),
  reg_bits (reg_typ_lpf_en, 7, 7, bool),
  reg_bits (en_hpf2,        6, 6, bool),
  reg_bits (en_hpf1,        5, 5, bool),
  reg_bits (en_bpf4,        4, 4, bool),
  reg_bits (en_bpf3,        3, 3, bool),
  reg_bits (en_bpf2,        2, 2, bool),
  reg_bits (en_bpf1,        1, 1, bool),
  reg_bits (en_lpf,         0, 0, bool)
)

alc5616_pr_reg16 (ALC5616_PRA0_EQ_LPF_A1, 0xA0)
alc5616_pr_reg16 (ALC5616_PRA1_EQ_LPF_H0, 0xA1)
alc5616_pr_reg16 (ALC5616_PRA2_EQ_BPF1_A1, 0xA2)
alc5616_pr_reg16 (ALC5616_PRA3_EQ_BPF1_A2, 0xA3)
alc5616_pr_reg16 (ALC5616_PRA4_EQ_BPF1_H0, 0xA4)
alc5616_pr_reg16 (ALC5616_PRA5_EQ_BPF2_A1, 0xA5)
alc5616_pr_reg16 (ALC5616_PRA6_EQ_BPF2_A2, 0xA6)
alc5616_pr_reg16 (ALC5616_PRA7_EQ_BPF2_H0, 0xA7)
alc5616_pr_reg16 (ALC5616_PRA8_EQ_BPF3_A1, 0xA8)
alc5616_pr_reg16 (ALC5616_PRA9_EQ_BPF3_A2, 0xA9)
alc5616_pr_reg16 (ALC5616_PRAA_EQ_BPF3_H0, 0xAA)
alc5616_pr_reg16 (ALC5616_PRAB_EQ_BPF4_A1, 0xAB)
alc5616_pr_reg16 (ALC5616_PRAC_EQ_BPF4_A2, 0xAC)
alc5616_pr_reg16 (ALC5616_PRAD_EQ_BPF4_H0, 0xAD)
alc5616_pr_reg16 (ALC5616_PRAE_EQ_HPF1_A1, 0xAE)
alc5616_pr_reg16 (ALC5616_PRAF_EQ_HPDF_H0, 0xAF)
alc5616_pr_reg16 (ALC5616_PRB0_EQ_HPF2_A1, 0xB0)
alc5616_pr_reg16 (ALC5616_PRB1_EQ_HPF2_A2, 0xB1)
alc5616_pr_reg16 (ALC5616_PRB2_EQ_HPF2_H0, 0xB2)
alc5616_pr_reg16 (ALC5616_PRB3_EQ_PRE_VOL, 0xB3)
alc5616_pr_reg16 (ALC5616_PRB4_EQ_POST_VOL, 0xB4)


enum alc5616_drc_agc_t
{
  alc5616_disable_drc_agc = 0b00,
  alc5616_enable_drc_to_dac_path = 0b01,
  alc5616_enable_agc_to_adc_path = 0b11
};

enum alc5616_drc_agc_rate_t
{
  alc5616_drc_agc_48khz = 0b001,
  alc5616_drc_agc_96khz = 0b010,
  alc5616_drc_agc_192khz = 0b011,
  alc5616_drc_agc_44_1khz = 0b101,
  alc5616_drc_agc_88_2khz = 0b110,
  alc5616_drc_agc_176_4khz = 0b111
};

alc5616_reg16 (ALC5616_MXB4_DRC_AGC_1, 0xB4,
  reg_bits (sel_drc_agc,          15, 14, enum alc5616_drc_agc_t),
  reg_bits (update_drc_agc_param, 13, 13, bool),
  reg_bits (sel_drc_agc_atk,      12,  8, uint8_t),
  reg_bits (drc_agc_rate_sel,      7,  5, enum alc5616_drc_agc_rate_t),
  reg_bits (sel_rc_rate,           4,  0, uint8_t)
)

enum alc5616_drc_agc_comp_ratio_t
{
  alc5616_comp_1_1 = 0b00,
  alc5616_comp_1_2 = 0b01,
  alc5616_comp_1_4 = 0b10,
  alc5616_comp_1_8 = 0b11
};

alc5616_reg16 (ALC5616_MXB5_DRC_AGC_2, 0xB5,
  reg_bits (sel_drc_agc_post_bst, 13, 8, uint8_t),
  reg_bits (en_drc_agc_compress,   7, 7, bool),
  reg_bits (sel_ratio,             6, 5, enum alc5616_drc_agc_comp_ratio_t),
  reg_bits (sel_drc_agc_pre_bst,   4, 0, uint8_t)
)

alc5616_reg16 (ALC5616_MXB6_DRC_AGC_3, 0xB6,
  reg_bits (noise_gate_boost,           15, 12, uint8_t),
  reg_bits (sel_drc_agc_thmax,          11,  7, uint8_t),
  reg_bits (en_drc_agc_noise_gate,       6,  6, bool),
  reg_bits (en_drc_agc_noise_gate_hold,  5,  5, bool),
  reg_bits (sel_drc_agc_noise_th,        4,  0, uint8_t)
)

enum alc5616_jd_sel_t
{
  alc5616_jd_off = 0b000,
  alc5616_jd_gpio = 0b001
};

alc5616_reg16 (ALC5616_MXBB_JD_1, 0xBB,
  reg_bits (sel_gpio_jd,         15, 13, enum alc5616_jd_sel_t),
  reg_bits (en_jd_hpo,           11, 11, bool),
  reg_bits (polarity_jd_tri_hpo, 10, 10, bool),
  reg_bits (en_jd_lout,           3,  3, bool),
  reg_bits (polarity_jd_tri_lout, 2,  2, bool)
)

enum alc5616_jd_trigger_t
{
  alc5616_from_sta_gpio_jd = 0b000,
  alc5616_from_sta_jd1_1   = 0b001,
  alc5616_from_sta_jd1_2   = 0b010,
  alc5616_from_sta_jd2     = 0b011
};

alc5616_reg16 (ALC5616_MXBC_JD_2, 0xBC,
  reg_bits (sel_jd_trigger, 11, 9, enum alc5616_jd_trigger_t)
)

alc5616_reg16 (ALC5616_MXBD_IRQ_1, 0xBD,
  reg_bits (en_irq_gpio_jd,    15, 15, bool),
  reg_bits (en_gpio_jd_sticky, 13, 13, bool),
  reg_bits (inv_gpio_jd,       11, 11, bool),
  reg_bits (en_irq_jd1_1,       9,  9, bool),
  reg_bits (en_jd1_1_sticky,    8,  8, bool),
  reg_bits (inv_jd1_1,          7,  7, bool),
  reg_bits (en_irq_jd1_2,       6,  6, bool),
  reg_bits (en_jd1_2_sticky,    5,  5, bool),
  reg_bits (inv_jd1_2,          4,  4, bool),
  reg_bits (en_irq_jd2,         3,  3, bool),
  reg_bits (en_jd2_sticky,      2,  2, bool),
  reg_bits (inv_jd2,            1,  1, bool)
)

alc5616_reg16 (ALC5616_MXBE_IRQ_2, 0xBE,
  reg_bits (en_irq_micbias1_ovcd,    15, 15, bool),
  reg_bits (en_micbias1_ovcd_sticky, 11, 11, bool),
  reg_bits (inv_micbias1_ovcd,        7,  7, bool),
  reg_bits (ovc_micbias1,             3,  3, bool)
)

alc5616_reg16 (ALC5616_MXBF_STATUS, 0xBF,
  reg_bits (sta_jd2,    14, 14, bool),
  reg_bits (sta_jd1_2,  13, 13, bool),
  reg_bits (sta_jd1_1,  12, 12, bool),
  reg_bits (sta_gpio1,   8,  8, bool),
  reg_bits (sta_gpio_jd, 4,  4, bool)
)

enum alc5616_gpio_sel_t
{
  alc5616_gpio_sel_gpio1 = 0,
  alc5616_gpio_sel_irq = 1
};

alc5616_reg16 (ALC5616_MXC0_GPIO_1, 0xC0,
  reg_bits (sel_gpio1_type, 15, 15, enum alc5616_gpio_sel_t)
)

enum alc5616_gpio_dir_t
{
  alc5616_gpio_input = 0,
  alc5616_gpio_output = 1
};

alc5616_reg16 (ALC5616_MXC1_GPIO_2, 0xC1,
  reg_bits (sel_gpio1,       2, 2, enum alc5616_gpio_dir_t),
  reg_bits (sel_gpio1_logic, 1, 1, bool),
  reg_bits (inv_gpio1,       0, 0, bool)
)

enum alc5616_wind_sample_rate_t
{
  alc5616_wind_sample_8K_12K_16K = 0b000,
  alc5616_wind_sample_24K_32K = 0b001,
  alc5616_wind_sample_48K_44K = 0b010,
  alc5616_wind_sample_96K_88K = 0b011,
  alc5616_wind_sample_192K_176K = 0b100
};

alc5616_reg16 (ALC5616_MXD3_WIND_CONTROL_1, 0xD3,
  reg_bits (adj_hpf_2nd_en, 15, 15, bool),
  reg_bits (adj_hpf_coef_l_sel, 14, 12, enum alc5616_wind_sample_rate_t),
  reg_bits (adj_hpf_coef_r_sel, 10,  8, enum alc5616_wind_sample_rate_t)
)

alc5616_reg16 (ALC5616_MXD4_WIND_CONTROL_1, 0xD4,
  reg_bits (adj_hpf_coef_l_num, 13, 8, uint8_t),
  reg_bits (adj_hpf_coef_r_num,  5, 0, uint8_t)
)

alc5616_reg16 (ALC5616_MXD9_SVOL_ZCD, 0xD9,
  reg_bits (en_softvol,     15, 15, bool),
  reg_bits (en_o_svol,      13, 13, bool),
  reg_bits (en_hpo_svol,    12, 12, bool),
  reg_bits (en_zcd_digital, 11, 11, bool),
  reg_bits (pow_zcd,        10, 10, bool),
  reg_bits (en_zcd_outmixr,  7,  7, bool),
  reg_bits (en_zcd_outmixl,  6,  6, bool),
  reg_bits (en_zcd_recmixr,  5,  5, bool),
  reg_bits (en_zcd_recmixl,  4,  4, bool),
  reg_bits (sel_svol,        3,  0, uint8_t)
)

alc5616_reg16 (ALC5616_MXFA_GENERAL_CONTROL_1, 0xFA,
  reg_bits (reserved_1, 15, 4, uint16_t),
  reg_bits (en_detect_clk_sys, 3, 3, bool),
  reg_bits (reserved_2, 2, 1, uint8_t),
  reg_bits (digital_gate_ctrl, 0, 0, bool)
)

alc5616_pr_reg16 (ALC5616_PR3D_ADC_DAC_RESET, 0x3D,
  reg_bits (reserved_1,   15, 13, uint8_t),
  reg_bits (en_ckgen_adc, 12, 12, bool),
  reg_bits (reserved_2,   11, 11, uint8_t),
  reg_bits (ckxen_dac,    10, 10, bool),
  reg_bits (en_ckgen_dac,  9,  9, bool)
)

alc5616_reg16 (ALC5616_MXFE_VENDOR_ID, 0xFE) // 0x10EC





#endif // includeguard_dev_alc5616_reg_h_includeguard
