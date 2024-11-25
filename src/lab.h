#include "lab_common.h"

// DECLARATIONS #############################################

// todo: move structs from lab_common.h to here

static EventMenu LabMenu_General;
static EventMenu LabMenu_OverlaysHMN;
static EventMenu LabMenu_OverlaysCPU;
static EventMenu LabMenu_InfoDisplayHMN;
static EventMenu LabMenu_InfoDisplayCPU;
static EventMenu LabMenu_CPU;
static EventMenu LabMenu_Record;
static EventMenu LabMenu_Tech;

#define AUTORESTORE_DELAY 20
#define INTANG_COLANIM 10
#define STICK_DEADZONE 0.2
#define TRIGGER_DEADZONE 0.2

#define CPUMASHRNG_MED 35
#define CPUMASHRNG_HIGH 55

typedef struct CPUAction {
    u16 state;                  // state to perform this action. -1 for last
    u8 frameLow;                // first possible frame to perform this action
    u8 frameHi;                 // last possible frame to perfrom this action
    s8 stickX;                  // left stick X value
    s8 stickY;                  // left stick Y value
    s8 cstickX;                 // c stick X value
    s8 cstickY;                 // c stick Y value
    int input;                  // button to input
    unsigned char isLast : 1;   // flag to indicate this was the final input
    unsigned char stickDir : 3; // 0 = none, 1 = towards opponent, 2 = away from opponent, 3 = forward, 4 = backward
} CPUAction;

enum stick_dir
{
    STCKDIR_NONE,
    STCKDIR_TOWARD,
    STCKDIR_AWAY,
    STCKDIR_FRONT,
    STCKDIR_BACK,
    STICKDIR_RDM,
};

enum hit_kind
{
    HITKIND_DAMAGE,
    HITKIND_SHIELD,
};

enum custom_asid_groups
{
    ASID_ACTIONABLE = 1000,
    ASID_ACTIONABLEGROUND,
    ASID_ACTIONABLEAIR,
    ASID_DAMAGEAIR,
    ASID_CROUCH,
    ASID_ANY,
};

typedef enum lab_state {
    LabState_Normal,
    LabState_SetCPUPosition,
} LabState;

// ACTIONS #################################################

