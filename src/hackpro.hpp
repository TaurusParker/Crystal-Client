#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <vector>
#include "imgui-cocos.hpp"
#include "HitboxNode.hpp"
#include "imgui.h"
#include <Geode/fmod/fmod.hpp>
#include <Geode/fmod/fmod.h>
#include <fstream>
#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <thread>
#include <chrono>
#include <numbers>
#include <Geode/utils/fetch.hpp>
#include <dirent.h>
#include "amethyst.hpp"

bool purchase = false;
bool water = false;
cocos2d::CCScene* gui;
ImGuiNode* node;

USE_GEODE_NAMESPACE();

using namespace tulip;
using namespace geode::cocos;

// StartPos Switcher
std::vector<std::pair<StartPosObject*, CCPoint>> g_startPoses;
int g_startPosIndex;
CCLabelBMFont* g_startPosText;
bool g_toReset;

int completepos;

// Checkpoint Switcher
bool cpswitch;
std::vector<PlayerCheckpoint*> g_checkpointsIG;
std::vector<CCPoint> g_checkpointsPos;
int g_checkpointIndex;
bool load;
CCLabelBMFont* g_checkpointText;
CCSprite* CPrightButton;
CCSprite* CPleftButton;

// Noclip Accuracy
static float frames, noclipped_frames = 0;
bool completed = false;
static float limitframes = 0;
CCLabelBMFont* text;
float percent;
bool lenient;
float killaccuracy;
bool finished;
bool resetaccuracy;

// Noclip Deaths
static int noclip_deaths = 0;
CCLabelBMFont* font;
int deathwait;

// Respawn
int smoothOut = 0;

// Practice Orb Bugfix
std::vector<GameObject*> g_activated_objects;
std::vector<GameObject*> g_activated_objects_p2; // amazing
std::vector<std::pair<size_t, size_t>> g_orbCheckpoints;
bool practiceOrb;

// Smart Start Pos
std::vector<GameObject*> gravityPortals;
std::vector<GameObject*> gamemodePortals;
std::vector<GameObject*> mirrorPortals;
std::vector<GameObject*> miniPortals;
std::vector<GameObject*> dualPortals;
std::vector<GameObject*> speedChanges;
std::vector<StartPosObject*> SPs;
std::vector<bool> willFlip;

bool hack;

// The Hacks
std::vector<const char*> playerHacks = { // player hacks comments are wrong
"Noclip Player 1", //playerBools[0]
"Noclip Player 2", //playerBools[1]
"Rainbow Icon P1", //playerBools[2]
"Rainbow Icon P2", //playerBools[3]
"Rainbow Wave Trail", //playerBools[4]
"Same Color Dual", //playerBools[5]
"Show Hitboxes on Death", //playerBools[6]
"Show only Hitboxes", //playerBools[7]
"Show Hitbox trail", //playerBools[8]
"Show Hitboxes in Editor", //playerBools[9]
"Disable Death Effect", //playerBools[10]
"Disable Particles", //playerBools[11]
"Disable Progressbar", //playerBools[12]
"Accurate Percentage", //playerBools[13]
"No Wave Pulse", //playerBools[14]
"Hide Attempts", //playerBools[15]
"Hide Attempts in Practice Mode", //playerBools[16]
"Practice Music Hack", //playerBools[17]
"Ignore ESC", //playerBools[18]
"Confirm Quit", //playerBools[19]
"Auto LDM", //playerBools[20]
"Instant Death Respawn", //playerBools[21]
"Flipped Dual Controls", //playerBools[22]
"Mirrored Dual Controls", //playerBools[23]
"StartPos Switcher", //playerBools[24]
"Respawn Bug Fix", //playerBools[25]
"Practice Orb Bugfix", //playerBools[26]
"Checkpoint Switcher", //playerBools[27]
"Solid Wave Trail",//playerBools[28]
"Frame Stepper", //playerBools[29]
"Load from Last Checkpoint", //playerBools[30]
"Auto Reset", //playerBools[31]
"Invisible Player", //playerBools[32]
"No Trail", //playerBools[33]
"No Glow", //playerBools[34]
"No Spikes", //playerBools[35]
"No Mirror", //playerBools[36]
"Force Dont Fade", //playerBools[37]
"Force Dont Enter", //playerBools[38]
"Layout Mode", //playerBools[39]
"AutoClicker", //playerBools[40]
"Smart StartPos" //playerBools[41]
};
bool playerBools[45] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,};
std::vector<const char*> globalHacks = { 
    "FPS Bypass",
    "Speedhack",
    "Safe Mode",
    "No Transition",
    "No Rotation",
    "Mute on Unfocus",
};
bool globalBools[30] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, };
std::vector<const char*> bypassHacks = { 
    "Anticheat Bypass", //d
    "Unlock All", //d
    "Object Limit Bypass", //d
    "Custom Object Object Limit Bypass", //d
    "Custom Object Limit Bypass", //d
    "Verify Bypass", //d
    "Copy Bypass", //d
    "Slider Bypass", //d
    "Editor Zoom Bypass", //d
    "Instant Complete", //d
};
bool bypassBools[30] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, };
std::vector<const char*> guiHacks = { 
    "Custom Message", //d (still need text input)
    "FPS Display", //d
    "CPS and Clicks", //d
    "Jumps", //d
    "Cheat Indicator", //d
    "Last Death", //d
    "Attempts Display", //d
    "Best Run", //d
    "Run From", //d
    "Noclip Accuracy", //d (still need reset accuracy)
    "Noclip Deaths", //d
    "Total Attempts", //d
    "Level Name and ID", //d
    "Hide ID", //d
    "Show Author", //d
};
bool guiBools[30] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, };

