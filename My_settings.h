#define PROJ_SCREENMODE TASMODE
//#define PROJ_SCREENMODE MODE_HI_4COLOR
//#define PROJ_SCREENMODE MODE13
//#define PROJ_SCREENMODE MODE13
//#define PROJ_SCREENMODE MODE15
//#define PROJ_SCREENMODE MODE64
//#define PROJ_SCREENMODE MIXMODE
//#define PROJ_SCREENMODE TASMODE // See also SECTION: TASMODE

#define PROJ_FPS 255

// Choose how to use Pokitto's "extra" 4kb of RAM.
// Optional. Can be:
//   HIGH_RAM_OFF   - Needed for USB to work
//   HIGH_RAM_ON    - Simply enable the RAM hardware
//   HIGH_RAM_MUSIC - Enable and move the soud buffers to high RAM
// Default is HIGH_RAM_OFF.
#define PROJ_HIGH_RAM HIGH_RAM_OFF


// ---- SECTION: SOUND ----
#define NUM_CHANNELS 3 // Set to what you need; default is 2.

// Not strictly necessary.
#define PROJ_AUD_FREQ 16000 // Only 'cause it's used in main.cpp
//#define PROJ_AUD_FREQ 8000 // Only 'cause it's used in main.cpp
// #define PROJ_ENABLE_SOUND 0 // Nah 
// #define PROJ_ENABLE_SFX // don't need that
// #define PROJ_ENABLE_SD_MUSIC // nope

// Automatically restart streaming music when it ends.
// Optional. Default is 1.
#define PROJ_STREAM_LOOP 0

// ---- SECTION: TASMODE ----
// These settings only apply to TASMODE

// Choose the maximum amount of sprites that can be drawn per frame.
// Higher values consume more RAM.
// Optional. Default is 100.
#define PROJ_MAX_SPRITES 100

// Choose the height in pixels of each tile
// Higher values consume more RAM.
// Optional. Default is 16.
#define PROJ_TILE_H 16

// Choose the width in pixels of each tile
// Higher values consume more RAM.
// Optional. Default is 16.
#define PROJ_TILE_W 16

// ---- SECTION: MISC ----

// Skips the initial Pokitto/Loader/Volume screens.
// Don't release binaries with this enabled!
// Optional. Can be 0 or 1, default is 0.
#define PROJ_DEVELOPER_MODE 0

// Tells the Tilemap library if tiles are stored in
// 1-tile-per-byte (256 tiles maximum) or
// 2-tiles-per-byte (16 tiles maximum) format.
// Optional. Can be 16 or 256. Default is 16.
#define MAX_TILE_COUNT 256