// CPU Action Definitions
static CPUAction Lab_CPUActionShield[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
    },
    {
        .state     = ASID_GUARDREFLECT,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionGrab[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_A | PAD_TRIGGER_R,
        .isLast    = 1,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_TRIGGER_Z,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionUpB[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_KNEEBEND,
        .stickY    = 127,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
    },
    {
        .state     = ASID_ACTIONABLE,
        .stickY    = 127,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionSideBToward[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLE,
        .stickX    = 127,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },
    -1,
};
static CPUAction Lab_CPUActionSideBAway[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLE,
        .stickX    = 127,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
        .stickDir  = STCKDIR_AWAY,
    },
    -1,
};
static CPUAction Lab_CPUActionDownB[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
    },
    {
        .state     = ASID_ACTIONABLE,
        .stickY    = -127,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionNeutralB[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_GUARD,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
    },

    {
        .state     = ASID_ACTIONABLE,
        .input     = PAD_BUTTON_B,
        .isLast    = 1,
    },
    -1,
};

// We buffer this for a single frame.
// For some reason spotdodge is only possible frame 2 when floorhugging
// an attack that would have otherwise knocked you into the air without knockdown.
// This doesn't occur with rolls for some reason.
static CPUAction Lab_CPUActionSpotdodge[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .stickY    = -127,
        .input     = PAD_TRIGGER_R,
    },
    {
        .state     = ASID_GUARDREFLECT,
        .stickY    = -127,
        .input     = PAD_TRIGGER_R,
    },
    {
        .state     = ASID_ESCAPE,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionRollAway[] = {
    {
        .state     = ASID_GUARD,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STCKDIR_AWAY,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_TRIGGER_R,
    },
    {
        .state     = ASID_GUARDREFLECT,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STCKDIR_AWAY,
    },
    -1,
};
static CPUAction Lab_CPUActionRollTowards[] = {
    {
        .state     = ASID_GUARD,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_TRIGGER_R,
    },
    {
        .state     = ASID_GUARDREFLECT,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },
    -1,
};
static CPUAction Lab_CPUActionRollRandom[] = {
    {
        .state     = ASID_GUARD,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STICKDIR_RDM,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_TRIGGER_R,
    },
    {
        .state     = ASID_GUARDREFLECT,
        .stickX    = 127,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
        .stickDir  = STICKDIR_RDM,
    },
    -1,
};
static CPUAction Lab_CPUActionNair[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_BUTTON_A,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionFair[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .cstickX   = 127,
        .isLast    = 1,
        .stickDir  = 3,
    },
    -1,
};
static CPUAction Lab_CPUActionDair[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .cstickY   = -127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionBair[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .cstickX   = 127,
        .isLast    = 1,
        .stickDir  = 4,
    },
    -1,
};
static CPUAction Lab_CPUActionUair[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .cstickY   = 127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionJump[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionJumpFull[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_KNEEBEND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionJumpAway[] = {
    {
        .state     = ASID_ACTIONABLEAIR,
        .stickX    = 127,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
        .stickDir  = STCKDIR_AWAY,
    },

    -1,
};
static CPUAction Lab_CPUActionJumpTowards[] = {
    {
        .state     = ASID_ACTIONABLEAIR,
        .stickX    = 127,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },

    -1,
};
static CPUAction Lab_CPUActionJumpNeutral[] = {
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_BUTTON_X,
        .isLast    = 1,
        .stickDir  = STCKDIR_NONE,
    },

    -1,
};
static CPUAction Lab_CPUActionAirdodge[] = {
    // wiggle out if we are in tumble
    {
        .state     = ASID_DAMAGEAIR,
        .stickX    = 127,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_TRIGGER_R,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionFFTumble[] = {
    {
        .state     = ASID_DAMAGEAIR,
        .stickY    = -127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionFFWiggle[] = {
    {
        .state     = ASID_DAMAGEAIR,
        .stickX    = 127,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .stickY    = -127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionJab[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_A,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionFTilt[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .stickX    = 80,
        .input     = PAD_BUTTON_A,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },
    -1,
};
static CPUAction Lab_CPUActionUTilt[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .stickY    = 80,
        .input     = PAD_BUTTON_A,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionDTilt[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .stickY    = -80,
        .input     = PAD_BUTTON_A,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionUSmash[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .cstickY   = 127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionDSmash[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .cstickY   = -127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionFSmash[] = {
    {
        .state     = ASID_ACTIONABLEGROUND,
        .cstickX   = 127,
        .isLast    = 1,
        .stickDir  = STCKDIR_TOWARD,
    },
    -1,
};
static CPUAction Lab_CPUActionUpSmashOOS[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_KNEEBEND,
        .cstickY   = 127,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionWavedashAway[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_TRIGGER_L,
        .stickY    = -45,
        .stickX    = 65,
        .stickDir  = STCKDIR_AWAY,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionWavedashTowards[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_TRIGGER_L,
        .stickY    = -45,
        .stickX    = 64,
        .stickDir  = STCKDIR_TOWARD,
        .isLast    = 1,
    },
    -1,
};
static CPUAction Lab_CPUActionWavedashDown[] = {
    {
        .state     = ASID_GUARD,
        .input     = PAD_TRIGGER_R | PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEGROUND,
        .input     = PAD_BUTTON_X,
    },
    {
        .state     = ASID_ACTIONABLEAIR,
        .input     = PAD_TRIGGER_L,
        .stickY    = -80,
        .isLast    = 1,
    },
    -1,
};

static CPUAction *Lab_CPUActions[] = {
    0,
    &Lab_CPUActionShield,
    &Lab_CPUActionGrab,
    &Lab_CPUActionUpB,
    &Lab_CPUActionSideBToward,
    &Lab_CPUActionSideBAway,
    &Lab_CPUActionDownB,
    &Lab_CPUActionNeutralB,
    &Lab_CPUActionSpotdodge,
    &Lab_CPUActionRollAway,
    &Lab_CPUActionRollTowards,
    &Lab_CPUActionRollRandom,
    &Lab_CPUActionNair,
    &Lab_CPUActionFair,
    &Lab_CPUActionDair,
    &Lab_CPUActionBair,
    &Lab_CPUActionUair,
    &Lab_CPUActionJump,
    &Lab_CPUActionJumpFull,
    &Lab_CPUActionJumpAway,
    &Lab_CPUActionJumpTowards,
    &Lab_CPUActionJumpNeutral,
    &Lab_CPUActionAirdodge,
    &Lab_CPUActionFFTumble,
    &Lab_CPUActionFFWiggle,
    &Lab_CPUActionJab,
    &Lab_CPUActionFTilt,
    &Lab_CPUActionUTilt,
    &Lab_CPUActionDTilt,
    &Lab_CPUActionUSmash,
    &Lab_CPUActionDSmash,
    &Lab_CPUActionFSmash,
    &Lab_CPUActionUpSmashOOS,
    &Lab_CPUActionWavedashAway,
    &Lab_CPUActionWavedashTowards,
    &Lab_CPUActionWavedashDown
};

enum CPU_ACTIONS
{
    CPUACT_NONE,
    CPUACT_SHIELD,
    CPUACT_GRAB,
    CPUACT_UPB,
    CPUACT_SIDEBTOWARD,
    CPUACT_SIDEBAWAY,
    CPUACT_DOWNB,
    CPUACT_NEUTRALB,
    CPUACT_SPOTDODGE,
    CPUACT_ROLLAWAY,
    CPUACT_ROLLTOWARDS,
    CPUACT_ROLLRDM,
    CPUACT_NAIR,
    CPUACT_FAIR,
    CPUACT_DAIR,
    CPUACT_BAIR,
    CPUACT_UAIR,
    CPUACT_SHORTHOP,
    CPUACT_FULLHOP,
    CPUACT_JUMPAWAY,
    CPUACT_JUMPTOWARDS,
    CPUACT_JUMPNEUTRAL,
    CPUACT_AIRDODGE,
    CPUACT_FFTUMBLE,
    CPUACT_FFWIGGLE,
    CPUACT_JAB,
    CPUACT_FTILT,
    CPUACT_UTILT,
    CPUACT_DTILT,
    CPUACT_USMASH,
    CPUACT_DSMASH,
    CPUACT_FSMASH,
    CPUACT_USMASHOOS,
    CPUACT_WAVEDASH_AWAY,
    CPUACT_WAVEDASH_TOWARDS,
    CPUACT_WAVEDASH_DOWN,

    CPUACT_COUNT
};

static u8 CPUCounterActionsGround[] = {CPUACT_NONE, CPUACT_SPOTDODGE, CPUACT_SHIELD, CPUACT_GRAB, CPUACT_UPB, CPUACT_SIDEBTOWARD, CPUACT_SIDEBAWAY, CPUACT_DOWNB, CPUACT_NEUTRALB, CPUACT_USMASH, CPUACT_DSMASH, CPUACT_FSMASH, CPUACT_ROLLAWAY, CPUACT_ROLLTOWARDS, CPUACT_ROLLRDM, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR, CPUACT_JAB, CPUACT_FTILT, CPUACT_UTILT, CPUACT_DTILT, CPUACT_SHORTHOP, CPUACT_FULLHOP, CPUACT_WAVEDASH_AWAY, CPUACT_WAVEDASH_TOWARDS, CPUACT_WAVEDASH_DOWN};

static u8 CPUCounterActionsAir[] = {CPUACT_NONE, CPUACT_AIRDODGE, CPUACT_JUMPAWAY, CPUACT_JUMPTOWARDS, CPUACT_JUMPNEUTRAL, CPUACT_UPB, CPUACT_SIDEBTOWARD, CPUACT_SIDEBAWAY, CPUACT_DOWNB, CPUACT_NEUTRALB, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR, CPUACT_FFTUMBLE, CPUACT_FFWIGGLE};

static u8 CPUCounterActionsShield[] = {CPUACT_NONE, CPUACT_GRAB, CPUACT_SHORTHOP, CPUACT_FULLHOP, CPUACT_SPOTDODGE, CPUACT_ROLLAWAY, CPUACT_ROLLTOWARDS, CPUACT_ROLLRDM, CPUACT_USMASHOOS, CPUACT_UPB, CPUACT_DOWNB, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR, CPUACT_WAVEDASH_AWAY, CPUACT_WAVEDASH_TOWARDS, CPUACT_WAVEDASH_DOWN};

static char *LabValues_CounterGround[] = {"None", "Spotdodge", "Shield", "Grab", "Up B", "Side B Toward", "Side B Away", "Down B", "Neutral B", "Up Smash", "Down Smash", "Forward Smash", "Roll Away", "Roll Towards", "Roll Random", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air", "Jab", "Forward Tilt", "Up Tilt", "Down Tilt", "Short Hop", "Full Hop", "Wavedash Away", "Wavedash Towards", "Wavedash Down"};
static char *LabValues_CounterAir[] = {"None", "Airdodge", "Jump Away", "Jump Towards", "Jump Neutral", "Up B", "Side B Toward", "Side B Away", "Down B", "Neutral B", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air", "Tumble Fastfall", "Wiggle Fastfall"};
static char *LabValues_CounterShield[] = {"None", "Grab", "Short Hop", "Full Hop", "Spotdodge", "Roll Away", "Roll Towards", "Roll Random", "Up Smash", "Up B", "Down B", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air", "Wavedash Away", "Wavedash Towards", "Wavedash Down"};

// MENUS ###################################################

// MAIN MENU --------------------------------------------------------------

enum lab_option
{
    OPTLAB_GENERAL_OPTIONS,
    OPTLAB_CPU_OPTIONS,
    OPTLAB_RECORD_OPTIONS,
    OPTLAB_INFODISP_HMN,
    OPTLAB_INFODISP_CPU,
    OPTLAB_HELP,
    OPTLAB_EXIT,

    OPTLAB_COUNT
};

static char *LabOptions_OffOn[] = {"Off", "On"};
static EventOption LabOptions_Main[OPTLAB_COUNT] = {
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_General,
        .option_name = "General",
        .desc = "Toggle player percent, overlays,\nframe advance, and camera settings.",
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_CPU,
        .option_name = "CPU Options",
        .desc = "Configure CPU behavior.",
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_Record,
        .option_name = "Recording",
        .desc = "Record and playback inputs.",
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_InfoDisplayHMN,
        .option_name = "HMN Info Display",
        .desc = "Display various game information onscreen.",
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_InfoDisplayCPU,
        .option_name = "CPU Info Display",
        .desc = "Display various game information onscreen.",
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Help",
        .desc = "D-Pad Left - Load State\nD-Pad Right - Save State\nD-Pad Down - Move CPU\nHold R in the menu for turbo.",
        .onOptionChange = Lab_Exit,
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Exit",
        .desc = "Return to the Event Select Screen.",
        .onOptionSelect = Lab_Exit,
    },
};

static EventMenu LabMenu_Main = {
    .name = "Main Menu",
    .option_num = sizeof(LabOptions_Main) / sizeof(EventOption),
    .options = &LabOptions_Main,
};

// GENERAL MENU --------------------------------------------------------------

enum gen_option
{
    OPTGEN_FRAME,
    OPTGEN_FRAMEBTN,
    OPTGEN_HMNPCNT,
    OPTGEN_HMNPCNTLOCK,
    OPTGEN_MODEL,
    OPTGEN_HIT,
    OPTGEN_COLL,
    OPTGEN_CAM,
    OPTGEN_OVERLAYS_HMN,
    OPTGEN_OVERLAYS_CPU,
    OPTGEN_HUD,
    OPTGEN_DI,
    OPTGEN_INPUT,
    OPTGEN_SPEED,
    OPTGEN_STALE,
    OPTGEN_TAUNT,

    OPTGEN_COUNT
};

static char *LabOptions_CamMode[] = {"Normal", "Zoom", "Fixed", "Advanced"};
static char *LabOptions_ShowInputs[] = {"Off", "HMN", "HMN and CPU"};
static char *LabOptions_FrameAdvButton[] = {"L", "Z", "X", "Y"};

static float LabOptions_GameSpeeds[] = {1.0, 2.0/3.0, 1.0/2.0, 1.0/4.0};
static char *LabOptions_GameSpeedText[] = {"1", "2/3", "1/2", "1/4"};

static EventOption LabOptions_General[OPTGEN_COUNT] = {
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_OffOn) / 4,
        .option_name = "Frame Advance",
        .desc = "Enable frame advance. Press to advance one\nframe. Hold to advance at normal speed.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeFrameAdvance,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_FrameAdvButton) / 4,
        .option_name = "Frame Advance Button",
        .desc = "Choose which button will advance the frame",
        .option_values = LabOptions_FrameAdvButton,
        .onOptionChange = Lab_ChangeFrameAdvanceButton,
        .disable = 1,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 999,
        .option_name = "Player Percent",
        .desc = "Adjust the player's percent.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangePlayerPercent,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Lock Player Percent",
        .desc = "Locks Player percent to current percent",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangePlayerLockPercent,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_val = 1,
        .option_name = "Model Display",
        .desc = "Toggle player and item model visibility.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeModelDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Fighter Collision",
        .desc = "Toggle hitbox and hurtbox visualization.\nYellow = hurt, red = hit, purple = grab, \nwhite = trigger, green = reflect,\nblue = shield/absorb.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeHitDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Environment Collision",
        .desc = "Toggle environment collision visualization.\nAlso displays the players' ECB (environmental \ncollision box).",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeEnvCollDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_CamMode) / 4,
        .option_name = "Camera Mode",
        .desc = "Adjust the camera's behavior.\nIn advanced mode, use C-Stick while holding\nA/B/Y to pan, rotate and zoom, respectively.",
        .option_values = LabOptions_CamMode,
        .onOptionChange = Lab_ChangeCamMode,
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_OverlaysHMN,
        .option_name = "HMN Color Overlays",
        .desc = "Set up color indicators for\n. action states.",
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_OverlaysCPU,
        .option_name = "CPU Color Overlays",
        .desc = "Set up color indicators for\n. action states.",
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_val = 1,
        .option_name = "HUD",
        .desc = "Toggle player percents and timer visibility.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeHUD,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "DI Display",
        .desc = "Display knockback trajectories.\nUse frame advance to see the effects of DI\nin realtime during hitstop.",
        .option_values = LabOptions_OffOn,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_ShowInputs) / sizeof(*LabOptions_ShowInputs),
        .option_name = "Input Display",
        .desc = "Display player inputs onscreen.",
        .option_values = LabOptions_ShowInputs,
        .onOptionChange = Lab_ChangeInputDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_GameSpeedText) / sizeof(*LabOptions_GameSpeedText),
        .option_name = "Game Speed",
        .desc = "Change how fast the game engine runs.",
        .option_values = LabOptions_GameSpeedText,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_val = 1,
        .option_name = "Move Staling",
        .desc = "Toggle the staling of moves. Attacks become \nweaker the more they are used.",
        .option_values = LabOptions_OffOn,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_val = 1,
        .option_name = "Disable Taunt",
        .desc = "Disable the taunt button (D-pad up)",
        .option_values = LabOptions_OffOn,
    },
};
static EventMenu LabMenu_General = {
    .name = "General",
    .option_num = sizeof(LabOptions_General) / sizeof(EventOption),
    .options = &LabOptions_General,
};

// INFO DISPLAY MENU --------------------------------------------------------------

enum infdisp_rows
{
    INFDISP_NONE,
    INFDISP_POS,
    INFDISP_STATE,
    INFDISP_FRAME,
    INFDISP_SELFVEL,
    INFDISP_KBVEL,
    INFDISP_TOTALVEL,
    INFDISP_ENGLSTICK,
    INFDISP_SYSLSTICK,
    INFDISP_ENGCSTICK,
    INFDISP_SYSCSTICK,
    INFDISP_ENGTRIGGER,
    INFDISP_SYSTRIGGER,
    INFDISP_LEDGECOOLDOWN,
    INFDISP_INTANGREMAIN,
    INFDISP_HITSTOP,
    INFDISP_HITSTUN,
    INFDISP_SHIELDHEALTH,
    INFDISP_SHIELDSTUN,
    INFDISP_GRIP,
    INFDISP_ECBLOCK,
    INFDISP_ECBBOT,
    INFDISP_JUMPS,
    INFDISP_WALLJUMPS,
    INFDISP_JAB,
    INFDISP_LINE,
    INFDISP_BLASTLR,
    INFDISP_BLASTUD,

    INFDISP_COUNT
};

enum info_disp_option
{
    OPTINF_PRESET,
    OPTINF_SIZE,
    OPTINF_ROW1,
    OPTINF_ROW2,
    OPTINF_ROW3,
    OPTINF_ROW4,
    OPTINF_ROW5,
    OPTINF_ROW6,
    OPTINF_ROW7,
    OPTINF_ROW8,

    OPTINF_COUNT
};

#define OPTINF_ROW_COUNT (OPTINF_COUNT - OPTINF_ROW1)

static char *LabValues_InfoDisplay[INFDISP_COUNT] = {"None", "Position", "State Name", "State Frame", "Velocity - Self", "Velocity - KB", "Velocity - Total", "Engine LStick", "System LStick", "Engine CStick", "System CStick", "Engine Trigger", "System Trigger", "Ledgegrab Timer", "Intangibility Timer", "Hitlag", "Hitstun", "Shield Health", "Shield Stun", "Grip Strength", "ECB Lock", "ECB Bottom", "Jumps", "Walljumps", "Jab Counter", "Line Info", "Blastzone Left/Right", "Blastzone Up/Down"};

static char *LabValues_InfoSizeText[] = {"Small", "Medium", "Large"};
static float LabValues_InfoSizes[] = {0.7, 0.85, 1.0};

static char *LabValues_InfoPresets[] = {"None", "Ledge", "Damage"};
static int LabValues_InfoPresetStates[][OPTINF_ROW_COUNT] = {
    // None
    { 0 },

    // Ledge
    {
        INFDISP_STATE,
        INFDISP_FRAME,
        INFDISP_SYSLSTICK,
        INFDISP_ENGLSTICK,
        INFDISP_INTANGREMAIN,
        INFDISP_ECBLOCK,
        INFDISP_ECBBOT,
        INFDISP_NONE,
    },

    // Damage
    {
        INFDISP_STATE,
        INFDISP_FRAME,
        INFDISP_SELFVEL,
        INFDISP_KBVEL,
        INFDISP_TOTALVEL,
        INFDISP_HITSTOP,
        INFDISP_HITSTUN,
        INFDISP_SHIELDSTUN,
    },
};

// copied from LabOptions_InfoDisplayDefault in Event_Init
static EventOption LabOptions_InfoDisplayHMN[OPTINF_COUNT];
static EventOption LabOptions_InfoDisplayCPU[OPTINF_COUNT];

static EventOption LabOptions_InfoDisplayDefault[OPTINF_COUNT] = {
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoPresets) / 4,
        .option_name = "Display Preset",
        .desc = "Choose between pre-configured selections.",
        .option_values = LabValues_InfoPresets,

        // set as Lab_ChangeInfoPresetHMN/CPU after memcpy.
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoSizeText) / 4,
        .option_val = 1,
        .option_name = "Size",
        .desc = "Change the size of the info display window.\nLarge is recommended for CRT.\nMedium/Small recommended for Dolphin Emulator.",
        .option_values = LabValues_InfoSizeText,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 1",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 2",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 3",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 4",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 5",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 6",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 7",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_InfoDisplay) / 4,
        .option_name = "Row 8",
        .desc = "Adjust what is displayed in this row.",
        .option_values = LabValues_InfoDisplay,
    },
};

static EventMenu LabMenu_InfoDisplayHMN = {
    .name = "HMN Info Display",
    .option_num = sizeof(LabOptions_InfoDisplayHMN) / sizeof(EventOption),
    .options = &LabOptions_InfoDisplayHMN,
};

static EventMenu LabMenu_InfoDisplayCPU = {
    .name = "CPU Info Display",
    .option_num = sizeof(LabOptions_InfoDisplayCPU) / sizeof(EventOption),
    .options = &LabOptions_InfoDisplayCPU,
};

// CPU MENU --------------------------------------------------------------

enum cpu_behave
{
    CPUBEHAVE_STAND,
    CPUBEHAVE_SHIELD,
    CPUBEHAVE_CROUCH,
    CPUBEHAVE_JUMP,

    CPUBEHAVE_COUNT
};

enum cpu_sdi
{
    CPUSDI_RANDOM,
    CPUSDI_NONE,

    CPUSDI_COUNT
};

enum cpu_tdi
{
    CPUTDI_RANDOM,
    CPUTDI_IN,
    CPUTDI_OUT,
    CPUTDI_CUSTOM,
    CPUTDI_RANDOM_CUSTOM,
    CPUTDI_NONE,
    CPUTDI_NUM,

    CPUTDI_COUNT
};

enum cpu_shield_angle
{
    CPUSHIELDANG_NONE,
    CPUSHIELDANG_UP,
    CPUSHIELDANG_TOWARD,
    CPUSHIELDANG_DOWN,
    CPUSHIELDANG_AWAY,

    CPUSHIELDANG_COUNT
};

enum cpu_tech
{
    CPUTECH_RANDOM,
    CPUTECH_NEUTRAL,
    CPUTECH_AWAY,
    CPUTECH_TOWARDS,
    CPUTECH_NONE,

    CPUTECH_COUNT
};

enum cpu_getup
{
    CPUGETUP_RANDOM,
    CPUGETUP_STAND,
    CPUGETUP_AWAY,
    CPUGETUP_TOWARD,
    CPUGETUP_ATTACK,

    CPUGETUP_COUNT
};
enum cpu_state
{
    CPUSTATE_START,
    CPUSTATE_GRABBED,
    CPUSTATE_SDI,
    CPUSTATE_TDI,
    CPUSTATE_TECH,
    CPUSTATE_GETUP,
    CPUSTATE_COUNTER,
    CPUSTATE_RECOVER,
    CPUSTATE_NONE,

    CPUSTATE_COUNT
};

enum cpu_mash
{
    CPUMASH_NONE,
    CPUMASH_MED,
    CPUMASH_HIGH,
    CPUMASH_PERFECT,

    CPUMASH_COUNT
};

enum cpu_inf_shield {
    CPUINFSHIELD_OFF,
    CPUINFSHIELD_UNTIL_HIT,
    CPUINFSHIELD_ON,

    CPUINFSHIELD_COUNT
};

enum cpu_option
{
    OPTCPU_PCNT,
    OPTCPU_LOCKPCNT,
    OPTCPU_TECHOPTIONS,
    OPTCPU_TDI,
    OPTCPU_CUSTOMTDI,
    OPTCPU_SDINUM,
    OPTCPU_SDIDIR,
    OPTCPU_ASDI,
    OPTCPU_SHIELD,
    OPTCPU_SHIELDDIR,
    OPTCPU_INTANG,
    OPTCPU_MASH,
    OPTCPU_BEHAVE,
    OPTCPU_CTRGRND,
    OPTCPU_CTRAIR,
    OPTCPU_CTRSHIELD,
    OPTCPU_CTRFRAMES,
    OPTCPU_CTRHITS,
    OPTCPU_SHIELDHITS,
    OPTCPU_SET_POS,

    OPTCPU_COUNT
};

enum asdi
{
    ASDI_AUTO,
    ASDI_AWAY,
    ASDI_TOWARD,
    ASDI_LEFT,
    ASDI_RIGHT,
    ASDI_UP,
    ASDI_DOWN,

    ASDI_COUNT
};

enum sdi_dir
{
    SDIDIR_RANDOM,
    SDIDIR_AWAY,
    SDIDIR_TOWARD,
    SDIDIR_UP,
    SDIDIR_DOWN,
    SDIDIR_LEFT,
    SDIDIR_RIGHT,

    SDIDIR_COUNT
};

static char *LabValues_Shield[] = {"Off", "On Until Hit", "On"};
static char *LabValues_ShieldDir[] = {"Neutral", "Up", "Towards", "Down", "Away"};
static char *LabValues_CPUBehave[] = {"Stand", "Shield", "Crouch", "Jump"};
static char *LabValues_TDI[] = {"Random", "Inwards", "Outwards", "Custom", "Random Custom", "None"};
static char *LabValues_ASDI[] = {"Auto", "Away", "Towards", "Left", "Right", "Up", "Down"};
static char *LabValues_SDIDir[] = {"Random", "Away", "Towards", "Up", "Down", "Left", "Right"};
static char *LabValues_Tech[] = {"Random", "Neutral", "Away", "Towards", "None"};
static char *LabValues_Getup[] = {"Random", "Stand", "Away", "Towards", "Attack"};
static char *LabValues_GrabEscape[] = {"None", "Medium", "High", "Perfect"};
static char *LabValues_LockCPUPercent[] = {"Off", "On"};

static const EventOption LabOptions_CPU_MoveCPU = {
    .option_kind = OPTKIND_FUNC,
    .option_name = "Move CPU",
    .desc = "Manually set the CPU's position.",
    .onOptionSelect = Lab_StartMoveCPU,
};

static const EventOption LabOptions_CPU_FinishMoveCPU = {
    .option_kind = OPTKIND_FUNC,
    .option_name = "Finish Moving CPU",
    .desc = "Finish setting the CPU's position.",
    .onOptionSelect = Lab_FinishMoveCPU,
};

static EventOption LabOptions_CPU[OPTCPU_COUNT] = {
    {
        .option_kind = OPTKIND_INT,
        .value_num = 999,
        .option_name = "CPU Percent",
        .desc = "Adjust the CPU's percent.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeCPUPercent,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Lock CPU Percent",
        .desc = "Locks CPU percent to current percent",
        .option_values = LabValues_LockCPUPercent,
        .onOptionChange = Lab_ChangeCPULockPercent,
    },
    {
        .option_kind = OPTKIND_MENU,
        .menu = &LabMenu_Tech,
        .option_name = "Tech Options",
        .desc = "Configure CPU Tech Behavior.",
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_TDI) / 4,
        .option_name = "Trajectory DI",
        .desc = "Adjust how the CPU will alter their knockback\ntrajectory.",
        .option_values = LabValues_TDI,
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Custom TDI",
        .desc = "Create custom trajectory DI values for the\nCPU to perform.",
        .onOptionSelect = Lab_SelectCustomTDI,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 8,
        .option_name = "Smash DI Amount",
        .desc = "Adjust how often the CPU will alter their position\nduring hitstop.",
        .option_values = "%d Frames",
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_SDIDir) / 4,
        .option_name = "Smash DI Direction",
        .desc = "Adjust the direction in which the CPU will alter \ntheir position during hitstop.",
        .option_values = LabValues_SDIDir,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_ASDI) / 4,
        .option_name = "ASDI",
        .desc = "Set CPU C-stick ASDI direction",
        .option_values = LabValues_ASDI,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_Shield) / 4,
        .option_val = 1,
        .option_name = "Infinite Shields",
        .desc = "Adjust how shield health deteriorates.",
        .option_values = LabValues_Shield,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_ShieldDir) / 4,
        .option_name = "Shield Angle",
        .desc = "Adjust how CPU angles their shield.",
        .option_values = LabValues_ShieldDir,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Intangibility",
        .desc = "Toggle the CPU's ability to take damage.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Lab_ChangeCPUIntang,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_GrabEscape) / 4,
        .option_val = CPUMASH_NONE,
        .option_name = "Grab Escape",
        .desc = "Adjust how the CPU will attempt to escape\ngrabs.",
        .option_values = LabValues_GrabEscape,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_CPUBehave) / 4,
        .option_name = "Behavior",
        .desc = "Adjust the CPU's default action.",
        .option_values = LabValues_CPUBehave,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_CounterGround) / 4,
        .option_val = 1,
        .option_name = "Counter Action (Ground)",
        .desc = "Select the action to be performed after a\ngrounded CPU's hitstun ends.",
        .option_values = LabValues_CounterGround,
        .onOptionChange = Lab_ChangeCounterAction_Ground,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_CounterAir) / 4,
        .option_val = 4,
        .option_name = "Counter Action (Air)",
        .desc = "Select the action to be performed after an\nairborne CPU's hitstun ends.",
        .option_values = LabValues_CounterAir,
        .onOptionChange = Lab_ChangeCounterAction_Air,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_CounterShield) / 4,
        .option_val = 1,
        .option_name = "Counter Action (Shield)",
        .desc = "Select the action to be performed after the\nCPU's shield is hit.",
        .option_values = LabValues_CounterShield,
        .onOptionChange = Lab_ChangeCounterAction_Shield,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 100,
        .option_name = "Counter After Frames",
        .desc = "Adjust the amount of actionable frames before \nthe CPU counters.",
        .option_values = "%d Frames",
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 100,
        .option_val = 1,
        .option_name = "Counter After Hits",
        .desc = "Adjust the amount of hits taken before the \nCPU counters.",
        .option_values = "%d Hits",
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 100,
        .option_val = 1,
        .option_name = "Counter After Shield Hits",
        .desc = "Adjust the amount of hits the CPU's shield\nwill take before they counter.",
        .option_values = "%d Hits",
    },

    // swapped between LabOptions_CPU_MoveCPU and LabOptions_CPU_FinishMoveCPU
    LabOptions_CPU_MoveCPU,
};

