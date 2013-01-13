/**
 * @file
 * FM control layer. This module should be independent from kt0810 module.
 *
 * @date 2013/01/08
 */

#include "SPDA2K.H"
#include "kt0810.h"
#include "tca8418_keypad.h"

typedef enum
{
    FM_ACTION_NONE,

    FM_ACTION_SEEK_UP,
    FM_ACTION_SEEK_DOWN,
    FM_ACTION_TUNE_STEP_UP,
    FM_ACTION_TUNE_STEP_DOWN,
    FM_ACTION_TUNE_CHANNEL_PREV,
    FM_ACTION_TUNE_CHANNEL_NEXT,
    FM_ACTION_TUNE_CHANNEL_SET,
    FM_ACTION_VOL_UP,
    FM_ACTION_VOL_DOWN,

} fm_action_t;

// One key maps to one action. This is 1:1 mapping relationship.
// The key definition is in tca8418_keypad.h.
typedef struct
{   
    uint8       key;
    fm_action_t fm_action;
} fm_key_map_t;

// REVISIT!!!
// In our real application, the step tune will be replaced by pre-defined
// channel tune.
fm_key_map_t fm_key_map[] =
{
    {KEY_VALUE_PREV,        FM_ACTION_TUNE_CHANNEL_PREV},
    {KEY_VALUE_NEXT,        FM_ACTION_TUNE_CHANNEL_NEXT},  
    {KEY_VALUE_FORWARD,     FM_ACTION_SEEK_UP},
    {KEY_VALUE_BACKWARD,    FM_ACTION_SEEK_DOWN},
    {KEY_VALUE_VOL_UP,      FM_ACTION_VOL_UP},
    {KEY_VALUE_VOL_DOWN,    FM_ACTION_VOL_DOWN},

    {KEY_VALUE_NONE,        FM_ACTION_NONE},
};

// FM channel station list for cities. The unit is KHz.
code uint32 fm_chan_list_shanghai[] = 
{
    107700,
    105700,
    104500,
    103700,
    101700,
    99000,
    97700,
    96800,
    94700,
    93400,
    91400,
    89900,
    87900,

    // Leave 0 at the end pls.
    0,
};

code uint32 fm_chan_list_beijing[] = 
{
    107700,
    107300,
    106800,
    106100,
    105700,
    105200,
    104600,
    103900,
    103100,

    // Leave 0 at the end pls.
    0,
};

typedef enum
{
    fm_city_shanghai,
    fm_city_beijing,
} fm_city_t;

typedef struct
{
    fm_city_t   city;
    uint8       index_default;
    uint8       index_cur;
    uint8       chan_cnt;
    uint32      station_cur;
    uint32      *p_chan_list;
} fm_city_cfg_t;

// The main purpose of this table here is to connect the channel list of a city
// to the city. Other fields like the index stuff, they are 0 here. But they
// will be initialized in fm_init().
static fm_city_cfg_t fm_cities[] = 
{
    {fm_city_shanghai,      0, 0, 0, 0, fm_chan_list_shanghai},
    {fm_city_beijing,       0, 0, 0, 0, fm_chan_list_beijing},
};

// FM control block for the current FM management status.
static fm_city_cfg_t fm_block;
static fm_city_cfg_t *p_fm = &fm_block;

static fm_city_t fm_city = FM_CITY; 




///----------------------------------------------------------------------------
/// Initial FM configuration in our application.
///
/// The FM city is pre-defined as a feature flag. Here we will populate the fm
/// control block after we know which city it is. The pre-defined city flag will
/// be used as fm_city_t.
///
/// This function should be called only once after the FM IC is initialized. So
/// it shouldn't start to listen to any radio. Do it at other place.
///
/// @date 2013/01/08
///----------------------------------------------------------------------------   
void fm_init(void)
{
    uint8 i=0;
    uint8 j=0;
    uint8 cnt_city;

    cnt_city = sizeof(fm_cities)/sizeof(fm_city_cfg_t);

    // Search through the city config table to find where this city is.
    for (i=0; i<cnt_city; i++)
    {
        if (fm_city == fm_cities[i].city)
        {
            break;
        }
    }

    if (i == cnt_city)
    {
        dbprintf ("Invalid city %bx\n", fm_city);
        return; 
    }

    // Populate the FM control block.
    p_fm->city          = fm_city;
    p_fm->index_default = fm_cities[i].index_default;
    p_fm->index_cur     = p_fm->index_default; // After initial, we use default channel.
    p_fm->p_chan_list   = fm_cities[i].p_chan_list;
    p_fm->station_cur   = *(p_fm->p_chan_list + p_fm->index_cur);

    // Let me counter how many channels defined there are for this city.
    while (*(p_fm->p_chan_list + j))
    {
        j++;
    }
    p_fm->chan_cnt = j;
    
    // Print some cfg info for the FM setting.
    dbprintf ("FM city is %bx\n", p_fm->city);
    dbprintf ("FM default channel index is %bx\n", p_fm->index_default);
    dbprintf ("FM current channel index is %bx\n", p_fm->index_cur);
    dbprintf ("FM channel number is %bx\n", p_fm->chan_cnt);
    dbprintf ("FM current station is %lx\n", p_fm->station_cur); 
}