float speedhack = 1;
bool noclip;
bool deathEffect;
bool particles;
bool progressBar;
bool accpercentage;
bool nopulse;
float pulse = 2;
bool freecopy;
bool unlockall;
bool hideatts;
bool hidepracatts;
bool safe;
bool pracmusic = true;
bool rainP1;
bool rainP2;
bool rainP1wave;
bool rainP2wave;
bool respawn;
char speedhack2;
bool startpos;
bool slider = true;
bool objlimit = true;
bool customobjlimit = true;
bool customlimit = true;
bool invis;
bool verify;
bool anticheat;
bool moon;
bool speed;
bool autoclick;
int clickframe;
int clickRel;
int clickPush;
bool drawTrail = true;
bool noLimitTrail = false;
bool onDeath = true;
bool onlyHitbox;
bool restartWithR;
bool fps;
bool levelinfo;
bool clicks;
bool jumps;
bool editorkey;
bool plugin;
bool zoomhack;
float zoom = 1;
float v_tracking;
bool layout;
bool ldm;
bool randomLoop;
bool cheater;
bool ignoreESC;
bool noglow;
bool mirror;
bool anykey;
bool solidwave;
bool samedual;
bool complete;
bool uncomplete;
bool framestep;
bool stepready = false;
bool noTrans;
bool noRot;
bool totalAtts;
bool lastDeath;
bool instantdeath;
bool dualcontrol;
bool notrail;
bool nospike;
bool forcefade;
bool forceenter;
bool accuracy;
bool unfocusmute;
bool freeze;
bool autoKill;
int autoresetnum;
bool autoreset;
char message[64];
bool customMessage;
bool clickreset;
bool nodim;
bool noclipDeaths;
bool author;
bool hideID;
float rainbowspeed = 5;
bool scrollzoom;
bool noclipP1;
bool noclipP2;
bool lastCheckpoint;
int customR1;
int customG1;
int customB1;
int customR2;
int customG2;
int customB2;
bool customColor;
bool confirmQuit;
bool shouldQuit;
bool overrideFL;
bool settingRefresh;
bool instantmaybe;
bool macroStatus;
bool classicspeed;
std::string bad;
bool pausecountdown;
bool freezeCount;
int countdown;
CCLabelBMFont* g_pauseCount;
bool inputmirror;

cocos2d::_ccColor3B col;
cocos2d::_ccColor3B colInverse;

int waitframe;

// Display
int leftDisplay;

bool attempts;
CCLabelBMFont* g_atts;
bool percentcustom;
float percentXpos = 100;
float percentYpos = 100;
float percentScale = 0.25;
float percentOpac = 255;
int clickscount;

CCLabelBMFont* g_run;
CCLabelBMFont* g_bestRun;
bool currentRun;
bool bestRun;
int bestEnd = 0;
int bestStart = 0;
int bestEnd2 = 0;
int bestStart2 = 0;
std::string display = "Best Run: " + std::to_string(bestStart) + " to " + std::to_string(bestEnd);

CCLabelBMFont* g_levelInfo;
CCLabelBMFont* g_clicks;
CCLabelBMFont* g_jumps;
CCLabelBMFont* g_cheating;
CCLabelBMFont* g_tatts;
CCLabelBMFont* g_death;
CCLabelBMFont* g_message;
CCLabelBMFont* g_macro;