static EventMenu LabMenu_CPU = {
    .name = "CPU Options",
    .option_num = sizeof(LabOptions_CPU) / sizeof(EventOption),
    .options = &LabOptions_CPU,
};

// TECH MENU --------------------------------------------------------------

static int tech_frame_distinguishable[27] = {
    -1, // Mario
     4, // Fox
     6, // Captain Falcon
    -1, // Donkey Kong
    -1, // Kirby
    -1, // Bowser
    -1, // Link
    -1, // Sheik
    -1, // Ness
     3, // Peach
     6, // Popo (Ice Climbers)
     6, // Nana (Ice Climbers)
     7, // Pikachu
    -1, // Samus
    -1, // Yoshi
    -1, // Jigglypuff
    -1, // Mewtwo
    -1, // Luigi
     6, // Marth
    -1, // Zelda
    -1, // Young Link
    -1, // Dr. Mario
     4, // Falco
    -1, // Pichu
    -1, // Game & Watch
    -1, // Ganondorf
    -1, // Roy
};

enum tech_option
{
    OPTTECH_INVISIBLE,
    OPTTECH_SOUND,
    OPTTECH_TECH,
    OPTTECH_TECHINPLACECHANCE,
    OPTTECH_TECHAWAYCHANCE,
    OPTTECH_TECHTOWARDCHANCE,
    OPTTECH_MISSTECHCHANCE,
    OPTTECH_GETUP,
    OPTTECH_GETUPSTANDCHANCE,
    OPTTECH_GETUPAWAYCHANCE,
    OPTTECH_GETUPTOWARDCHANCE,
    OPTTECH_GETUPATTACKCHANCE,

