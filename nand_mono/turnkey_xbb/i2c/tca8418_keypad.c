////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief Driver of TCA8418 keypad scan module.
///
/// Some notes about tca8418a:
/// 1. All the registers can be read and written. The default value after reset
///    is 0.
/// 2. All the 18 GPIOs (ROW0-7 and COL0-9) is defaultly set to GPIO mode with
///    INPUT direction and pullup resistors after reset. Later, you can set them
///    to keypad mode.
/// 3. In keyscan mode, ROWs are used as an input with internal pullup resistor.
///    COLs are used as an output.
/// 4. When working in keyscan mode, the idle status: cols are output driving
///    low and rows are input with pull-ups. So the input for rows will be high.
///    Once a key is pressed, some row will read low since the column outputs low.
///    At this stage, it can detect the row which has a pressed key. Then it starts
///    a key scan cycle to determine the column of the key that was pressed by:
///    set one column as output low and all other columns as high.
///
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
// Below info could be different based on the different application.
// There are 8 rows from row 0 to7 and 10 columns from col 0 to 9. So for the
// configuration of column, there could be two bytes for two registers.
//----------------------------------------------------------------------------

// TCA8418 device address on i2c bus
#define TCA8418_ADDR_WR         0x68
#define TCA8418_ADDR_RD         0x69

// Keypad matrix registers mask based on what we use:
// row 0-4 and col 0-2 for keypad.
#define REG_MASK_MATRIX_ROW     0x1F
#define REG_MASK_MATRIX_COL_LO  0x07  

// GPIOs are used to drive LEDs:
// row 5,6,7 and column 3,4,5,6,7,8.
// After reset, the initial mode is GPIO INPUT mode.
#define REG_MASK_GPIO_ROW       0xE0
#define REG_MASK_GPIO_COL_LO    0xF8
#define REG_MASK_GPIO_COL_HI    0x01
#define REG_MASK_GPIO_COL       0x01F8       

// The counter of keypad we use tca8418 to scan 
// The counter of led we use in our hw design.
// But I don't use them as the size to define a table...
#define KEYMAP_COUNTER          15
#define LEDMAP_COUNTER          18

// 128 pin evaluation board:
// P1.3 is used as the play key. But I set it as output direction.
// Not sure if it can work as expectedly.
//
// 64 pin hw design with SPDA2635A:
// P1.3 is used as the reset pin for the keyscan id. Low active.
#define TCA8418_RESET_S         P1_3=0
#define TCA8418_RESET_E         P1_3=1

// Define the enumerated names for leds. Does the order matter?
typedef enum
{
    LED_FM,
    LED_OPERA,
    LED_PLAY,
    LED_MUSIC,              // there is no this category???
    LED_FICTION,
    LED_PREV,
    LED_CITY,
    LED_COMIC,
    LED_NEXT,
    LED_SONG,
    LED_STORY,
    LED_FORWARD,
    LED_LECTURE,
    LED_MISC,
    LED_BACKWARD,
    LED_HEALTH,

    // Keep this as last.
    LED_MAX,
} led_t;

// Attributes describing how we should control the led.
typedef enum
{
    LED_ATTR_NO_ACT,             // don't do anything.
    LED_ATTR_SWITCH,             // turn on or off.
    LED_ATTR_FLASHED,            // light it and turn it off.
    LED_ATTR_BLINKING,           // blink periodically.

    LED_ATTR_MAX,
} led_attr_t;

typedef enum
{
    LED_ACTION_OFF,
    LED_ACTION_ON,
} led_action_t;




// In our hw design, the mapping between key event and key code and the mapping
// between led and (row, col) is fixed by hw. So those info can be defined as
// a static table. If you want to add some control which is used to dynamically
// access this mapping, you should define a new type. This way, we can have a
// better layer model.
//
// Key mapping: key code -> real key. A real key is the 1st level of key info.
// In our project, we use row 0,1,2,3,4 and col 0,1,2 as the keypad. 15 keys in
// total.
typedef struct
{
    uint8 key_code;
    uint8 key_value;
} tca8418_keymap_t;

// Led mapping.
typedef struct
{
    uint8               key_code;
    led_t               led;
    uint8               row;        // used to drive this led, 0-7.
    uint8               col;        // used to drive this led, 0-9.
    led_attr_t          attr;       // OR of attributes.   
} tca8418_ledmap_t;   

