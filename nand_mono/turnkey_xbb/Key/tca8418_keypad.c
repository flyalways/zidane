////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief Driver of TCA8418 keypad scan module.
///
/// @author William Chang
/// @date   2012/11/03
////////////////////////////////////////////////////////////////////////////////

#include "SPDA2K.H"
#include "tca8418_keypad.h"
#include "i2c.h"

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
#define KEY_LCK_EC_KEC		0x7
#define KEY_EVENT_CODE		0x7f
#define KEY_EVENT_VALUE		0x80




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

//i2c driver used for TCA8148
static i2c_driver_t *tca8418_drv = &i2c_driver_tca8418;

// Key mapping: key code -> real key. A real key is the 1st level of key info.
// In our project, we use row 4,5,6 and col 0,1,2 as the keypad.
xdata uint16 tca8418_keymap[][2] =
{
    {40,}, {41,}, {42,},
    {51,}, {52,}, {53,},
    {61,}, {62,}, {63,},
};

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
    int8 err

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
/// This is all what we want to know from tca8418.
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
        
}







#if 0
// 
struct tca8418_keypad {
	unsigned int rows;
	unsigned int cols;
	unsigned int keypad_mask; /* Mask for keypad col/rol regs */
	unsigned int irq;
	unsigned int row_shift;

	struct i2c_client *client;
	struct input_dev *input;

	/* Flexible array member, must be at end of struct */
	unsigned short keymap[];
};

/*
 * Write a byte to the TCA8418
 */
static int tca8418_write_byte(struct tca8418_keypad *keypad_data,
			      int reg, u8 val)
{
	int error;

	error = i2c_smbus_write_byte_data(keypad_data->client, reg, val);
	if (error < 0) {
		dev_err(&keypad_data->client->dev,
			"%s failed, reg: %d, val: %d, error: %d\n",
			__func__, reg, val, error);
		return error;
	}

	return 0;
}

/*
 * Read a byte from the TCA8418
 */
static int tca8418_read_byte(struct tca8418_keypad *keypad_data,
			     int reg, u8 *val)
{
	int error;

	error = i2c_smbus_read_byte_data(keypad_data->client, reg);
	if (error < 0) {
		dev_err(&keypad_data->client->dev,
				"%s failed, reg: %d, error: %d\n",
				__func__, reg, error);
		return error;
	}

	*val = (u8)error;

	return 0;
}

static void tca8418_read_keypad(struct tca8418_keypad *keypad_data)
{
	int error, col, row;
	u8 reg, state, code;

	/* Initial read of the key event FIFO */
	error = tca8418_read_byte(keypad_data, REG_KEY_EVENT_A, &reg);

	/* Assume that key code 0 signifies empty FIFO */
	while (error >= 0 && reg > 0) {
		state = reg & KEY_EVENT_VALUE;
		code  = reg & KEY_EVENT_CODE;

		row = code / TCA8418_MAX_COLS;
		col = code % TCA8418_MAX_COLS;

		row = (col) ? row : row - 1;
		col = (col) ? col - 1 : TCA8418_MAX_COLS - 1;

		code = MATRIX_SCAN_CODE(row, col, keypad_data->row_shift);
		input_event(keypad_data->input, EV_MSC, MSC_SCAN, code);
		input_report_key(keypad_data->input,
				keypad_data->keymap[code], state);

		/* Read for next loop */
		error = tca8418_read_byte(keypad_data, REG_KEY_EVENT_A, &reg);
	}

	if (error < 0)
		dev_err(&keypad_data->client->dev,
			"unable to read REG_KEY_EVENT_A\n");

	input_sync(keypad_data->input);
}

/*
 * Threaded IRQ handler and this can (and will) sleep.
 */
static irqreturn_t tca8418_irq_handler(int irq, void *dev_id)
{
	struct tca8418_keypad *keypad_data = dev_id;
	u8 reg;
	int error;

	error = tca8418_read_byte(keypad_data, REG_INT_STAT, &reg);
	if (error) {
		dev_err(&keypad_data->client->dev,
			"unable to read REG_INT_STAT\n");
		goto exit;
	}

	if (reg & INT_STAT_OVR_FLOW_INT)
		dev_warn(&keypad_data->client->dev, "overflow occurred\n");

	if (reg & INT_STAT_K_INT)
		tca8418_read_keypad(keypad_data);

exit:
	/* Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK) */
	reg = 0xff;
	error = tca8418_write_byte(keypad_data, REG_INT_STAT, reg);
	if (error)
		dev_err(&keypad_data->client->dev,
			"unable to clear REG_INT_STAT\n");

	return IRQ_HANDLED;
}

/*
 * Configure the TCA8418 for keypad operation
 */