    OPTTECH_COUNT
};


static EventOption LabOptions_Tech[OPTTECH_COUNT] = {
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Tech Invisibility",
        .desc = "Toggle the CPU turning invisible during tech\nanimations.",
        .option_values = LabOptions_OffOn,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_name = "Tech Sound",
        .desc = "Toggle playing a sound when tech is\ndistinguishable.",
        .option_values = LabOptions_OffOn,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_Tech) / 4,
        .option_name = "Tech Option",
        .desc = "Adjust what the CPU will do upon colliding\nwith the stage.",
        .option_values = LabValues_Tech,
        .onOptionChange = Lab_ChangeTech,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Tech in Place Chance",
        .desc = "Adjust the chance the CPU will tech in place.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeTechInPlaceChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Tech Away Chance",
        .desc = "Adjust the chance the CPU will tech away.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeTechAwayChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Tech Toward Chance",
        .desc = "Adjust the chance the CPU will tech toward.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeTechTowardChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Miss Tech Chance",
        .desc = "Adjust the chance the CPU will miss tech.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeMissTechChance,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_Getup) / 4,
        .option_name = "Get Up Option",
        .desc = "Adjust what the CPU will do after missing\na tech input.",
        .option_values = LabValues_Getup,
        .onOptionChange = Lab_ChangeGetup,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Stand Chance",
        .desc = "Adjust the chance the CPU will stand.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeStandChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Roll Away Chance",
        .desc = "Adjust the chance the CPU will roll away.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeRollAwayChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Roll Toward Chance",
        .desc = "Adjust the chance the CPU will roll toward.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeRollTowardChance,
    },
    {
        .option_kind = OPTKIND_INT,
        .value_num = 101,
        .option_val = 25,
        .option_name = "Getup Attack Chance",
        .desc = "Adjust the chance the CPU will getup attack.",
        .option_values = "%d%%",
        .onOptionChange = Lab_ChangeGetupAttackChance,
    },
};

