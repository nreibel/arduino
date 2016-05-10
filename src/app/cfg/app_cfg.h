#ifndef SRC_APP_CFG_APP_CFG_H_
#define SRC_APP_CFG_APP_CFG_H_

#define SAVE_PRESET_DELAY 2000
#define LED_BLINK_DELAY   200
#define NUMBER_OF_BLINKS  4

#define WHAMMY_CHANNEL_BYPASS_OFFSET  21
#define WHAMMY_CHORDS_MODE_OFFSET     42

typedef enum {
	Whammy_2_Oct_Up = 0,
	Whammy_Oct_Up,
	Whammy_5th_Up,
	Whammy_4th_Up,
	Whammy_2nd_Down,
	Whammy_4th_Down,
	Whammy_5th_Down,
	Whammy_Oct_Down,
	Whammy_2_Oct_Down,
	Whammy_Dive_Bomb,
	Detune_Deep,
	Detune_Shallow,
	Harmony_2nd_3rd_Up,
	Harmony_b3rd_3rd_Up,
	Harmony_3th_4th_Up,
	Harmony_4th_5th_Up,
	Harmony_5th_6th_Up,
	Harmony_5th_7th_Up,
	Harmony_4th_3rd_Down,
	Harmony_5th_4th_Down,
	Harmony_Oct_Up_Oct_Down,
	WHAMMY_MAX_MODE
} WhammyMode;


#endif /* SRC_APP_CFG_APP_CFG_H_ */