xdata tca8418_keymap_t tca8418_keymap[] =
{
    {1,  KEY_VALUE_LECTURE},{2,  KEY_VALUE_HEALTH}, {3,  KEY_VALUE_NEXT},
    {11, KEY_VALUE_FICTION},{12, KEY_VALUE_SONG},   {13, KEY_VALUE_PREV},
    {21, KEY_VALUE_COMIC},  {22, KEY_VALUE_OPERA},  {23, KEY_VALUE_PLAY},
    {31, KEY_VALUE_MISC},   {32, KEY_VALUE_STORY},  {33, KEY_VALUE_BACKWARD},
    {41, KEY_VALUE_CITY},   {42, KEY_VALUE_FM},     {43, KEY_VALUE_FORWARD},
};

xdata tca8418_ledmap_t tca8418_ledmap[] =
{
    {1,     LED_LECTURE,    5, 4, LED_ATTR_SWITCH},
    {2,     LED_HEALTH,     5, 5, LED_ATTR_SWITCH},
    //{3,     LED_NEXT,       5, 6, LED_ATTR_SWITCH}, // Temporary use.
    {11,    LED_FICTION,    7, 4, LED_ATTR_SWITCH},    
    {12,    LED_SONG,       7, 5, LED_ATTR_SWITCH},
    {21,    LED_COMIC,      6, 4, LED_ATTR_SWITCH},
    {22,    LED_OPERA,      6, 5, LED_ATTR_SWITCH},
    //{23,    LED_PLAY,       5, 6, LED_ATTR_SWITCH},
    {31,    LED_MISC,       5, 3, LED_ATTR_SWITCH},                
    {32,    LED_STORY,      6, 3, LED_ATTR_SWITCH},
    {41,    LED_CITY,       7, 3, LED_ATTR_SWITCH},
    {42,    LED_FM,         7, 6, LED_ATTR_SWITCH},
     
//    // Keep this at the bottom!
//    {0, 0, 0, 0, 0},        
};


static tca8418_ledmap_t *led_pre=NULL;
static tca8418_ledmap_t *led_cur=NULL;

#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)
//i2c driver used for TCA8148
static i2c_driver_t *tca8418_drv = &i2c_driver_tca8418 reentrant;
#endif




///-----------------------------------------------------------------------------
/// Write a byte to a register of tca8418a.
///
/// @param  
///
/// @return error status:
///             0: no error.
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
int8 tca8418_write_byte (uint8 reg, int8 val)
{
    int8 err;
    
    #if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

    err = tca8418_drv->i2c_write_reg_byte (tca8418_drv,
                                           TCA8418_ADDR_WR,
                                           reg,
                                           val);

    #else

    err = i2c_write_reg_byte (TCA8418_ADDR_WR, reg, val);

    #endif 

    return err;
}

///-----------------------------------------------------------------------------
/// Read one byte of content from a register in tca8418a.
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

    #if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

    err = tca8418_drv->i2c_read_reg_byte (tca8418_drv,
                                          TCA8418_ADDR_RD,
                                          reg,
                                          p_val);

    #else

    err = i2c_read_reg_byte_tca8418a (TCA8418_ADDR_WR, reg, p_val);

    #endif

    return err;
}

///-----------------------------------------------------------------------------
/// Control the GPIOs in tca8418a to drive a led. 
    // Based on the current hw design, if we want to light a led:
    //      row: output, high.
    //      col: output, low.
    // So when we control a row, we need to do 2 things:
    //      1. set the relative bits in REG_GPIO_DAT_OUT1.
    //      2. set the GPIO direction to OUTPUT, REG_GPIO_DIR1.
    // When we control a col, we need to do 1 thing:
    //      1. set the GPIO direction to OUTPUT. 
    // This is because all the default value of registers is 0 and we don't change
    // the data value related settings to 1... So when we only toggle the direction,
    // the GPIO changes between INPUT and low output.
    //
    // But if we have set row's direction, we don't do it again. So the best way is
    // to initialize the direction in initialization. And then we toggle the output
    // data for row and col.