static EventMenu LabMenu_Tech = {
    .name = "Tech Options",
    .option_num = sizeof(LabOptions_Tech) / sizeof(EventOption),
    .options = &LabOptions_Tech,
};

// PLAYBACK CHANCES MENU -----------------------------------------------------

enum slot_chance_menu
{
    OPTSLOTCHANCE_1,
    OPTSLOTCHANCE_2,
    OPTSLOTCHANCE_3,
    OPTSLOTCHANCE_4,
    OPTSLOTCHANCE_5,
    OPTSLOTCHANCE_6,

    OPTSLOTCHANCE_COUNT
};

static EventOption LabOptions_SlotChancesHMN[REC_SLOTS] = {
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 1",
        .desc = "Chance of slot 1 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot1ChanceHMN,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 2",
        .desc = "Chance of slot 2 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot2ChanceHMN,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 3",
        .desc = "Chance of slot 3 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot3ChanceHMN,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 4",
        .desc = "Chance of slot 4 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot4ChanceHMN,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 5",
        .desc = "Chance of slot 5 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot5ChanceHMN,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 6",
        .desc = "Chance of slot 6 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot6ChanceHMN,
    },
};

static EventOption LabOptions_SlotChancesCPU[REC_SLOTS] = {
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 1",
        .desc = "Chance of slot 1 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot1ChanceCPU,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 2",
        .desc = "Chance of slot 2 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot2ChanceCPU,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 3",
        .desc = "Chance of slot 3 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot3ChanceCPU,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 4",
        .desc = "Chance of slot 4 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot4ChanceCPU,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 5",
        .desc = "Chance of slot 5 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot5ChanceCPU,
    },
    {
        .option_kind = OPTKIND_INT,
        .option_name = "Slot 6",
        .desc = "Chance of slot 6 occuring.",
        .option_values = "%d%%",
        .value_num = 101,
        .disable = 1,
        .onOptionChange = Lab_ChangeSlot6ChanceCPU,
    },
};