///----------------------------------------------------------------------------
/// Start FM to tune to the current station.
///
/// This function should be called every time FM task starts to run.
///
/// @date 2013/01/10
///----------------------------------------------------------------------------   
void fm_start(void)
{
    kt0810_set_station (p_fm->station_cur);
}

///----------------------------------------------------------------------------
/// Tune to a channel by next or previous we have pre-defined.
///
/// The operation to tune pre-defined channel is implemented in fm layer. Because
/// it is completely based on our application. It should be independent from
/// the driver layer.
///
/// @param uint8 dir: 0 means next channel, 1 means previous channel.
///
/// @date 2013/01/08
///----------------------------------------------------------------------------   
void fm_tune_chan (uint8 dir)
{
    
    // Modify the current index.
    if (!dir) // Next channel. Index should be added one or roll over to 0.
    {
        if (p_fm->index_cur == (p_fm->chan_cnt - 1))
        {
            p_fm->index_cur = 0;
        }
        else
        {
            p_fm->index_cur++;
        }
    }

    else // Previous channel. Index should subtract one or roll over to the last.
    {
        if (p_fm->index_cur == 0)
        {
            p_fm->index_cur = p_fm->chan_cnt - 1;
        }
        else
        {
            p_fm->index_cur--;
        }
    }

    // Tune to the new station.
    kt0810_set_station (*(p_fm->p_chan_list + p_fm->index_cur));
}

///----------------------------------------------------------------------------
/// Check if the key is a valid one to do some operation controlling FM.
///
/// @return 0: false. 1: true.
/// @date 2013/01/10
///----------------------------------------------------------------------------
uint8 fm_is_control_key (uint8 key_val)
{
    uint8 i;
    uint8 cnt;

    cnt = sizeof(fm_key_map)/sizeof(fm_key_map_t);

    // Just quit if no valid key passed in.
    if (key_val == KEY_VALUE_NONE)
    {
        // REVISIT!!!
        // The debug message here could be anoyed here. Wipe it off later.
        dbprintf ("None fm key\n");
        return 0;
    }

    // Search through the fm key mapping table to find the key-action pair.
    for (i=0; i<cnt; i++)
    {
        if (key_val == fm_key_map[i].key)
        {
            break;
        }
    }

    if (i==cnt)
    {
        dbprintf ("Invalid fm control key %bx\n", key_val);
        return 0;
    }
    else
    {
        dbprintf ("A valid fm control key %bx\n", key_val);
        return 1;
    }    
}

///----------------------------------------------------------------------------
/// FM control based on the key event.
///
/// This layer will control FM IC to do a valid job. Other stuff like UI update,
/// it should implemented seperately. So we should have a higher level entry for
/// fm module like a fm task to include this control routine and ui stuff.
///
/// @param uint8 key_val: key event
///
/// @date 2013/01/08
///----------------------------------------------------------------------------   
void fm_control (uint8 key_val)
{
    uint8 i;
    uint8 cnt;
    fm_action_t fm_action;

    // Check if it is a valid fm control key.
    if (!fm_is_control_key(key_val))
    {
        return;
    }

    // Search through the fm key mapping table to find the key-action pair.
    cnt = sizeof(fm_key_map)/sizeof(fm_key_map_t);

    for (i=0; i<cnt; i++)
    {
        if (key_val == fm_key_map[i].key)
        {
            break;
        }
    }

    // We've find a valid key-action pair if we are here.
    fm_action = fm_key_map[i].fm_action;
    dbprintf ("FM action is %Bx\n", fm_action);

    switch (fm_action)
    {
        case FM_ACTION_SEEK_UP:
            kt0810_seek(1);
            break;

        case FM_ACTION_SEEK_DOWN:
            kt0810_seek(0);
            break;

        case FM_ACTION_TUNE_STEP_UP:
            kt0810_tune_step(1);
            break;

        case FM_ACTION_TUNE_STEP_DOWN:
            kt0810_tune_step(0);
            break;

        case FM_ACTION_TUNE_CHANNEL_NEXT:
            fm_tune_chan(0);
            break;

        case FM_ACTION_TUNE_CHANNEL_PREV:
            fm_tune_chan(1);
            break;

        case FM_ACTION_VOL_UP:
            kt0810_vol_change(1);
            break;

        case FM_ACTION_VOL_DOWN:
            kt0810_vol_change(0);
            break;

        default:
            dbprintf ("No action defined for %bx\n", fm_action);
            break;
    }
}

///----------------------------------------------------------------------------
/// fm task
///
/// @date 2013/01/10
///----------------------------------------------------------------------------
void fm_task(void)
{
    uint8 key;

    // Stop the play system


    // Run a station first.
    fm_start();

    while(1)
    {
        key = tca8418_get_real_key();
        
        // If the key is a valid key to control FM.
        if (fm_is_control_key (key))
        {
            fm_control (key);
        }
        // If the key is a key to switch category.
        else if (tca8418_is_category_key(key))
        {
            break;
        }
        // Other invalid keys. Do nothing.
        else
        {
            continue;
        }
    }

    // Prepare to switch to play task.
}

///----------------------------------------------------------------------------
/// fm module test routine
///----------------------------------------------------------------------------
#if (FM_TEST == FEATURE_ON)

void fm_test (void)
{
    uint8 key;

    fm_start();

    while (1)
    {
        if (key = tca8418_get_real_key())
        {
            fm_control (key);    
        }
    }
}

#endif