///
/// How we control a led depends on the attribute and the current status.
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/12/23
///-----------------------------------------------------------------------------
void tca8418_led_drive (uint8 led, uint8 action)
{
    uint8 i;
    uint8 row, col, attr;

    uint8 mask_row;
    uint16 mask_col;
    uint8 ledmap_cnt;
    uint8 reg_tmp;

    ledmap_cnt = sizeof(tca8418_ledmap)/sizeof(tca8418_ledmap_t);

    // Using led as the index, search the led mapping info in the table.
    for (i=0; i<ledmap_cnt; i++)
    {
        if (led == tca8418_ledmap[i].led)
        {
            row = tca8418_ledmap[i].row;
            col = tca8418_ledmap[i].col;
            attr = tca8418_ledmap[i].attr; 
            break;           
        }
    }

    // Theoretically, we will definitely search it successfully since this is
    // the second search in the table. If it fails first time, we will not come
    // here. We use different thing to search between two times. So below code
    // is just for safe.
    if (i==ledmap_cnt)
    {
        // We don't find this entry in led map table... Quit.
        dbprintf ("Don't find any entry (led) in led map\n");
        return;
    }
        
    // Build the mask.
    mask_row = 1<<row;
    mask_col = 1<<col;
    
    // Based on the attribute, control the GPIOs by modify the registers.
    // Currently, I only implement the LED_ATTR_SWITCH. For the leds with attribute
    // LED_ATTR_SWITCH, there will be only one led lit at any time. So there are
    // several ways to achieve this:
    //      1. Maintain the current status of leds: which led is lit now.
    //      2. Check the register contents to find out the current status of leds.
    // I prefer the first one. But it is maintained at one higher level.
    //
    // REVISIT!!!
    // The turn-off operation should be done at first because some leds share the same
    // GPIOs. With the current impl, turn-off operation will turn-off all leds sharing
    // the same GPIO. That's too bad. A terrible hw design! This is a must fix later.
    // A better way could be subscribe the turn-off request...
    if (attr==LED_ATTR_SWITCH)
    {
        // Is it a turn-off?
        if (action == LED_ACTION_OFF)
        {
            // Turn off the row driver. 5 leds sharing this row...
            tca8418_read_byte (REG_GPIO_DAT_OUT1, &reg_tmp);
            tca8418_write_byte (REG_GPIO_DAT_OUT1, reg_tmp & ~mask_row);
            
            // Turn off the col driver. output is high. 3 leds sharing this col...
            if (mask_col&0x00FF)
            {
                tca8418_read_byte (REG_GPIO_DAT_OUT2, &reg_tmp);
                tca8418_write_byte (REG_GPIO_DAT_OUT2, reg_tmp | (mask_col&0xFF));   
            } 
            if (mask_col>>8)
            {
                tca8418_read_byte (REG_GPIO_DAT_OUT3, &reg_tmp);
                tca8418_write_byte (REG_GPIO_DAT_OUT3, reg_tmp | (mask_col>>8));   
            }        
        }
        // Is it a turn-on?
        else if (action == LED_ACTION_ON)
        {
            // Turn on the row driver. This will turn on 5 leds sharing this row...
            tca8418_read_byte (REG_GPIO_DAT_OUT1, &reg_tmp);
            tca8418_write_byte (REG_GPIO_DAT_OUT1, reg_tmp|mask_row);

            // Turn on the col driver. output is low. 3 leds sharing this col...
            if (mask_col&0xFF)
            {
                tca8418_read_byte (REG_GPIO_DAT_OUT2, &reg_tmp);
                tca8418_write_byte (REG_GPIO_DAT_OUT2, reg_tmp & (~(mask_col&0xFF)));   
            } 
            if (mask_col>>8)
            {
                tca8418_read_byte (REG_GPIO_DAT_OUT3, &reg_tmp);
                tca8418_write_byte (REG_GPIO_DAT_OUT3, reg_tmp & (~(mask_col>>8)));   
            } 
        }
        else
        {
            // We shouldn't be here... If you don't want to do anything, don't
            // call this function.
            dbprintf ("no led action\n");
        }
        return;        
    }    
    
}