static EventMenu LabMenu_SlotChancesHMN = {
    .name = "HMN Playback Slot Chances",
    .option_num = sizeof(LabOptions_SlotChancesHMN) / sizeof(EventOption),
    .options = &LabOptions_SlotChancesHMN,
};

static EventMenu LabMenu_SlotChancesCPU = {
    .name = "HMN Playback Slot Chances",
    .option_num = sizeof(LabOptions_SlotChancesCPU) / sizeof(EventOption),
    .options = &LabOptions_SlotChancesCPU,
};

// RECORDING MENU --------------------------------------------------------------

enum autorestore
{
    AUTORESTORE_NONE,
    AUTORESTORE_PLAYBACK_END,
    AUTORESTORE_COUNTER,

    AUTORESTORE_COUNT
};

enum rec_mode_hmn
{
    RECMODE_HMN_OFF,
    RECMODE_HMN_RECORD,
    RECMODE_HMN_PLAYBACK,

    RECMODE_COUNT
};

enum rec_mode_cpu
{
    RECMODE_CPU_OFF,
    RECMODE_CPU_CONTROL,
    RECMODE_CPU_RECORD,
    RECMODE_CPU_PLAYBACK,

    RECMODE_CPU_COUNT
};

enum rec_option
{
   OPTREC_SAVE_LOAD,
   OPTREC_HMNMODE,
   OPTREC_HMNSLOT,
   OPTREC_CPUMODE,
   OPTREC_CPUSLOT,
   OPTREC_MIRRORED_PLAYBACK,
   OPTREC_LOOP,
   OPTREC_AUTORESTORE,
   OPTREC_RESAVE,
   OPTREC_DELETE,
   OPTREC_HMNCHANCE,
   OPTREC_CPUCHANCE,
   OPTREC_EXPORT,

