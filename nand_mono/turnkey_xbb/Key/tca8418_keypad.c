////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief Driver of TCA8418 keypad scan module.
///
/// @author William Chang
/// @date   2012/11/03
////////////////////////////////////////////////////////////////////////////////

#include "SPDA2K.H"
#include "tca8418_keypad.h"
#include "../i2c/i2c.h"

/* TCA8418 hardware limits */
#define TCA8418_MAX_ROWS	8
#define TCA8418_MAX_COLS	10

/* TCA8418 register offsets */
#define REG_CFG			    0x01
#define REG_INT_STAT		0x02
#define REG_KEY_LCK_EC		0x03
#define REG_KEY_EVENT_A		0x04
#define REG_KEY_EVENT_B		0x05
#define REG_KEY_EVENT_C		0x06
#define REG_KEY_EVENT_D		0x07
#define REG_KEY_EVENT_E		0x08
#define REG_KEY_EVENT_F		0x09
#define REG_KEY_EVENT_G		0x0A
#define REG_KEY_EVENT_H		0x0B
#define REG_KEY_EVENT_I		0x0C
#define REG_KEY_EVENT_J		0x0D
#define REG_KP_LCK_TIMER	0x0E
#define REG_UNLOCK1		    0x0F
#define REG_UNLOCK2		    0x10
#define REG_GPIO_INT_STAT1	0x11
#define REG_GPIO_INT_STAT2	0x12
#define REG_GPIO_INT_STAT3	0x13
#define REG_GPIO_DAT_STAT1	0x14
#define REG_GPIO_DAT_STAT2	0x15
#define REG_GPIO_DAT_STAT3	0x16
#define REG_GPIO_DAT_OUT1	0x17
#define REG_GPIO_DAT_OUT2	0x18
#define REG_GPIO_DAT_OUT3	0x19
#define REG_GPIO_INT_EN1	0x1A
#define REG_GPIO_INT_EN2	0x1B
#define REG_GPIO_INT_EN3	0x1C
#define REG_KP_GPIO1		0x1D
#define REG_KP_GPIO2		0x1E
#define REG_KP_GPIO3		0x1F
#define REG_GPI_EM1		    0x20
#define REG_GPI_EM2		    0x21
#define REG_GPI_EM3		    0x22
#define REG_GPIO_DIR1		0x23
#define REG_GPIO_DIR2		0x24
#define REG_GPIO_DIR3		0x25
#define REG_GPIO_INT_LVL1	0x26
#define REG_GPIO_INT_LVL2	0x27
#define REG_GPIO_INT_LVL3	0x28
#define REG_DEBOUNCE_DIS1	0x29
#define REG_DEBOUNCE_DIS2	0x2A
#define REG_DEBOUNCE_DIS3	0x2B
#define REG_GPIO_PULL1		0x2C
#define REG_GPIO_PULL2		0x2D
#define REG_GPIO_PULL3		0x2E

/* TCA8418 bit definitions */
#define CFG_AI			    BIT(7)
#define CFG_GPI_E_CFG		BIT(6)
#define CFG_OVR_FLOW_M		BIT(5)
#define CFG_INT_CFG		    BIT(4)
#define CFG_OVR_FLOW_IEN	BIT(3)
#define CFG_K_LCK_IEN		BIT(2)
#define CFG_GPI_IEN		    BIT(1)
#define CFG_KE_IEN		    BIT(0)

#define INT_STAT_CAD_INT	BIT(4)
#define INT_STAT_OVR_FLOW_INT	BIT(3)
#define INT_STAT_K_LCK_INT	BIT(2)
#define INT_STAT_GPI_INT	BIT(1)
#define INT_STAT_K_INT		BIT(0)

/* TCA8418 register masks */
#define KEY_LCK_EC_KEC		0x0F
#define KEY_EVENT_CODE		0x7f
#define KEY_EVENT_TYPE		0x80




//----------------------------------------------------------------------------
// Below info could be different based on the different application
//----------------------------------------------------------------------------

// TCA8418 device address on i2c bus
#define TCA8418_ADDR_WR     0x68
#define TCA8418_ADDR_RD     0x69

// Keypad matrix registers mask based on what we use:
// row 4,5,6 and col 0,1,2.
#define REG_MASK_ROW        0x7F
#define REG_MASK_COL        0x07

// The counter of keypad we use tca8418 to scan 
#define KEYMAP_COUNTER      15

// Key value definition. Used as a combination with key definition in define.h
#define KEY_VALUE_NONE          0
#define KEY_VALUE_FM            0x21
#define KEY_VALUE_OPERA         0x22
#define KEY_VALUE_PLAY          0x23
#define KEY_VALUE_MUSIC         0x24
#define KEY_VALUE_FICTION       0x25
#define KEY_VALUE_PREV          0x26
#define KEY_VALUE_CITY          0x27
#define KEY_VALUE_COMIC         0x28
#define KEY_VALUE_NEXT          0x29
#define KEY_VALUE_SONG          0x2A
#define KEY_VALUE_STORY         0x2B
#define KEY_VALUE_FORWARD       0x2C
#define KEY_VALUE_LECTURE       0x2D
#define KEY_VALUE_MISC          0x2E
#define KEY_VALUE_BACKWARD      0x2F

