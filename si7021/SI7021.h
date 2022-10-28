#include "main.h"

typedef enum Si7021_commands
{
  Humi_HM        = 0xE5, // Measure Relative Humidity, Hold Master Mode
  Humi_NHM       = 0xF5, // Measure Relative Humidity, No Hold Master Mode
  Temp_HM        = 0xE3, // Measure Temperature, Hold Master Mode
  Temp_NHM       = 0xF3, // Measure Temperature, No Hold Master Mode
  Temp_AH        = 0xE0, // Read Temperature Value from Previous RH Measurement
  Si7021_Reset   = 0xFE, // Reset
  W_RHT_U_reg    = 0xE6, // Write RH/T User Register 1
  R_RHT_U_reg    = 0xE7, // Read RH/T User Register 1
  W_Heater_C_reg = 0x51, // Write Heater Control Register
  R_Heater_C_reg = 0x11, // Read Heater Control Register
  R_ID_Byte11    = 0xFA, // Read Electronic ID 1st Byte, first part
  R_ID_Byte12    = 0x0F, // Read Electronic ID 1st Byte, second part
  R_ID_Byte21    = 0xFC, // Read Electronic ID 2nd Byte, first part
  R_ID_Byte22    = 0xC9, // Read Electronic ID 2nd Byte, second part
  R_Firm_rev1    = 0x84, // Read Firmware Revision, first part
  R_Firm_rev2    = 0xB8  // Read Firmware Revision, second part
}Si7021_commands_t;

typedef enum Si7021_measurement_type
{
  Humidity,
  Temperature
}Si7021_measurement_type_t;



typedef enum Si7021_registers
{
  User_Register_1,
  Heater_Control_Register
}Si7021_registers_t;

typedef enum Si7021_resolution
{
  H12_T14 = 0x00,
  H8_T12  = 0x01,
  H10_T13 = 0x80,
  H11_T11 = 0x81
}Si7021_resolution_t;

int8_t r_reg(Si7021_registers_t reg);
int8_t w_reg(uint8_t value, Si7021_registers_t reg);
uint16_t convert_to_uint16(uint8_t bytes[]);
float process_temp_code(uint16_t temp_code);
float process_humi_code(uint16_t humi_code);
int8_t r_single_Si7021(float* data, Si7021_measurement_type_t type);
int8_t r_both_Si7021(float* humidity, float* temperature);
int8_t set_heater_current_Si7021(uint8_t current);
int8_t r_heater_current_Si7021(void);
int8_t rst_Si7021(void);
int8_t enable_heater_Si7021(uint8_t val);