   OPTREC_COUNT
};

// Aitch: Please be aware that the order of these options is important.
// The option idx will be serialized when exported, so loading older replays could load the wrong option if we reorder/remove options.
static char *LabValues_RecordSlot[] = {"Random", "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6"};
static char *LabValues_HMNRecordMode[] = {"Off", "Record", "Playback"};
static char *LabValues_CPURecordMode[] = {"Off", "Control", "Record", "Playback"};
static char *LabValues_AutoRestore[] = {"Off", "Playback Ends", "CPU Counters"};

static const EventOption Record_Save = {
    .option_kind = OPTKIND_FUNC,
    .option_name = "Save Positions",
    .desc = "Save the current fighter positions\nas the initial positions.",
    .onOptionSelect = Record_InitState,
};

static const EventOption Record_Load = {
    .option_kind = OPTKIND_FUNC,
    .option_name = "Restore Positions",
    .desc = "Load the saved fighter positions and \nstart the sequence from the beginning.",
    .onOptionSelect = Record_RestoreState,
};

static EventOption LabOptions_Record[OPTREC_COUNT] = {
    // swapped between Record_Save and Record_Load
    Record_Save,

    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_HMNRecordMode) / 4,
        .option_name = "HMN Mode",
        .desc = "Toggle between recording and playback of\ninputs.",
        .option_values = LabValues_HMNRecordMode,
        .onOptionChange = Record_ChangeHMNMode,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_RecordSlot) / 4,
        .option_val = 1,
        .option_name = "HMN Record Slot",
        .desc = "Toggle which recording slot to save inputs \nto. Maximum of 6 and can be set to random \nduring playback.",
        .option_values = LabValues_RecordSlot,
        .onOptionChange = Record_ChangeHMNSlot,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_CPURecordMode) / 4,
        .option_name = "CPU Mode",
        .desc = "Toggle between recording and playback of\ninputs.",
        .option_values = LabValues_CPURecordMode,
        .onOptionChange = Record_ChangeCPUMode,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_RecordSlot) / 4,
        .option_val = 1,
        .option_name = "CPU Record Slot",
        .desc = "Toggle which recording slot to save inputs \nto. Maximum of 6 and can be set to random \nduring playback.",
        .option_values = LabValues_RecordSlot,
        .onOptionChange = Record_ChangeCPUSlot,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_OffOn) / 4,
        .option_name = "Mirrored Playback",
    	.desc = "Playback with mirrored the recorded inputs,\npositions and facing directions.\n(!) This works properly only on symmetrical \nstages.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = Record_ChangeMirroredPlayback,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabOptions_OffOn) / 4,
        .option_name = "Loop Input Playback",
        .desc = "Loop the recorded inputs when they end.",
        .option_values = LabOptions_OffOn,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = sizeof(LabValues_AutoRestore) / 4,
        .option_name = "Auto Restore",
        .desc = "Automatically restore saved positions.",
        .option_values = LabValues_AutoRestore,
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Re-Save Positions",
        .desc = "Save the current position, without\nremoving recorded inputs.",
        .onOptionSelect = Record_ResaveState,
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Delete Positions",
        .desc = "Delete the current initial position\nand recordings.",
        .onOptionSelect = Record_DeleteState,
    },
    {
        .option_kind = OPTKIND_MENU,
        .option_name = "Set HMN Playback Chances",
        .desc = "Set the chances that slots will be selected\nduring random playback.",
        .menu = &LabMenu_SlotChancesHMN,
    },
    {
        .option_kind = OPTKIND_MENU,
        .option_name = "Set CPU Playback Chances",
        .desc = "Set the chances that slots will be selected\nduring random playback.",
        .menu = &LabMenu_SlotChancesCPU,
    },
    {
        .option_kind = OPTKIND_FUNC,
        .option_name = "Export",
        .desc = "Export the recording to a memory card\nfor later use or to share with others.",
        .onOptionSelect = Export_Init,
    },
};