// Key mapping: key code -> real key. A real key is the 1st level of key info.
// In our project, we use row 0,1,2,3,4 and col 0,1,2 as the keypad. 15 keys in
// total.
typedef struct
{
    uint8 key_code;
    uint8 key_value;
} tca8418_keymap_t;

xdata tca8418_keymap_t tca8418_keymap[KEYMAP_COUNTER] =
{
    {1,  KEY_VALUE_FM},     {2,  KEY_VALUE_OPERA},  {3,  KEY_VALUE_PLAY},
    {11, KEY_VALUE_MUSIC},  {12, KEY_VALUE_FICTION},{13, KEY_VALUE_PREV},
    {21, KEY_VALUE_CITY},   {22, KEY_VALUE_COMIC},  {23, KEY_VALUE_NEXT},
    {31, KEY_VALUE_SONG},   {32, KEY_VALUE_STORY},  {33, KEY_VALUE_FORWARD},
    {41, KEY_VALUE_LECTURE},{42, KEY_VALUE_MISC},   {43, KEY_VALUE_BACKWARD},
};

//i2c driver used for TCA8148
static i2c_driver_t *tca8418_drv = &i2c_driver_tca8418;




///-----------------------------------------------------------------------------
/// 
///
/// 
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
int8 tca8418_write_byte (uint8 reg, int8 val)
{
    int8 err;
    
    err = tca8418_drv->i2c_write_reg_byte (tca8418_drv,
                                           TCA8418_ADDR_WR,
                                           reg,
                                           val);
//    err = i2c_driver_tca8418.i2c_write_reg_byte (tca8418_drv,
//                                           TCA8418_ADDR_WR,
//                                           reg,
//                                           val);
    return err;
}

///-----------------------------------------------------------------------------
/// 
///
/// 
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
int8 tca8418_read_byte (uint8 reg, int8 *p_val)
{
    int8 err;

    err = tca8418_drv->i2c_read_reg_byte (tca8418_drv,
                                          TCA8418_ADDR_RD,
                                          reg,
                                          p_val);
//    err = i2c_driver_tca8418.i2c_read_reg_byte (tca8418_drv,
//                                          TCA8418_ADDR_RD,
//                                          reg,
//                                          p_val);
    return err;
}

///-----------------------------------------------------------------------------
/// Initialize tca8418.
///
/// At this level, the function could be completely independent. Try to impl good
/// modularization in order to port if needed later.
/// Currently, I don't handle with the possible errors for simplicity.
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
void tca8418_init (void)
{
    int8 err;

    // Write config register.
    // Currently we don't use overflow feature, any interrupt, key lock feature,
    // GPI feature, etc for tca8418. Basically, just keep the default value 0.
    err = tca8418_write_byte (REG_CFG, 0);

    // Set registers to keypad mode based on which keys we use.
    err |= tca8418_write_byte (REG_KP_GPIO1, REG_MASK_ROW);
    err |= tca8418_write_byte (REG_KP_GPIO2, REG_MASK_COL&0xFF);
    err |= tca8418_write_byte (REG_KP_GPIO3, REG_MASK_COL>>8);                            

    // REVISIT!!!
    // Enable debouncing???
}

///-----------------------------------------------------------------------------
/// Get a real key value from tca8418.
///
/// This is all what we want to know from tca8418. We want to know which key the
/// user have "pressed". That means the key is pressed first and then released.
///
/// @param  
///
/// @return uint16: a real key value.
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
uint16 tca8418_get_real_key (void)
{
    uint8 i;
    uint8 j;
    int8 err;
    uint8 event_cnt;
    int8 reg_a;
    int8 key_code;

    // Read how many key event there are.
    err = tca8418_read_byte (REG_KEY_LCK_EC, &event_cnt);
    event_cnt &= KEY_LCK_EC_KEC;

    // Read the released key events.
    for (i=0; i<event_cnt; i++)
    {
        err|= tca8418_read_byte (REG_KEY_EVENT_A, &reg_a);
        
        // REVIST!!!
        // The easiest way is only to check released type. This way, we are not
        // able to get a long pressed key event. We do this only for simplicity
        // temporarily.
        if (!(reg_a & KEY_EVENT_TYPE)) // Released type.
        {
            key_code = reg_a&KEY_EVENT_CODE;
            break;            
        } 
    }

    // If we do not get any released key event. Just quit.
    if (!key_code)
    {
        return KEY_VALUE_NONE;
    }

    // Search through the keymap to find out which key does the key code mean.
    // REVISIT!!!
    // Are you sure the expression is correct below? or We can use this one for
    // safe:
    // for (j=0; j<KEYMAP_COUNTER; j++)
    for (j=0; j<( sizeof(tca8418_keymap)/sizeof(tca8418_keymap_t) ); j++)
    {
        if (tca8418_keymap[j].key_code == key_code)
        {
            return tca8418_keymap[j].key_value;
        }
    }
    
    // REVISTI!!!                                                               
    // If we are here, there is something wrong in the keymap. Use the 
    // KEY_VALUE_NONE just for simplicity without error handling.
    return KEY_VALUE_NONE;    
}