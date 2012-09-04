#ifndef LCM_BMP_DRIVER_H
#define LCM_BMP_DRIVER_H

// This setting is important for use about how to use the ST7587+GWMS9708D.
// Details is at the top of LCM_BMP_driver.c.
#define LCM_SCAN_DIRECTION_SETTING  REVERSE_Y

// We want to use 160x120 driven by ST7587, but actually we can only use
// 159x120. Because we use column address to write DDRAM and each column address
// means 3 dots. So the x resolution must be the multiple of 3.
#define LCM_RESOLUTION_X        160
#define LCM_RESOLUTION_Y        120
#define LCM_RESOLUTION_LIMIT_X  384
#define LCM_RESOLUTION_LIMIT_Y  160

//-----------------------------------------------------------------------------
// Commands for LCM ST7587 
//-----------------------------------------------------------------------------
#define ST7587_AUTO_READ_CMD            0xD7
#define ST7587_AUTO_READ_DISABLE        0x9F
#define ST7587_OTP_SELECT_CMD           0xE4
#define ST7587_OTP_SELECT_DISABLE       0x19
#define ST7587_SLEEP_OUT_CMD            0x11
#define ST7587_SLEEP_IN_CMD             0x10
#define ST7587_DISPLAY_OFF_CMD          0x28
#define ST7587_DISPLAY_ON_CMD           0x29
#define ST7587_VOP_SET_CMD              0xC0
#define ST7587_VOP_SET_11V_LO           0xB9
#define ST7587_VOP_SET_11V_HI           0x00
#define ST7587_VOP_SET_15V_LO           0x1D
#define ST7587_VOP_SET_15V_HI           0x01
#define ST7587_BIAS_SET_CMD             0xC3
#define ST7587_BIAS_SET_1_9             0x05
#define ST7587_BIAS_SET_1_12            0x02
#define ST7587_BIAS_SET_1_14            0x00
#define ST7587_BOOSTER_SET_CMD          0xC4
#define ST7587_BOOSTER_SET_X8           0x07
#define ST7587_ANALOG_CTRL_CMD          0xD0
#define ST7587_ANALOG_CTRL_DATA         0x1D
#define ST7587_LINE_INVERSION_CMD       0xB5
#define ST7587_LINE_INVERSION_FRAME     0x00
#define ST7587_DDR_ENABLE_CMD           0x38
#define ST7587_DDR_IFC_CMD              0x3A
#define ST7587_DDR_IFC_DATA             0x03
#define ST7587_SCAN_DIRECTION_CMD       0x36
#define ST7587_SCAN_DIRECTION_DEFAULT   0x00     // COM0->COM159, SEG0->SEG383
#define ST7587_SCAN_DIRECTION_REVERSE_COL   0x40 // COM0->COM159, SEG383->SEG0
#define ST7587_SCAN_DIRECTION_REVERSE_ROW   0x80 // COM159->COM0, SEG0->SEG383
#define ST7587_SCAN_DIRECTION_REVERSE_ALL   0xC0 // COM159->COM0, SEG383->SEG0
#define ST7587_DUTY_SET_CMD             0xB0
#define ST7587_DUTY_SET_DATA_120        0x77
#define ST7587_PARTIAL_ON_CMD           0x12
#define ST7587_PARTIAL_OFF_CMD          0x13
#define ST7587_PARTIAL_SET_CMD          0xB4
#define ST7587_PARTIAL_SET_DATA         0xA0
#define ST7587_PARTIAL_AREA_CMD         0x30
#define ST7587_PARTIAL_AREA_START_HI    0x00
#define ST7587_PARTIAL_AREA_START_LO    0x00
#define ST7587_PARTIAL_AREA_END_HI      0x00
#define ST7587_PARTIAL_AREA_END_LO      0x77 // COM0-COM119
#define ST7587_INVERSION_DISABLE_CMD    0x20

// Below is the area setting for the whole range of 160x120
// 
// 2012/08/28, William:
// Column setting of ST7587 is somehow special and tricky. The unit is dot but 
//the each value n means 3 dots with address: n, n+1, n+2. That means the range
// of column area we set must be multiple of 3. It also determines that the 160
// resolution is actually 159 resolution by columns. If we set column's range
// in the command to be (1,3), the actual dot range will be (3, 11).
//
// Each byte of DDRAM data can describe 2 dots. So we have some problem like:
// Can we control or set column range to be 3 or 9? How many bytes of data will
// that be if the column range is an odd?
//
// The column range of character's modulo must be multiple of 8. The reason is,
// each byte controls 8 dots and the number of data at horizon direction each
// line in the dot matrix array must be multiple of bytes.
//
// Combined with consideration above, setting of column range must be multiple
// of 3*8. That's multiple of 24.
#define ST7587_COL_ADDR_SET_CMD         0x2A
#define ST7587_COL_ADDR_SET_START_HI    0x00
#define ST7587_COL_ADDR_SET_START_LO    0x00
#define ST7587_COL_ADDR_SET_END_HI      0x00
#define ST7587_COL_ADDR_SET_END_LO      0x34 // 159 SEG. SEG0 - SEG158. It's dot's position.
#define ST7587_ROW_ADDR_SET_CMD         0x2B
#define ST7587_ROW_ADDR_SET_START_HI    0x00
#define ST7587_ROW_ADDR_SET_START_LO    0x00
#define ST7587_ROW_ADDR_SET_END_HI      0x00
#define ST7587_ROW_ADDR_SET_END_LO      0x77 // 120 COM. COM0 - COM119
#define ST7587_ALL_PIXEL_OFF            0x22 // All pixel off
#define ST7587_ALL_PIXEL_ON             0x23 // All pixel on

#define ST7587_WRITE_DISPLAY_DATA_CMD   0x2C

//-----------------------------------------------------------------------------
// Data type declaration
//-----------------------------------------------------------------------------
typedef enum
{
    LCM_SWITCH_FMGPIO_TO_GPIO,
    LCM_SWITCH_FMGPIO_TO_NAND,
} lcm_switch_fmgpio_action_t;

// Groups of scan direction setting
typedef enum
{
    NORMAL,         // COM0->COM159, SEG0->SEG383
    REVERSE_X,      // COM0->COM159, SEG383->SEG0
    REVERSE_Y,      // COM159->COM0, SEG0->SEG383
    REVERSE_ALL,    // COM159->COM0, SEG383->SEG0
} scan_dir_t;

// Provide offset information in DDRAM based on different scan direction
// settings and different display ranges.
typedef struct 
{
    uint16 offset_x;
    uint16 offset_y;
} area_offset_t;

// function declaration
void lcm_write_command(uint8 command);
void lcm_write_data(uint8 content);
void lcm_clear_screen(void);
void lcm_init_spi(void);
void lcm_set_addr(uint8 row, uint8 col);
void lcm_test_exclusive(void);
void lcm_set_fmgpio(lcm_switch_fmgpio_action_t action);
void lcm_clear_ddram(void);
void lcm_write_data_cooked(uint8 content);
void lcm_set_disp_bound(uint16, uint16, uint16, uint16);

#endif