static EventMenu LabMenu_Record = {
    .name = "Recording",
    .option_num = sizeof(LabOptions_Record) / sizeof(EventOption),
    .options = &LabOptions_Record,
};

// OVERLAY MENU --------------------------------------------------------------

#define OVERLAY_COLOUR_COUNT 10
static char *LabValues_OverlayNames[OVERLAY_COLOUR_COUNT] = { 
    "None", "Red", "Green", "Blue", "Yellow", "White", "Black", 
    "Remove Overlay", "Invisible", "Play Sound" 
};

typedef struct Overlay {
    u32 invisible : 1;
    u32 play_sound : 1;
    u32 occur_once : 1;
    GXColor color;
} Overlay;

static Overlay LabValues_OverlayColours[OVERLAY_COLOUR_COUNT] = {
    { .color = { 0  , 0  , 0  , 0   } },
    { .color = { 255, 20 , 20 , 180 } },
    { .color = { 20 , 255, 20 , 180 } },
    { .color = { 20 , 20 , 255, 180 } },
    { .color = { 220 , 220 , 20, 180 } },
    { .color = { 255, 255, 255, 180 } },
    { .color = { 20 , 20 , 20 , 180 } },

    { .color = { 0  , 0  , 0  , 0   } },
    { .invisible = 1 },
    { .play_sound = 1, .occur_once = 1 }
};

typedef enum overlay_type
{
    OVERLAY_ACTIONABLE,
    OVERLAY_HITSTUN,
    OVERLAY_INVINCIBLE,
    OVERLAY_LEDGE_ACTIONABLE,
    OVERLAY_MISSED_LCANCEL,
    OVERLAY_CAN_FASTFALL,
    OVERLAY_AUTOCANCEL,
    OVERLAY_CROUCH,
    OVERLAY_WAIT,
    OVERLAY_WALK,
    OVERLAY_DASH,
    OVERLAY_RUN,
    OVERLAY_DOUBLEJUMP,
    OVERLAY_FULLHOP,
    OVERLAY_SHORTHOP,
    OVERLAY_IASA,

    OVERLAY_COUNT,
} OverlayGroup;

// copied from LabOptions_OverlaysDefault in Event_Init
static EventOption LabOptions_OverlaysCPU[OVERLAY_COUNT];
static EventOption LabOptions_OverlaysHMN[OVERLAY_COUNT];

static EventOption LabOptions_OverlaysDefault[OVERLAY_COUNT] = {
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Actionable",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Hitstun",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Invincible",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Ledge Actionable",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Missed L-Cancel",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Can Fastfall",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Autocancel",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Crouch",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Wait",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Walk",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Dash",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Run",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Jumps",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Full Hop",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "Short Hop",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = OVERLAY_COLOUR_COUNT,
        .option_name = "IASA",
        .desc = "",
        .option_values = LabValues_OverlayNames,
        .onOptionChange = Lab_ChangeOverlays,
    },
};

static EventMenu LabMenu_OverlaysHMN = {
    .name = "HMN Overlays",
    .option_num = sizeof(LabOptions_OverlaysHMN) / sizeof(EventOption),
    .options = &LabOptions_OverlaysHMN,
};

static EventMenu LabMenu_OverlaysCPU = {
    .name = "CPU Overlays",
    .option_num = sizeof(LabOptions_OverlaysCPU) / sizeof(EventOption),
    .options = &LabOptions_OverlaysCPU,
};


// FUNCTION PROTOTYPES ##############################################################

static u32 lz77_compress(u8 *uncompressed_text, u32 uncompressed_size, u8 *compressed_text, u8 pointer_length_width);
static u32 lz77_decompress(u8 *compressed_text, u8 *uncompressed_text);
static float get_angle_out_of_deadzone(float angle, int lastSDIWasCardinal);
static void distribute_chances(u16 *chances[], unsigned int chance_count);
static void rebound_chances(u16 *chances[], unsigned int chance_count, int just_changed_option);
static int is_tech_anim(int state);