static int __devinit tca8418_configure(struct tca8418_keypad *keypad_data)
{
	int reg, error;

	/* Write config register, if this fails assume device not present */
	error = tca8418_write_byte(keypad_data, REG_CFG,
				CFG_INT_CFG | CFG_OVR_FLOW_IEN | CFG_KE_IEN);
	if (error < 0)
		return -ENODEV;


	/* Assemble a mask for row and column registers */
	reg  =  ~(~0 << keypad_data->rows);
	reg += (~(~0 << keypad_data->cols)) << 8;
	keypad_data->keypad_mask = reg;

	/* Set registers to keypad mode */
	error |= tca8418_write_byte(keypad_data, REG_KP_GPIO1, reg);
	error |= tca8418_write_byte(keypad_data, REG_KP_GPIO2, reg >> 8);
	error |= tca8418_write_byte(keypad_data, REG_KP_GPIO3, reg >> 16);

	/* Enable column debouncing */
	error |= tca8418_write_byte(keypad_data, REG_DEBOUNCE_DIS1, reg);
	error |= tca8418_write_byte(keypad_data, REG_DEBOUNCE_DIS2, reg >> 8);
	error |= tca8418_write_byte(keypad_data, REG_DEBOUNCE_DIS3, reg >> 16);

	return error;
}

static int __devinit tca8418_keypad_probe(struct i2c_client *client,
					  const struct i2c_device_id *id)
{
	const struct tca8418_keypad_platform_data *pdata =
						client->dev.platform_data;
	struct tca8418_keypad *keypad_data;
	struct input_dev *input;
	int error, row_shift, max_keys;

	/* Copy the platform data */
	if (!pdata) {
		dev_dbg(&client->dev, "no platform data\n");
		return -EINVAL;
	}

	if (!pdata->keymap_data) {
		dev_err(&client->dev, "no keymap data defined\n");
		return -EINVAL;
	}

	if (!pdata->rows || pdata->rows > TCA8418_MAX_ROWS) {
		dev_err(&client->dev, "invalid rows\n");
		return -EINVAL;
	}

	if (!pdata->cols || pdata->cols > TCA8418_MAX_COLS) {
		dev_err(&client->dev, "invalid columns\n");
		return -EINVAL;
	}

	/* Check i2c driver capabilities */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
		dev_err(&client->dev, "%s adapter not supported\n",
			dev_driver_string(&client->adapter->dev));
		return -ENODEV;
	}

	row_shift = get_count_order(pdata->cols);
	max_keys = pdata->rows << row_shift;

	/* Allocate memory for keypad_data, keymap and input device */
	keypad_data = kzalloc(sizeof(*keypad_data) +
			max_keys * sizeof(keypad_data->keymap[0]), GFP_KERNEL);
	if (!keypad_data)
		return -ENOMEM;

	keypad_data->rows = pdata->rows;
	keypad_data->cols = pdata->cols;
	keypad_data->client = client;
	keypad_data->row_shift = row_shift;

	/* Initialize the chip or fail if chip isn't present */
	error = tca8418_configure(keypad_data);
	if (error < 0)
		goto fail1;

	/* Configure input device */
	input = input_allocate_device();
	if (!input) {
		error = -ENOMEM;
		goto fail1;
	}
	keypad_data->input = input;

	input->name = client->name;
	input->dev.parent = &client->dev;

	input->id.bustype = BUS_I2C;
	input->id.vendor  = 0x0001;
	input->id.product = 0x001;
	input->id.version = 0x0001;

	error = matrix_keypad_build_keymap(pdata->keymap_data, NULL,
					   pdata->rows, pdata->cols,
					   keypad_data->keymap, input);
	if (error) {
		dev_dbg(&client->dev, "Failed to build keymap\n");
		goto fail2;
	}

	if (pdata->rep)
		__set_bit(EV_REP, input->evbit);
	input_set_capability(input, EV_MSC, MSC_SCAN);

	input_set_drvdata(input, keypad_data);

	if (pdata->irq_is_gpio)
		client->irq = gpio_to_irq(client->irq);

	error = request_threaded_irq(client->irq, NULL, tca8418_irq_handler,
				     IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
				     client->name, keypad_data);
	if (error) {
		dev_dbg(&client->dev,
			"Unable to claim irq %d; error %d\n",
			client->irq, error);
		goto fail2;
	}

	error = input_register_device(input);
	if (error) {
		dev_dbg(&client->dev,
			"Unable to register input device, error: %d\n", error);
		goto fail3;
	}

	i2c_set_clientdata(client, keypad_data);
	return 0;

fail3:
	free_irq(client->irq, keypad_data);
fail2:
	input_free_device(input);
fail1:
	kfree(keypad_data);
	return error;
}

static int __devexit tca8418_keypad_remove(struct i2c_client *client)
{
	struct tca8418_keypad *keypad_data = i2c_get_clientdata(client);

	free_irq(keypad_data->client->irq, keypad_data);

	input_unregister_device(keypad_data->input);

	kfree(keypad_data);

	return 0;
}


static struct i2c_driver tca8418_keypad_driver = {
	.driver = {
		.name	= TCA8418_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= tca8418_keypad_probe,
	.remove		= __devexit_p(tca8418_keypad_remove),
	.id_table	= tca8418_id,
};

static int __init tca8418_keypad_init(void)
{
	return i2c_add_driver(&tca8418_keypad_driver);
}
subsys_initcall(tca8418_keypad_init);

static void __exit tca8418_keypad_exit(void)
{
	i2c_del_driver(&tca8418_keypad_driver);
}
module_exit(tca8418_keypad_exit);

MODULE_AUTHOR("Kyle Manna <kyle.manna@fuel7.com>");
MODULE_DESCRIPTION("Keypad driver for TCA8418");
MODULE_LICENSE("GPL");

#endif