CCSprite* rightButton;
CCSprite* leftButton;

// Rendering
bool showing = false;
bool dev;
bool initimgui;

static inline bool s_onlyHitboxes = false;
static inline bool s_showOnDeath = false;
static inline bool s_drawOnDeath = false;
static inline bool s_noLimitTrail = false;
static inline tulip::HitboxNode* s_drawer = nullptr;
bool editorHitbox;
bool turnonHitbox;

void fps_shower_init();

bool clickready = true;
bool clickready2 = true;
cocos2d::ccColor3B secondary;
cocos2d::ccColor3B primary;

// Amethyst
bool record;
bool replay;
bool frameAccuracy;
char macroname[25];
std::vector<float> xpos;
std::vector<float> ypos;
std::vector<float> rot;
std::vector<bool> push;
bool clickBot;
bool pushing;

int theme = 1;

// Keybindings
std::vector<const char*> bindMods = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
std::vector<const char*> bindKeys = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Up Arrow", "Down Arrow", "Left Arrow", "Right Arrow", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Backspace", "Tab", "Enter", "Shift", "Control", "Alt", "Caps Lock", "Escape", "Space" };
std::vector<cocos2d::enumKeyCodes> keyCodes = {
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_ArrowUp,
        KEY_ArrowDown,
        KEY_ArrowLeft,
        KEY_ArrowRight,
        KEY_Zero,
        KEY_One,
        KEY_Two,
        KEY_Three,
        KEY_Four,
        KEY_Five,
        KEY_Six,
        KEY_Seven,
        KEY_Eight,
        KEY_Nine,
        KEY_Backspace,
        KEY_Tab,
        KEY_Enter,
        KEY_Shift,
        KEY_Control,
        KEY_Alt,
        KEY_CapsLock,
        KEY_Escape,
        KEY_Space
};

const char* keybindings[48];
const char* modbindings[10];

std::vector<std::pair<cocos2d::enumKeyCodes, int>> bindvector;

std::vector<cocos2d::enumKeyCodes> activeKeys;
std::vector<int> activeMods;

static int currentKey;
static int currentMod;
static ImGuiComboFlags flags = 0;
const char* preview = "Pick A Keybind";

int keybinds;

const char* keys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Arrow", "Right Arrow", "Up Arrow", "Down Arrow", "1", "2", "3", "4", "5" };
const char* mods[] = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
static int key_current;
static int mod_current;
const char* keys2[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Arrow", "Right Arrow", "1", "2", "3", "4", "5" };
const char* mods2[] = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
static int key_current2;
static int mod_current2;
const char* keys3[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Arrow", "Right Arrow", "1", "2", "3", "4", "5" };
const char* mods3[] = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
static int key_current3;
static int mod_current3;
const char* keys4[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Arrow", "Right Arrow", "1", "2", "3", "4", "5" };
const char* mods4[] = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
static int key_current4;
static int mod_current4;
const char* keys5[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Arrow", "Right Arrow", "1", "2", "3", "4", "5" };
const char* mods5[] = { "Noclip", "Suicide", "Restart Level", "Speedhack", "Hitbox Viewer", "AutoClicker", "Switcher Left", "Switcher Right", "Player 1", "Player 2", "Place Checkpoint", "Remove Checkpoint" };
static int key_current5;
static int mod_current5;
cocos2d::enumKeyCodes customKey;
cocos2d::enumKeyCodes customKey2;
cocos2d::enumKeyCodes customKey3;
cocos2d::enumKeyCodes customKey4;
cocos2d::enumKeyCodes customKey5;

// Variable Changer
const char* variables[] = { "Level Name", "Level Description", "Level Author", "Attempts", "Jumps", "Percentage" };
static int variable_index;
bool applyVC;
bool FloatString;
char stringVC[64];
float floatVC;

// NONG Switcher
char replaceWith[64];
char replacing[64];
bool EnableNONGLoader;
std::string findsong;
std::string replacesong;
std::vector<std::string> NONGs;
bool manualNONG;
static int item_current_idx = 0;
const char* items[] = { "None", "Empty", "Empty", "Empty", "Empty", "Empty", "Empty" };

std::chrono::time_point<std::chrono::high_resolution_clock> previous_frame, last_update;
float frame_time_sum = 0.f;
int frame_count = 0;
int bypass = 60;
bool applybypass;
double g_fps = 60.0;
double time_since = 0;