///-----------------------------------------------------------------------------
/// LED control function drived by tca8418a GPIOs.
///
/// Obviously, the led is heavily customized by our own hw design. We can treat
/// it as a part of key control. There will be no a layer for led on the ui level.
///
/// @param  key_code: uint8. A value standing for the key in tca8418a. It's the
///                   same with the key code in key map.
///
/// @return 
///
/// @author William Chang
/// @date   2012/12/21
///-----------------------------------------------------------------------------
void tca8418_led_handle (uint8 key_code)
{
    uint8 i;
    uint8 ledmap_cnt;
    tca8418_ledmap_t *led_tmp;

    if (key_code == 0)
    {
        // We don't accept 0 key code... Warn and quit.
        dbprintf ("key code is 0 for led handle!\n");
        return;
    }

    ledmap_cnt = sizeof(tca8418_ledmap)/sizeof(tca8418_ledmap_t);

    // Based on the code to locate it in the led map table, then get its info.
    for (i=0; i< ledmap_cnt; i++)
    {
        if (key_code == tca8418_ledmap[i].key_code)
        {
            break;
        }
    }

    if (i==ledmap_cnt)
    {
        // We don't find this entry in led map table... Quit.
        dbprintf ("Don't find any entry in led map\n");
        return;
    }
    
    // Check if the new one is the same with current.
    led_tmp = &tca8418_ledmap[i];
    if (led_tmp == led_cur)
    {
        dbprintf("it's the same led\n");
        return;
    }

    // Based on its attribute, control it.
    // Turn off the current one, and turn on the new one.
    if (led_cur != NULL)
    {
        tca8418_led_drive (led_cur->led, LED_ACTION_OFF);
    }
    tca8418_led_drive (led_tmp->led, LED_ACTION_ON);

    // update the current pointer.
    led_cur = led_tmp;
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
    int8 err=0;

    // Reset this ic.
    // REVISIT!!!
    // When we set this ic here, there will be an error for the first communication
    // message which will be next statement below here. Probably the reset duration
    // is not satisfied.
    // We don't have to reset here since the ic is reset when powers up.
    TCA8418_RESET_S;
    USER_DelayDTms(1);
    TCA8418_RESET_E;
    USER_DelayDTms(1);

    // Write config register.
    // Currently we don't use overflow feature, any interrupt, key lock feature,
    // GPI feature, etc for tca8418. Basically, just keep the default value 0.
    err = tca8418_write_byte (REG_CFG, 0);

    // Set registers to keypad scan mode based on which keys we use.
    // REVISIT!!!
    // A better way to set is read-modify-write. Below direct modification
    // could change something unexpectedly. But our usage of tca8418a is simple.
    // So I leave it there. 
    err |= tca8418_write_byte (REG_KP_GPIO1, REG_MASK_MATRIX_ROW);
    err |= tca8418_write_byte (REG_KP_GPIO2, REG_MASK_MATRIX_COL_LO);
    
    // Initialize out data for the GPIOS used to drive led.
    // row is output low; col is output high;
    err |= tca8418_write_byte (REG_GPIO_DAT_OUT2, REG_MASK_GPIO_COL&0xFF);
    err |= tca8418_write_byte (REG_GPIO_DAT_OUT3, REG_MASK_GPIO_COL>>8);

    // Set the GPIOs used to drive led as OUTPUT mode.
    err |= tca8418_write_byte (REG_GPIO_DIR1, REG_MASK_GPIO_ROW);
    err |= tca8418_write_byte (REG_GPIO_DIR2, REG_MASK_GPIO_COL&0xFF);
    err |= tca8418_write_byte (REG_GPIO_DIR3, REG_MASK_GPIO_COL>>8);                            

    // REVISIT!!!
    // Enable debouncing???
}

///-----------------------------------------------------------------------------
/// Get a real key value from tca8418.
///
/// This is all what we want to know from tca8418. We want to know which key the
/// user have "pressed". That means the key is pressed first and then released.
/// We only care about the released event in the tca8418a since we don't use any
/// long key function on the keypad matrix.
///
/// Each time this function is called, it returns only one key value.
///
/// @param  
///
/// @return uint16: a real key value.
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
uint8 tca8418_get_real_key (void)
{
    uint8 i;
    uint8 j;
    int8 err;
    uint8 event_cnt;
    int8 reg_a;
    int8 key_code=0;

    // Read how many key event there are.
    err = tca8418_read_byte (REG_KEY_LCK_EC, &event_cnt);
    event_cnt &= KEY_LCK_EC_KEC;

    // Quit right away if key counter is 0.
    if (event_cnt == 0)
    {
        return KEY_VALUE_NONE;
    }
    //dbprintf ("key counter is %bx\n",event_cnt);

    // Read the released key events.
    for (i=0; i<event_cnt; i++)
    {
        err|= tca8418_read_byte (REG_KEY_EVENT_A, &reg_a);
        
        // REVISIT!!!
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

    // Search through the keymap to find out which key the key code means.
    // REVISIT!!!
    // Are you sure the expression is correct below? or We can use this one for
    // safe:
    // for (j=0; j<KEYMAP_COUNTER; j++)
    // Yes, I am sure it's correct.
    for (j=0; j<( sizeof(tca8418_keymap)/sizeof(tca8418_keymap_t) ); j++)
    {
        if (tca8418_keymap[j].key_code == key_code)
        {
            dbprintf ("ked code is %bx\n", key_code);
            //dbprintf ("key value is %bx\n", tca8418_keymap[j].key_value);
            tca8418_led_handle (key_code);
            return tca8418_keymap[j].key_value;
        }
    }
    
    // REVISTI!!!                                                               
    // If we are here, there is something wrong in the keymap. Use the 
    // KEY_VALUE_NONE just for simplicity without error handling.
    dbprintf ("Don't find the key code in the key map\n");
    return KEY_VALUE_NONE;    
}

///-----------------------------------------------------------------------------
/// Check if the key is a key to switch the category.
///
/// @param uint8 key
/// @return 0: false; 1: true.
/// @date 2013/01/10
///-----------------------------------------------------------------------------
uint8 tca8418_is_category_key (uint8 key)
{
    if ((key == KEY_VALUE_OPERA)
        || (key == KEY_VALUE_MUSIC)
        || (key == KEY_VALUE_FICTION)
        || (key == KEY_VALUE_COMIC)
        || (key == KEY_VALUE_SONG)
        || (key == KEY_VALUE_STORY)
        || (key == KEY_VALUE_LECTURE)
        || (key == KEY_VALUE_MISC)
        || (key == KEY_VALUE_HEALTH))
    {
        dbprintf ("It is a category key %bx\n", key);
        return 1;
    }
    else
    {
        dbprintf ("It is not a category key %bx\n", key);
        return 0;
    }
}

///-----------------------------------------------------------------------------
/// LED test routine drived using tca8418.
///
/// It is supposed to be used in tca8418_test() routine.
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/12/21
///-----------------------------------------------------------------------------
#if (TCA8418_TEST == FEATURE_ON)
void tca8418_test_led (void)
{
//    // Test D5 which is drived by LED_D1 (ROW5) and LED_S2 (COL4).
//    // Define a mask for convenience. 
//    uint8 reg_read;
//    uint8 row_mask = 0x20;
//    uint8 col_mask_lo = 0x10;
//
//    // Set related row to high before set it as output. The default after reset is 0.
//    tca8418_read_byte (REG_GPIO_DAT_OUT1, &reg_read);
//    tca8418_write_byte (REG_GPIO_DAT_OUT1, reg_read|row_mask); 
//
//    // set col to GPIO OUTPUT direction. The default value after reset is 0, which
//    // means INTPUT.
//    tca8418_read_byte (REG_GPIO_DIR1, &reg_read);
//    tca8418_write_byte (REG_GPIO_DIR1, reg_read|row_mask);
//
//    tca8418_read_byte (REG_GPIO_DIR2, &reg_read);
//    tca8418_write_byte (REG_GPIO_DIR2, reg_read|col_mask_lo);

    uint8 i;

    while (1)
    {
        for (i=LED_FM; i<LED_MAX-1; i++)
        {
            tca8418_led_drive (i, LED_ACTION_ON);
        }
        USER_DelayDTms(20000);
        for (i=LED_FM; i<LED_MAX-1; i++)
        {
            tca8418_led_drive (i, LED_ACTION_OFF);
        }
        USER_DelayDTms(20000);
    } 
}

#endif

///-----------------------------------------------------------------------------
/// Test routine for tca8418.
///
/// It is supposed to be used independantly in main().
///
/// @param  
///
/// @return 
///
/// @author William Chang
/// @date   2012/12/16
///-----------------------------------------------------------------------------
#if (TCA8418_TEST == FEATURE_ON)
void tca8418_test()
{
    uint8 reg_val;
    uint8 i;
    uint8 key_value;

    tca8418_init();
    
    //tca8418_test_led();
    //tca8418_led_drive (LED_FM, LED_ACTION_ON);

    // Read all the registers and print them out.
//    for (i=REG_CFG; i<=REG_GPIO_PULL3; i++)
//    {
//        tca8418_read_byte (i, &reg_val);
//        dbprintf ("reg %bx = %bx\n", i, reg_val);
//    }

    while(1)
    {
        tca8418_get_real_key();
    }
}
#endif