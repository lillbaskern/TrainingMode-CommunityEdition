// DECLARATIONS #############################################

static EventOption LabOptions_Main[];
static EventOption LabOptions_General[];
static EventOption LabOptions_InfoDisplay[];
static EventOption LabOptions_Tech[];

static EventMenu LabMenu_General;
static EventMenu LabMenu_InfoDisplay;
static EventMenu LabMenu_CPU;
static EventMenu LabMenu_Record;
static EventMenu LabMenu_Tech;

static void rebound_tech_chances(int menu_lookup[4], int just_changed_option);
static int is_tech_anim(int state);

#define AUTORESTORE_DELAY 20

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

// VARS ####################################################

// General Options
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
    OPTGEN_HUD,
    OPTGEN_DI,
    OPTGEN_INPUT,
    OPTGEN_STALE,
};

// CPU Options
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
    //OPTCPU_RESET,
    OPTCPU_BEHAVE,
    OPTCPU_CTRGRND,
    OPTCPU_CTRAIR,
    OPTCPU_CTRSHIELD,
    OPTCPU_CTRFRAMES,
    OPTCPU_CTRHITS,
    OPTCPU_SHIELDHITS,
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
};

// ASDI 
enum asdi
{
    ASDI_NONE,
    ASDI_AWAY,
    ASDI_TOWARD,
    ASDI_LEFT,
    ASDI_RIGHT,
    ASDI_UP,
    ASDI_DOWN,
};

// SDI Freq
enum sdi_dir
{
    SDIDIR_RANDOM,
    SDIDIR_AWAY,
    SDIDIR_TOWARD,
    SDIDIR_UP,
    SDIDIR_DOWN,
    SDIDIR_LEFT,
    SDIDIR_RIGHT,
};

enum autorestore
{
    AUTORESTORE_NONE = 0,
    AUTORESTORE_PLAYBACK_END,
    AUTORESTORE_COUNTER,
};

enum rec_mode_hmn
{
    RECMODE_HMN_OFF = 0,
    RECMODE_HMN_RECORD,
    RECMODE_HMN_PLAYBACK,
};

enum rec_mode_cpu
{
    RECMODE_CPU_OFF = 0,
    RECMODE_CPU_CONTROL,
    RECMODE_CPU_RECORD,
    RECMODE_CPU_PLAYBACK,
};

// Recording Options
#define OPTREC_SAVE_LOAD 0
#define OPTREC_HMNMODE 1
#define OPTREC_HMNSLOT 2
#define OPTREC_CPUMODE 3
#define OPTREC_CPUSLOT 4
#define OPTREC_LOOP 5
#define OPTREC_AUTORESTORE 6
#define OPTREC_RESAVE 7
#define OPTREC_EXPORT 8
#define OPTREC_DELETE 9

// Info Display Options
//#define OPTINF_TOGGLE 0
#define OPTINF_PLAYER 0
#define OPTINF_SIZE 1
#define OPTINF_PRESET 2
#define OPTINF_ROW1 3

// Info Display Rows
enum infdisp_rows
{
    INFDISPROW_NONE,
    INFDISPROW_POS,
    INFDISPROW_STATE,
    INFDISPROW_FRAME,
    INFDISPROW_SELFVEL,
    INFDISPROW_KBVEL,
    INFDISPROW_TOTALVEL,
    INFDISPROW_ENGLSTICK,
    INFDISPROW_SYSLSTICK,
    INFDISPROW_ENGCSTICK,
    INFDISPROW_SYSCSTICK,
    INFDISPROW_ENGTRIGGER,
    INFDISPROW_SYSTRIGGER,
    INFDISPROW_LEDGECOOLDOWN,
    INFDISPROW_INTANGREMAIN,
    INFDISPROW_HITSTOP,
    INFDISPROW_HITSTUN,
    INFDISPROW_SHIELDHEALTH,
    INFDISPROW_SHIELDSTUN,
    INFDISPROW_GRIP,
    INFDISPROW_ECBLOCK,
    INFDISPROW_ECBBOT,
    INFDISPROW_JUMPS,
    INFDISPROW_WALLJUMPS,
    INFDISPROW_JAB,
    INFDISPROW_LINE,
    INFDISPROW_BLASTLR,
    INFDISPROW_BLASTUD,
};

// CPU States
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
};

// Grab Escape Options
enum cpu_mash
{
    CPUMASH_NONE,
    CPUMASH_MED,
    CPUMASH_HIGH,
    CPUMASH_PERFECT,
};

#define INTANG_COLANIM 10

// Grab Escape RNG Def
#define CPUMASHRNG_MED 35
#define CPUMASHRNG_HIGH 55

// Behavior Definitions
#define CPUBEHAVE_STAND 0
#define CPUBEHAVE_SHIELD 1
#define CPUBEHAVE_CROUCH 2
#define CPUBEHAVE_JUMP 3

// SDI Definitions
#define CPUSDI_RANDOM 0
#define CPUSDI_NONE 1

// TDI Definitions
#define CPUTDI_RANDOM 0
#define CPUTDI_IN 1
#define CPUTDI_OUT 2
#define CPUTDI_FLOORHUG 3
#define CPUTDI_CUSTOM 4
#define CPUTDI_RANDOM_CUSTOM 5
#define CPUTDI_NONE 6
#define CPUTDI_NUM 7

// Shield Angle Definitions
#define CPUSHIELDANG_NONE 0
#define CPUSHIELDANG_UP 1
#define CPUSHIELDANG_TOWARD 2
#define CPUSHIELDANG_DOWN 3
#define CPUSHIELDANG_AWAY 4

// Tech Definitions
#define CPUTECH_RANDOM 0
#define CPUTECH_NEUTRAL 1
#define CPUTECH_AWAY 2
#define CPUTECH_TOWARDS 3
#define CPUTECH_NONE 4

// Getup Definitions
#define CPUGETUP_RANDOM 0
#define CPUGETUP_STAND 1
#define CPUGETUP_AWAY 2
#define CPUGETUP_TOWARD 3
#define CPUGETUP_ATTACK 4

// Stick Direction Definitions
enum STICKDIR
{
    STCKDIR_NONE,
    STCKDIR_TOWARD,
    STCKDIR_AWAY,
    STCKDIR_FRONT,
    STCKDIR_BACK,
    STICKDIR_RDM,
};

// Hit kind defintions
#define HITKIND_DAMAGE 0
#define HITKIND_SHIELD 1

// Custom ASID animation checks
#define ASID_ACTIONABLE 1000
#define ASID_ACTIONABLEGROUND 1001
#define ASID_ACTIONABLEAIR 1002
#define ASID_DAMAGEAIR 1003
#define ASID_ANY 1004

// ACTIONS #################################################

// CPU Action Definitions
static CPUAction Lab_CPUActionShield[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_GUARDREFLECT, // state to perform this action. -1 for last
        0,                 // first possible frame to perform this action
        0,                 // last possible frame to perfrom this action
        0,                 // left stick X value
        0,                 // left stick Y value
        0,                 // c stick X value
        0,                 // c stick Y value
        PAD_TRIGGER_R,     // button to input
        1,                 // is the last input
        0,                 // specify stick direction
    },
    {
        ASID_GUARD,    // state to perform this action. -1 for last
        0,             // first possible frame to perform this action
        0,             // last possible frame to perfrom this action
        0,             // left stick X value
        0,             // left stick Y value
        0,             // c stick X value
        0,             // c stick Y value
        PAD_TRIGGER_R, // button to input
        1,             // is the last input
        0,             // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionGrab[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_BUTTON_A | PAD_TRIGGER_R, // button to input
        1,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_Z,         // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionUpB[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_GUARD,   // state to perform this action. -1 for last
        0,            // first possible frame to perform this action
        0,            // last possible frame to perfrom this action
        0,            // left stick X value
        0,            // left stick Y value
        0,            // c stick X value
        0,            // c stick Y value
        PAD_BUTTON_X, // button to input
        0,            // is the last input
        0,            // specify stick direction
    },
    {
        ASID_KNEEBEND, // state to perform this action. -1 for last
        0,             // first possible frame to perform this action
        0,             // last possible frame to perfrom this action
        0,             // left stick X value
        127,           // left stick Y value
        0,             // c stick X value
        0,             // c stick Y value
        PAD_BUTTON_B,  // button to input
        1,             // is the last input
        0,             // specify stick direction
    },
    {
        ASID_ACTIONABLE, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        0,               // left stick X value
        127,             // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_B,    // button to input
        1,               // is the last input
        0,               // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionSideBToward[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_GUARD,   // state to perform this action. -1 for last
        0,            // first possible frame to perform this action
        0,            // last possible frame to perfrom this action
        0,            // left stick X value
        0,            // left stick Y value
        0,            // c stick X value
        0,            // c stick Y value
        PAD_BUTTON_X, // button to input
        0,            // is the last input
        0,            // specify stick direction
    },
    {
        ASID_ACTIONABLE, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        127,               // left stick X value
        0,             // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_B,    // button to input
        1,               // is the last input
        STCKDIR_TOWARD,  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionSideBAway[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_GUARD,   // state to perform this action. -1 for last
        0,            // first possible frame to perform this action
        0,            // last possible frame to perfrom this action
        0,            // left stick X value
        0,            // left stick Y value
        0,            // c stick X value
        0,            // c stick Y value
        PAD_BUTTON_X, // button to input
        0,            // is the last input
        0,            // specify stick direction
    },
    {
        ASID_ACTIONABLE, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        127,             // left stick X value
        0,               // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_B,    // button to input
        1,               // is the last input
        STCKDIR_AWAY,    // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionDownB[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_GUARD,   // state to perform this action. -1 for last
        0,            // first possible frame to perform this action
        0,            // last possible frame to perfrom this action
        0,            // left stick X value
        0,            // left stick Y value
        0,            // c stick X value
        0,            // c stick Y value
        PAD_BUTTON_X, // button to input
        1,            // is the last input
        0,            // specify stick direction
    },
    {
        ASID_ACTIONABLE, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        0,               // left stick X value
        -127,            // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_B,    // button to input
        1,               // is the last input
        0,               // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionNeutralB[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_GUARD,   // state to perform this action. -1 for last
        0,            // first possible frame to perform this action
        0,            // last possible frame to perfrom this action
        0,            // left stick X value
        0,            // left stick Y value
        0,            // c stick X value
        0,            // c stick Y value
        PAD_BUTTON_X, // button to input
        1,            // is the last input
        0,            // specify stick direction
    },

    {
        ASID_ACTIONABLE, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        0,               // left stick X value
        0,               // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_B,    // button to input
        1,               // is the last input
        0,               // specify stick direction
    },
    -1,
};

// We buffer this for a single frame.
// For some reason spotdodge is only possible frame 2 when floorhugging 
// an attack that would have otherwise knocked you into the air without knockdown.
// This doesn't occur with rolls for some reason.
static CPUAction Lab_CPUActionSpotdodge[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        -127,                  // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ANY,              // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        -127,                  // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionRollAway[] = {
    {
        ASID_GUARD,    // state to perform this action. -1 for last
        0,             // first possible frame to perform this action
        0,             // last possible frame to perfrom this action
        127,           // left stick X value
        0,             // left stick Y value
        0,             // c stick X value
        0,             // c stick Y value
        PAD_TRIGGER_R, // button to input
        1,             // is the last input
        STCKDIR_AWAY,  // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_GUARDREFLECT, // state to perform this action. -1 for last
        0,                 // first possible frame to perform this action
        0,                 // last possible frame to perfrom this action
        127,               // left stick X value
        0,                 // left stick Y value
        0,                 // c stick X value
        0,                 // c stick Y value
        PAD_TRIGGER_R,     // button to input
        1,                 // is the last input
        STCKDIR_AWAY,      // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionRollTowards[] = {
    {
        ASID_GUARD,     // state to perform this action. -1 for last
        0,              // first possible frame to perform this action
        0,              // last possible frame to perfrom this action
        127,            // left stick X value
        0,              // left stick Y value
        0,              // c stick X value
        0,              // c stick Y value
        PAD_TRIGGER_R,  // button to input
        1,              // is the last input
        STCKDIR_TOWARD, // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_GUARDREFLECT, // state to perform this action. -1 for last
        0,                 // first possible frame to perform this action
        0,                 // last possible frame to perfrom this action
        127,               // left stick X value
        0,                 // left stick Y value
        0,                 // c stick X value
        0,                 // c stick Y value
        PAD_TRIGGER_R,     // button to input
        1,                 // is the last input
        STCKDIR_TOWARD,    // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionRollRandom[] = {
    {
        ASID_GUARD,    // state to perform this action. -1 for last
        0,             // first possible frame to perform this action
        0,             // last possible frame to perfrom this action
        127,           // left stick X value
        0,             // left stick Y value
        0,             // c stick X value
        0,             // c stick Y value
        PAD_TRIGGER_R, // button to input
        1,             // is the last input
        STICKDIR_RDM,  // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_TRIGGER_R,         // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_GUARDREFLECT, // state to perform this action. -1 for last
        0,                 // first possible frame to perform this action
        0,                 // last possible frame to perfrom this action
        127,               // left stick X value
        0,                 // left stick Y value
        0,                 // c stick X value
        0,                 // c stick Y value
        PAD_TRIGGER_R,     // button to input
        1,                 // is the last input
        STICKDIR_RDM,      // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionNair[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        PAD_BUTTON_A,       // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionFair[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        127,                // c stick X value
        0,                  // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        3,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionDair[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        -127,               // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionBair[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        127,                // c stick X value
        0,                  // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        4,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionUair[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        127,                // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionJump[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionJumpFull[] = {
    {
        ASID_GUARD,                   // state to perform this action. -1 for last
        0,                            // first possible frame to perform this action
        0,                            // last possible frame to perfrom this action
        0,                            // left stick X value
        0,                            // left stick Y value
        0,                            // c stick X value
        0,                            // c stick Y value
        PAD_TRIGGER_R | PAD_BUTTON_X, // button to input
        0,                            // is the last input
        0,                            // specify stick direction
    },
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_X,          // button to input
        0,                     // is the last input
        0,                     // specify stick direction
    },
    {
        ASID_KNEEBEND, // state to perform this action. -1 for last
        0,             // first possible frame to perform this action
        0,             // last possible frame to perfrom this action
        0,             // left stick X value
        0,             // left stick Y value
        0,             // c stick X value
        0,             // c stick Y value
        PAD_BUTTON_X,  // button to input
        0,             // is the last input
        0,             // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionJumpAway[] = {
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        127,                // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        PAD_BUTTON_X,       // button to input
        1,                  // is the last input
        STCKDIR_AWAY,       // specify stick direction
    },

    -1,
};
static CPUAction Lab_CPUActionJumpTowards[] = {
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        127,                // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        PAD_BUTTON_X,       // button to input
        1,                  // is the last input
        STCKDIR_TOWARD,     // specify stick direction
    },

    -1,
};
static CPUAction Lab_CPUActionJumpNeutral[] = {
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,               // first possible frame to perform this action
        0,               // last possible frame to perfrom this action
        0,               // left stick X value
        0,               // left stick Y value
        0,               // c stick X value
        0,               // c stick Y value
        PAD_BUTTON_X,    // button to input
        1,               // is the last input
        STCKDIR_NONE,    // specify stick direction
    },

    -1,
};
static CPUAction Lab_CPUActionAirdodge[] = {
    // wiggle out if we are in tumble
    {
        ASID_DAMAGEAIR, // state to perform this action. -1 for last
        0,              // first possible frame to perform this action
        0,              // last possible frame to perfrom this action
        127,            // left stick X value
        0,              // left stick Y value
        0,              // c stick X value
        0,              // c stick Y value
        0,              // button to input
        0,              // is the last input
        0,              // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        0,                  // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        PAD_TRIGGER_R,      // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionFFTumble[] = {
    {
        ASID_DAMAGEAIR, // state to perform this action. -1 for last
        0,              // first possible frame to perform this action
        0,              // last possible frame to perfrom this action
        0,              // left stick X value
        -127,           // left stick Y value
        0,              // c stick X value
        0,              // c stick Y value
        0,              // button to input
        1,              // is the last input
        0,              // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionFFWiggle[] = {
    {
        ASID_DAMAGEAIR, // state to perform this action. -1 for last
        0,              // first possible frame to perform this action
        0,              // last possible frame to perfrom this action
        127,            // left stick X value
        0,              // left stick Y value
        0,              // c stick X value
        0,              // c stick Y value
        0,              // button to input
        0,              // is the last input
        0,              // specify stick direction
    },
    {
        ASID_ACTIONABLEAIR, // state to perform this action. -1 for last
        0,                  // first possible frame to perform this action
        0,                  // last possible frame to perfrom this action
        0,                  // left stick X value
        -127,               // left stick Y value
        0,                  // c stick X value
        0,                  // c stick Y value
        0,                  // button to input
        1,                  // is the last input
        0,                  // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionJab[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_A,          // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionFTilt[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        80,                    // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_A,          // button to input
        1,                     // is the last input
        STCKDIR_TOWARD,        // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionUTilt[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        80,                    // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_A,          // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionDTilt[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        -80,                   // left stick Y value
        0,                     // c stick X value
        0,                     // c stick Y value
        PAD_BUTTON_A,          // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionUSmash[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        127,                   // c stick Y value
        0,                     // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionDSmash[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        0,                     // c stick X value
        -127,                  // c stick Y value
        0,                     // button to input
        1,                     // is the last input
        0,                     // specify stick direction
    },
    -1,
};
static CPUAction Lab_CPUActionFSmash[] = {
    {
        ASID_ACTIONABLEGROUND, // state to perform this action. -1 for last
        0,                     // first possible frame to perform this action
        0,                     // last possible frame to perfrom this action
        0,                     // left stick X value
        0,                     // left stick Y value
        127,                   // c stick X value
        0,                     // c stick Y value
        0,                     // button to input
        1,                     // is the last input
        STCKDIR_TOWARD,        // specify stick direction
    },
    -1,
};

static CPUAction *Lab_CPUActions[] = {
    // none 0
    0,
    // shield 1
    &Lab_CPUActionShield,
    // grab 2
    &Lab_CPUActionGrab,
    // up b 3
    &Lab_CPUActionUpB,
    // side b 4
    &Lab_CPUActionSideBToward,
    &Lab_CPUActionSideBAway,
    // down b 4
    &Lab_CPUActionDownB,
    &Lab_CPUActionNeutralB,
    // spotdodge 5
    &Lab_CPUActionSpotdodge,
    // roll away 6
    &Lab_CPUActionRollAway,
    // roll towards 7
    &Lab_CPUActionRollTowards,
    // roll random
    &Lab_CPUActionRollRandom,
    // nair 8
    &Lab_CPUActionNair,
    // fair 9
    &Lab_CPUActionFair,
    // dair 10
    &Lab_CPUActionDair,
    // bair 11
    &Lab_CPUActionBair,
    // uair 12
    &Lab_CPUActionUair,
    // short hop 13
    &Lab_CPUActionJump,
    // full hop 14
    &Lab_CPUActionJumpFull,
    // jump away 15
    &Lab_CPUActionJumpAway,
    // jump towards 16
    &Lab_CPUActionJumpTowards,
    // jump neutral 16
    &Lab_CPUActionJumpNeutral,
    // airdodge 17
    &Lab_CPUActionAirdodge,
    // fastfall 18
    &Lab_CPUActionFFTumble,
    // wiggle fastfall 19
    &Lab_CPUActionFFWiggle,
    &Lab_CPUActionJab,
    &Lab_CPUActionFTilt,
    &Lab_CPUActionUTilt,
    &Lab_CPUActionDTilt,
    &Lab_CPUActionUSmash,
    &Lab_CPUActionDSmash,
    &Lab_CPUActionFSmash,
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
};
static char *CPU_ACTIONS_NAMES[] = {
    "CPUACT_NONE",
    "CPUACT_SHIELD",
    "CPUACT_GRAB",
    "CPUACT_UPB",
    "CPUACT_SIDEBLEFT",
    "CPUACT_SIDEBRIGHT",
    "CPUACT_DOWNB",
    "CPUACT_NEUTRALB",
    "CPUACT_SPOTDODGE",
    "CPUACT_ROLLAWAY",
    "CPUACT_ROLLTOWARDS",
    "CPUACT_ROLLRDM",
    "CPUACT_NAIR",
    "CPUACT_FAIR",
    "CPUACT_DAIR",
    "CPUACT_BAIR",
    "CPUACT_UAIR",
    "CPUACT_SHORTHOP",
    "CPUACT_FULLHOP",
    "CPUACT_JUMPAWAY",
    "CPUACT_JUMPTOWARDS",
    "CPUACT_JUMPNEUTRAL",
    "CPUACT_AIRDODGE",
    "CPUACT_FFTUMBLE",
    "CPUACT_FFWIGGLE",
    "CPUACT_JAB",
    "CPUACT_FTILT",
    "CPUACT_UTILT",
    "CPUACT_DTILT",
    "CPUACT_USMASH",
    "CPUACT_DSMASH",
    "CPUACT_FSMASH",
};
static u8 GrAcLookup[] = {CPUACT_NONE, CPUACT_SPOTDODGE, CPUACT_SHIELD, CPUACT_GRAB, CPUACT_UPB, CPUACT_SIDEBTOWARD, CPUACT_SIDEBAWAY, CPUACT_DOWNB, CPUACT_NEUTRALB, CPUACT_USMASH, CPUACT_DSMASH, CPUACT_FSMASH, CPUACT_ROLLAWAY, CPUACT_ROLLTOWARDS, CPUACT_ROLLRDM, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR, CPUACT_JAB, CPUACT_FTILT, CPUACT_UTILT, CPUACT_DTILT, CPUACT_SHORTHOP, CPUACT_FULLHOP};
static u8 AirAcLookup[] = {CPUACT_NONE, CPUACT_AIRDODGE, CPUACT_JUMPAWAY, CPUACT_JUMPTOWARDS, CPUACT_JUMPNEUTRAL, CPUACT_UPB, CPUACT_SIDEBTOWARD, CPUACT_SIDEBAWAY, CPUACT_DOWNB, CPUACT_NEUTRALB, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR, CPUACT_FFTUMBLE, CPUACT_FFWIGGLE};
static u8 ShieldAcLookup[] = {CPUACT_NONE, CPUACT_GRAB, CPUACT_SHORTHOP, CPUACT_FULLHOP, CPUACT_SPOTDODGE, CPUACT_ROLLAWAY, CPUACT_ROLLTOWARDS, CPUACT_ROLLRDM, CPUACT_UPB, CPUACT_DOWNB, CPUACT_NAIR, CPUACT_FAIR, CPUACT_DAIR, CPUACT_BAIR, CPUACT_UAIR};

// MENUS ###################################################

// Main Menu
static char **LabOptions_OffOn[] = {"Off", "On"};
static EventOption LabOptions_Main[] = {
    {
        .option_kind = OPTKIND_MENU,                                                     // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                                                  // number of values for this option
        .option_val = 0,                                                                 // value of this option
        .menu = &LabMenu_General,                                                        // pointer to the menu that pressing A opens
        .option_name = {"General"},                                                      // pointer to a string
        .desc = "Toggle player percent, overlays,\nframe advance, and camera settings.", // string describing what this option does
        .option_values = 0,                                                              // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_MENU,       // the type of option this is; menu, string list, integers list, etc
        .value_num = 0,                    // number of values for this option
        .option_val = 0,                   // value of this option
        .menu = &LabMenu_CPU,              // pointer to the menu that pressing A opens
        .option_name = {"CPU Options"},    // pointer to a string
        .desc = "Configure CPU behavior.", // string describing what this option does
        .option_values = 0,                // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_MENU,           // the type of option this is; menu, string list, integers list, etc
        .value_num = 0,                        // number of values for this option
        .option_val = 0,                       // value of this option
        .menu = &LabMenu_Record,               // pointer to the menu that pressing A opens
        .option_name = "Recording",            // pointer to a string
        .desc = "Record and playback inputs.", // string describing what this option does
        .option_values = 0,                    // pointer to an array of strings
        .onOptionChange = 0,
    },
    // info display
    {
        .option_kind = OPTKIND_MENU,                          // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                       // number of values for this option
        .option_val = 0,                                      // value of this option
        .menu = &LabMenu_InfoDisplay,                         // pointer to the menu that pressing A opens
        .option_name = "Info Display",                        // pointer to a string
        .desc = "Display various game information onscreen.", // string describing what this option does
        .option_values = 0,                                   // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_FUNC,                                                                                       // the type of option this is; menu, string list, integers list, etc
        .value_num = 0,                                                                                                    // number of values for this option
        .option_val = 0,                                                                                                   // value of this option
        .menu = 0,                                                                                                         // pointer to the menu that pressing A opens
        .option_name = "Help",                                                                                             // pointer to a string
        .desc = "D-Pad Left - Load State\nD-Pad Right - Save State\nD-Pad Down - Move CPU\nHold R in the menu for turbo.", // string describing what this option does
        .option_values = 0,                                                                                                // pointer to an array of strings
        .onOptionChange = Lab_Exit,
    },
    {
        .option_kind = OPTKIND_FUNC,                  // the type of option this is; menu, string list, integers list, etc
        .value_num = 0,                               // number of values for this option
        .option_val = 0,                              // value of this option
        .menu = 0,                                    // pointer to the menu that pressing A opens
        .option_name = "Exit",                        // pointer to a string
        .desc = "Return to the Event Select Screen.", // string describing what this option does
        .option_values = 0,                           // pointer to an array of strings
        .onOptionChange = 0,
        .onOptionSelect = Lab_Exit,
    },
};
static EventMenu LabMenu_Main = {
    .name = "Main Menu",                                         // the name of this menu
    .option_num = sizeof(LabOptions_Main) / sizeof(EventOption), // number of options this menu contains
    .scroll = 0,                                                 // runtime variable used for how far down in the menu to start
    .state = 0,                                                  // bool used to know if this menu is focused, used at runtime
    .cursor = 0,                                                 // index of the option currently selected, used at runtime
    .options = &LabOptions_Main,                                 // pointer to all of this menu's options
    .prev = 0,                                                   // pointer to previous menu, used at runtime
};
// General
static char **LabOptions_CamMode[] = {"Normal", "Zoom", "Fixed", "Advanced"};
static char **LabOptions_FrameAdvButton[] = {"L", "Z", "X", "Y"};
static EventOption LabOptions_General[] = {
    // frame advance
    {
        .option_kind = OPTKIND_STRING,                                                                 // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabOptions_OffOn) / 4,                                                     // number of values for this option
        .option_val = 0,                                                                               // value of this option
        .menu = 0,                                                                                     // pointer to the menu that pressing A opens
        .option_name = "Frame Advance",                                                                // pointer to a string
        .desc = "Enable frame advance. Press to advance one\nframe. Hold to advance at normal speed.", // string describing what this option does
        .option_values = LabOptions_OffOn,                                                             // pointer to an array of strings
        .onOptionChange = Lab_ChangeFrameAdvance,
    },
    // frame advance button
    {
        .option_kind = OPTKIND_STRING,                        // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabOptions_FrameAdvButton) / 4,   // number of values for this option
        .option_val = 0,                                      // value of this option
        .menu = 0,                                            // pointer to the menu that pressing A opens
        .option_name = "Frame Advance Button",                // pointer to a string
        .desc = "Choose which button will advance the frame", // string describing what this option does
        .option_values = LabOptions_FrameAdvButton,           // pointer to an array of strings
        .onOptionChange = 0,
        .disable = 1,
    },
    // p1 percent
    {
        .option_kind = OPTKIND_INT,             // the type of option this is; menu, string list, integer list, etc
        .value_num = 999,                       // number of values for this option
        .option_val = 0,                        // value of this option
        .menu = 0,                              // pointer to the menu that pressing A opens
        .option_name = "Player Percent",        // pointer to a string
        .desc = "Adjust the player's percent.", // string describing what this option does
        .option_values = "%d%%",                // pointer to an array of strings
        .onOptionChange = Lab_ChangePlayerPercent,
    },
    {
        .option_kind = OPTKIND_STRING,          
        .value_num = 2,                    
        .option_val = 0,                         
        .menu = 0,                           
        .option_name = "Lock Player Percent",        
        .desc = "Locks Player percent to current percent",
        .option_values = LabOptions_OffOn,             
        .onOptionChange = Lab_ChangePlayerLockPercent,
    },
    // model display
    {
        .option_kind = OPTKIND_STRING,                      // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                     // number of values for this option
        .option_val = 1,                                    // value of this option
        .menu = 0,                                          // pointer to the menu that pressing A opens
        .option_name = "Model Display",                     // pointer to a string
        .desc = "Toggle player and item model visibility.", // string describing what this option does
        .option_values = LabOptions_OffOn,                  // pointer to an array of strings
        .onOptionChange = Lab_ChangeModelDisplay,
    },
    // fighter collision
    {
        .option_kind = OPTKIND_STRING,                                                                                                                           // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                                                                                                                          // number of values for this option
        .option_val = 0,                                                                                                                                         // value of this option
        .menu = 0,                                                                                                                                               // pointer to the menu that pressing A opens
        .option_name = "Fighter Collision",                                                                                                                      // pointer to a string
        .desc = "Toggle hitbox and hurtbox visualization.\nYellow = hurt, red = hit, purple = grab, \nwhite = trigger, green = reflect,\nblue = shield/absorb.", // string describing what this option does
        .option_values = LabOptions_OffOn,                                                                                                                       // pointer to an array of strings
        .onOptionChange = Lab_ChangeHitDisplay,
    },
    // environment collision
    {
        .option_kind = OPTKIND_STRING,                                                                                          // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                                                                                         // number of values for this option
        .option_val = 0,                                                                                                        // value of this option
        .menu = 0,                                                                                                              // pointer to the menu that pressing A opens
        .option_name = "Environment Collision",                                                                                 // pointer to a string
        .desc = "Toggle environment collision visualization.\nAlso displays the players' ECB (environmental \ncollision box).", // string describing what this option does
        .option_values = LabOptions_OffOn,                                                                                      // pointer to an array of strings
        .onOptionChange = Lab_ChangeEnvCollDisplay,
    },
    // camera mode
    {
        .option_kind = OPTKIND_STRING,                                                                                                      // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabOptions_CamMode) / 4,                                                                                        // number of values for this option
        .option_val = 0,                                                                                                                    // value of this option
        .menu = 0,                                                                                                                          // pointer to the menu that pressing A opens
        .option_name = "Camera Mode",                                                                                                       // pointer to a string
        .desc = "Adjust the camera's behavior.\nIn advanced mode, use C-Stick while holding\nA/B/Y to pan, rotate and zoom, respectively.", // string describing what this option does
        .option_values = LabOptions_CamMode,                                                                                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeCamMode,
    },
    // hud
    {
        .option_kind = OPTKIND_STRING,                          // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                         // number of values for this option
        .option_val = 1,                                        // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "HUD",                                   // pointer to a string
        .desc = "Toggle player percents and timer visibility.", // string describing what this option does
        .option_values = LabOptions_OffOn,                      // pointer to an array of strings
        .onOptionChange = Lab_ChangeHUD,
    },
    // di display
    {
        .option_kind = OPTKIND_STRING,                                                                                      // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                                                                                     // number of values for this option
        .option_val = 0,                                                                                                    // value of this option
        .menu = 0,                                                                                                          // pointer to the menu that pressing A opens
        .option_name = "DI Display",                                                                                        // pointer to a string
        .desc = "Display knockback trajectories.\nUse frame advance to see the effects of DI\nin realtime during hitstop.", // string describing what this option does
        .option_values = LabOptions_OffOn,                                                                                  // pointer to an array of strings
        .onOptionChange = 0,
    },
    // input display
    {
        .option_kind = OPTKIND_STRING,             // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                            // number of values for this option
        .option_val = 0,                           // value of this option
        .menu = 0,                                 // pointer to the menu that pressing A opens
        .option_name = "Input Display",            // pointer to a string
        .desc = "Display player inputs onscreen.", // string describing what this option does
        .option_values = LabOptions_OffOn,         // pointer to an array of strings
        .onOptionChange = 0,
    },
    // move staling
    {
        .option_kind = OPTKIND_STRING,                                                          // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                                                         // number of values for this option
        .option_val = 1,                                                                        // value of this option
        .menu = 0,                                                                              // pointer to the menu that pressing A opens
        .option_name = "Move Staling",                                                          // pointer to a string
        .desc = "Toggle the staling of moves. Attacks become \nweaker the more they are used.", // string describing what this option does
        .option_values = LabOptions_OffOn,                                                      // pointer to an array of strings
        .onOptionChange = 0,
    },
};
static EventMenu LabMenu_General = {
    .name = "General",                                              // the name of this menu
    .option_num = sizeof(LabOptions_General) / sizeof(EventOption), // number of options this menu contains
    .scroll = 0,                                                    // runtime variable used for how far down in the menu to start
    .state = 0,                                                     // bool used to know if this menu is focused, used at runtime
    .cursor = 0,                                                    // index of the option currently selected, used at runtime
    .options = &LabOptions_General,                                 // pointer to all of this menu's options
    .prev = 0,                                                      // pointer to previous menu, used at runtime
};
// Info Display
static char **LabValues_InfoDisplay[] = {"None", "Position", "State Name", "State Frame", "Velocity - Self", "Velocity - KB", "Velocity - Total", "Engine LStick", "System LStick", "Engine CStick", "System CStick", "Engine Trigger", "System Trigger", "Ledgegrab Timer", "Intangibility Timer", "Hitlag", "Hitstun", "Shield Health", "Shield Stun", "Grip Strength", "ECB Lock", "ECB Bottom", "Jumps", "Walljumps", "Jab Counter", "Line Info", "Blastzone Left/Right", "Blastzone Up/Down"};
static char **LabValues_InfoPresets[] = {"None", "Custom", "Ledge", "Damage"};
//static char **LabValues_InfoPosition[] = {"Top Left", "Top Mid", "Top Right", "Bottom Left", "Bottom Mid", "Bottom Right"};
static char **LabValues_InfoSize[] = {"Small", "Medium", "Large"};
static char **LabValues_InfoPlayers[] = {"Player 1", "Player 2", "Player 3", "Player 4"};
static EventOption LabOptions_InfoDisplay[] = {
    {
        .option_kind = OPTKIND_STRING,                           // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoPlayers) / 4,          // number of values for this option
        .option_val = 0,                                         // value of this option
        .menu = 0,                                               // pointer to the menu that pressing A opens
        .option_name = "Player",                                 // pointer to a string
        .desc = "Toggle which player's information to display.", // string describing what this option does
        .option_values = LabValues_InfoPlayers,                  // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoPlayer,
    },
    {
        .option_kind = OPTKIND_STRING,                                                                                                        // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoSize) / 4,                                                                                          // number of values for this option
        .option_val = 1,                                                                                                                      // value of this option
        .menu = 0,                                                                                                                            // pointer to the menu that pressing A opens
        .option_name = "Size",                                                                                                                // pointer to a string
        .desc = "Change the size of the info display window.\nLarge is recommended for CRT.\nMedium/Small recommended for Dolphin Emulator.", // string describing what this option does
        .option_values = LabValues_InfoSize,                                                                                                  // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoSizePos,
    },
    {
        .option_kind = OPTKIND_STRING,                       // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoPresets) / 4,      // number of values for this option
        .option_val = 0,                                     // value of this option
        .menu = 0,                                           // pointer to the menu that pressing A opens
        .option_name = "Display Preset",                     // pointer to a string
        .desc = "Choose between pre-configured selections.", // string describing what this option does
        .option_values = LabValues_InfoPresets,              // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoPreset,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 1",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 2",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 3",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 4",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 5",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 6",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 7",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
    {
        .option_kind = OPTKIND_STRING,                   // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_InfoDisplay) / 4,  // number of values for this option
        .option_val = 0,                                 // value of this option
        .menu = 0,                                       // pointer to the menu that pressing A opens
        .option_name = "Row 8",                          // pointer to a string
        .desc = "Adjust what is displayed in this row.", // string describing what this option does
        .option_values = LabValues_InfoDisplay,          // pointer to an array of strings
        .onOptionChange = Lab_ChangeInfoRow,
    },
};
static EventMenu LabMenu_InfoDisplay = {
    .name = "Info Display",             // the name of this menu
    .option_num = 11,                   // number of options this menu contains
    .scroll = 0,                        // runtime variable used for how far down in the menu to start
    .state = 0,                         // bool used to know if this menu is focused, used at runtime
    .cursor = 0,                        // index of the option currently selected, used at runtime
    .options = &LabOptions_InfoDisplay, // pointer to all of this menu's options
    .prev = 0,                          // pointer to previous menu, used at runtime
};
// CPU
static char **LabValues_Shield[] = {"Off", "On Until Hit", "On"};
static char **LabValues_ShieldDir[] = {"Neutral", "Up", "Towards", "Down", "Away"};
static char **LabValues_CPUBehave[] = {"Stand", "Shield", "Crouch", "Jump"};
static char **LabValues_TDI[] = {"Random", "Inwards", "Outwards", "Floorhug", "Custom", "Random Custom", "None"};
static char **LabValues_ASDI[] = {"None", "Away", "Towards", "Left", "Right", "Up", "Down"};
static char **LabValues_SDIDir[] = {"Random", "Away", "Towards", "Up", "Down", "Left", "Right"};
static char **LabValues_Tech[] = {"Random", "Neutral", "Away", "Towards", "None"};
static char **LabValues_Getup[] = {"Random", "Stand", "Away", "Towards", "Attack"};
static char **LabValues_CounterGround[] = {"None", "Spotdodge", "Shield", "Grab", "Up B", "Side B Toward", "Side B Away", "Down B", "Neutral B", "Up Smash", "Down Smash", "Forward Smash", "Roll Away", "Roll Towards", "Roll Random", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air", "Jab", "Forward Tilt", "Up Tilt", "Down Tilt", "Short Hop", "Full Hop"};
static char **LabValues_CounterAir[] = {"None", "Airdodge", "Jump Away", "Jump Towards", "Jump Neutral", "Up B", "Side B Toward", "Side B Away", "Down B", "Neutral B", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air", "Tumble Fastfall", "Wiggle Fastfall"};
static char **LabValues_CounterShield[] = {"None", "Grab", "Short Hop", "Full Hop", "Spotdodge", "Roll Away", "Roll Towards", "Roll Random", "Up B", "Down B", "Neutral Air", "Forward Air", "Down Air", "Back Air", "Up Air"};
static char **LabValues_GrabEscape[] = {"None", "Medium", "High", "Perfect"};
static char **LabValues_LockCPUPercent[] = {"Off", "On"};
static EventOption LabOptions_CPU[] = {
    // cpu percent
    {
        .option_kind = OPTKIND_INT,          // the type of option this is; menu, string list, integer list, etc
        .value_num = 999,                    // number of values for this option
        .option_val = 0,                     // value of this option
        .menu = 0,                           // pointer to the menu that pressing A opens
        .option_name = "CPU Percent",        // pointer to a string
        .desc = "Adjust the CPU's percent.", // string describing what this option does
        .option_values = "%d%%",             // pointer to an array of strings
        .onOptionChange = Lab_ChangeCPUPercent,
    },
    {
        .option_kind = OPTKIND_STRING,          
        .value_num = 2,                    
        .option_val = 0,                         
        .menu = 0,                           
        .option_name = "Lock CPU Percent",        
        .desc = "Locks CPU percent to current percent",
        .option_values = LabValues_LockCPUPercent,             
        .onOptionChange = Lab_ChangeCPULockPercent,
    },
    {
        .option_kind = OPTKIND_MENU,
        .value_num = 0,
        .option_val = 0,
        .menu = &LabMenu_Tech,
        .option_name = {"Tech Options"},
        .desc = "Configure CPU Tech Behavior.",
        .option_values = 0,
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                        // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_TDI) / 4,                               // number of values for this option
        .option_val = 0,                                                      // value of this option
        .menu = 0,                                                            // pointer to the menu that pressing A opens
        .option_name = "Trajectory DI",                                       // pointer to a string
        .desc = "Adjust how the CPU will alter their knockback\ntrajectory.", // string describing what this option does
        .option_values = LabValues_TDI,                                       // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_FUNC,                                           // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                                        // number of values for this option
        .option_val = 0,                                                       // value of this option
        .menu = 0,                                                             // pointer to the menu that pressing A opens
        .option_name = "Custom TDI",                                           // pointer to a string
        .desc = "Create custom trajectory DI values for the\nCPU to perform.", // string describing what this option does
        .option_values = 0,                                                    // pointer to an array of strings
        .onOptionChange = 0,
        .onOptionSelect = Lab_SelectCustomTDI,
    },
    // SDI amount
    {
        .option_kind = OPTKIND_INT,
        .value_num = 8,
        .option_val = 0,
        .menu = 0,
        .option_name = "Smash DI Amount",
        .desc = "Adjust how often the CPU will alter their position\nduring hitstop.",
        .option_values = "%d Frames",
        .onOptionChange = 0,
    },
    // SDI Direction
    {
        .option_kind = OPTKIND_STRING,                                                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_SDIDir) / 4,                                                   // number of values for this option
        .option_val = 0,                                                                             // value of this option
        .menu = 0,                                                                                   // pointer to the menu that pressing A opens
        .option_name = "Smash DI Direction",                                                         // pointer to a string
        .desc = "Adjust the direction in which the CPU will alter \ntheir position during hitstop.", // string describing what this option does
        .option_values = LabValues_SDIDir,                                                           // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                         // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_ASDI) / 4,                               // number of values for this option
        .option_val = 0,                                                       // value of this option
        .menu = 0,                                                             // pointer to the menu that pressing A opens
        .option_name = "ASDI",                                                 // pointer to a string
        .desc = "Set CPU C-stick ASDI direction",                              // string describing what this option does
        .option_values = LabValues_ASDI,                                       // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                    // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_Shield) / 4,        // number of values for this option
        .option_val = 1,                                  // value of this option
        .menu = 0,                                        // pointer to the menu that pressing A opens
        .option_name = {"Infinite Shields"},              // pointer to a string
        .desc = "Adjust how shield health deteriorates.", // string describing what this option does
        .option_values = LabValues_Shield,                // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                    // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_ShieldDir) / 4,     // number of values for this option
        .option_val = 0,                                  // value of this option
        .menu = 0,                                        // pointer to the menu that pressing A opens
        .option_name = {"Shield Angle"},              // pointer to a string
        .desc = "Adjust how CPU angles their shield.",    // string describing what this option does
        .option_values = LabValues_ShieldDir,             // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                      // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                     // number of values for this option
        .option_val = 0,                                    // value of this option
        .menu = 0,                                          // pointer to the menu that pressing A opens
        .option_name = {"Intangibility"},                   // pointer to a string
        .desc = "Toggle the CPU's ability to take damage.", // string describing what this option does
        .option_values = LabOptions_OffOn,                  // pointer to an array of strings
        .onOptionChange = Lab_ChangeCPUIntang,
    },
    {
        .option_kind = OPTKIND_STRING,                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_GrabEscape) / 4,               // number of values for this option
        .option_val = CPUMASH_NONE,                                  // value of this option
        .menu = 0,                                                   // pointer to the menu that pressing A opens
        .option_name = "Grab Escape",                                // pointer to a string
        .desc = "Adjust how the CPU will attempt to escape\ngrabs.", // string describing what this option does
        .option_values = LabValues_GrabEscape,                       // pointer to an array of strings
        .onOptionChange = 0,
    },
    /*
    {
        .option_kind = OPTKIND_STRING,                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabOptions_OffOn) / 4,                   // number of values for this option
        .option_val = 0,                                             // value of this option
        .menu = 0,                                                   // pointer to the menu that pressing A opens
        .option_name = "Auto Reset",                                 // pointer to a string
        .desc = "Automatically reset after the CPU is\nactionable.", // string describing what this option does
        .option_values = LabOptions_OffOn,                           // pointer to an array of strings
        .onOptionChange = 0,
    },
*/
    {
        .option_kind = OPTKIND_STRING,                // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_CPUBehave) / 4, // number of values for this option
        .option_val = 0,                              // value of this option
        .menu = 0,                                    // pointer to the menu that pressing A opens
        .option_name = "Behavior",                    // pointer to a string
        .desc = "Adjust the CPU's default action.",   // string describing what this option does
        .option_values = LabValues_CPUBehave,         // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                                     // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_CounterGround) / 4,                                  // number of values for this option
        .option_val = 1,                                                                   // value of this option
        .menu = 0,                                                                         // pointer to the menu that pressing A opens
        .option_name = "Counter Action (Ground)",                                          // pointer to a string
        .desc = "Select the action to be performed after a\ngrounded CPU's hitstun ends.", // string describing what this option does
        .option_values = LabValues_CounterGround,                                          // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                                      // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_CounterAir) / 4,                                      // number of values for this option
        .option_val = 4,                                                                    // value of this option
        .menu = 0,                                                                          // pointer to the menu that pressing A opens
        .option_name = "Counter Action (Air)",                                              // pointer to a string
        .desc = "Select the action to be performed after an\nairborne CPU's hitstun ends.", // string describing what this option does
        .option_values = LabValues_CounterAir,                                              // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_CounterShield) / 4,                            // number of values for this option
        .option_val = 1,                                                             // value of this option
        .menu = 0,                                                                   // pointer to the menu that pressing A opens
        .option_name = "Counter Action (Shield)",                                    // pointer to a string
        .desc = "Select the action to be performed after the\nCPU's shield is hit.", // string describing what this option does
        .option_values = LabValues_CounterShield,                                    // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_INT,                                                  // the type of option this is; menu, string list, integer list, etc
        .value_num = 100,                                                            // number of values for this option
        .option_val = 0,                                                             // value of this option
        .menu = 0,                                                                   // pointer to the menu that pressing A opens
        .option_name = "Counter After Frames",                                       // pointer to a string
        .desc = "Adjust the amount of actionable frames before \nthe CPU counters.", // string describing what this option does
        .option_values = "%d Frames",                                                // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_INT,                                           // the type of option this is; menu, string list, integer list, etc
        .value_num = 100,                                                     // number of values for this option
        .option_val = 1,                                                      // value of this option
        .menu = 0,                                                            // pointer to the menu that pressing A opens
        .option_name = "Counter After Hits",                                  // pointer to a string
        .desc = "Adjust the amount of hits taken before the \nCPU counters.", // string describing what this option does
        .option_values = "%d Hits",                                           // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_INT,                                                           // the type of option this is; menu, string list, integer list, etc
        .value_num = 100,                                                                     // number of values for this option
        .option_val = 1,                                                                      // value of this option
        .menu = 0,                                                                            // pointer to the menu that pressing A opens
        .option_name = "Counter After Shield Hits",                                           // pointer to a string
        .desc = "Adjust the amount of hits the CPU's shield\nwill take before they counter.", // string describing what this option does
        .option_values = "%d Hits",                                                           // pointer to an array of strings
        .onOptionChange = 0,
    },
};
static EventMenu LabMenu_CPU = {
    .name = "CPU Options",                                      // the name of this menu
    .option_num = sizeof(LabOptions_CPU) / sizeof(EventOption), // number of options this menu contains
    .scroll = 0,                                                // runtime variable used for how far down in the menu to start
    .state = 0,                                                 // bool used to know if this menu is focused, used at runtime
    .cursor = 0,                                                // index of the option currently selected, used at runtime
    .options = &LabOptions_CPU,                                 // pointer to all of this menu's options
    .prev = 0,                                                  // pointer to previous menu, used at runtime
};
// Tech Options
static EventOption LabOptions_Tech[] = {
    {
        .option_kind = OPTKIND_STRING,                                          // the type of option this is; menu, string list, integer list, etc
        .value_num = 2,                                                         // number of values for this option
        .option_val = 0,                                                        // value of this option
        .menu = 0,                                                              // pointer to the menu that pressing A opens
        .option_name = "Tech Invisibility",                                     // pointer to a string
        .desc = "Toggle the CPU turning invisible during tech\nanimations.",    // string describing what this option does
        .option_values = LabOptions_OffOn,                                      // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,
        .value_num = 2,
        .option_val = 0,
        .menu = 0,
        .option_name = "Tech Sound",
        .desc = "Toggle playing a sound when tech is\ndistinguishable.",
        .option_values = LabOptions_OffOn,
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                         // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_Tech) / 4,                               // number of values for this option
        .option_val = 0,                                                       // value of this option
        .menu = 0,                                                             // pointer to the menu that pressing A opens
        .option_name = "Tech Option",                                          // pointer to a string
        .desc = "Adjust what the CPU will do upon colliding\nwith the stage.", // string describing what this option does
        .option_values = LabValues_Tech,                                       // pointer to an array of strings
        .onOptionChange = Lab_ChangeTech,
    },
    {
        .option_kind = OPTKIND_INT,                                 // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                           // number of values for this option
        .option_val = 25,                                           // value of this option
        .menu = 0,                                                  // pointer to the menu that pressing A opens
        .option_name = "Tech in Place Chance",                      // pointer to a string
        .desc = "Adjust the chance the CPU will tech in place.",    // string describing what this option does
        .option_values = "%d%%",                                    // pointer to an array of strings
        .onOptionChange = Lab_ChangeTechInPlaceChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                                 // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                           // number of values for this option
        .option_val = 25,                                           // value of this option
        .menu = 0,                                                  // pointer to the menu that pressing A opens
        .option_name = "Tech Away Chance",                          // pointer to a string
        .desc = "Adjust the chance the CPU will tech away.",        // string describing what this option does
        .option_values = "%d%%",                                    // pointer to an array of strings
        .onOptionChange = Lab_ChangeTechAwayChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                       // number of values for this option
        .option_val = 25,                                       // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "Tech Toward Chance",                    // pointer to a string
        .desc = "Adjust the chance the CPU will tech toward.",  // string describing what this option does
        .option_values = "%d%%",                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeTechTowardChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                       // number of values for this option
        .option_val = 25,                                       // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "Miss Tech Chance",                      // pointer to a string
        .desc = "Adjust the chance the CPU will miss tech.",    // string describing what this option does
        .option_values = "%d%%",                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeMissTechChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                      // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_Getup) / 4,                           // number of values for this option
        .option_val = 0,                                                    // value of this option
        .menu = 0,                                                          // pointer to the menu that pressing A opens
        .option_name = "Get Up Option",                                     // pointer to a string
        .desc = "Adjust what the CPU will do after missing\na tech input.", // string describing what this option does
        .option_values = LabValues_Getup,                                   // pointer to an array of strings
        .onOptionChange = Lab_ChangeGetup,
    },
    {
        .option_kind = OPTKIND_INT,                         // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                   // number of values for this option
        .option_val = 25,                                   // value of this option
        .menu = 0,                                          // pointer to the menu that pressing A opens
        .option_name = "Stand Chance",                      // pointer to a string
        .desc = "Adjust the chance the CPU will stand.",    // string describing what this option does
        .option_values = "%d%%",                            // pointer to an array of strings
        .onOptionChange = Lab_ChangeStandChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                       // number of values for this option
        .option_val = 25,                                       // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "Roll Away Chance",                      // pointer to a string
        .desc = "Adjust the chance the CPU will roll away.",    // string describing what this option does
        .option_values = "%d%%",                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeRollAwayChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                       // number of values for this option
        .option_val = 25,                                       // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "Roll Toward Chance",                    // pointer to a string
        .desc = "Adjust the chance the CPU will roll toward.",  // string describing what this option does
        .option_values = "%d%%",                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeRollTowardChance,
        .disable = 0,
    },
    {
        .option_kind = OPTKIND_INT,                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 101,                                       // number of values for this option
        .option_val = 25,                                       // value of this option
        .menu = 0,                                              // pointer to the menu that pressing A opens
        .option_name = "Getup Attack Chance",                   // pointer to a string
        .desc = "Adjust the chance the CPU will getup attack.", // string describing what this option does
        .option_values = "%d%%",                                // pointer to an array of strings
        .onOptionChange = Lab_ChangeGetupAttackChance,
        .disable = 0,
    },
};
static EventMenu LabMenu_Tech = {
    .name = "Tech Options",
    .option_num = sizeof(LabOptions_Tech) / sizeof(EventOption),
    .scroll = 0,
    .state = 0,
    .cursor = 0,
    .options = &LabOptions_Tech,
    .prev = 0,
};
// Recording
//
// Aitch: Please be aware that the order of these options is important.
// The option idx will be serialized when exported, so loading older replays could load the wrong option if we reorder/remove options.
static char **LabValues_RecordSlot[] = {"Random", "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6"};
static char **LabValues_HMNRecordMode[] = {"Off", "Record", "Playback"};
static char **LabValues_CPURecordMode[] = {"Off", "Control", "Record", "Playback"};
static char **LabValues_AutoRestore[] = {"Off", "Playback Ends", "CPU Counters"};

static const EventOption Record_Save = {
    .option_kind = OPTKIND_FUNC,                                             // the type of option this is; menu, string list, integer list, etc
    .value_num = 0,                                                          // number of values for this option
    .option_val = 0,                                                         // value of this option
    .menu = 0,                                                               // pointer to the menu that pressing A opens
    .option_name = "Save Positions",                                         // pointer to a string
    .desc = "Save the current fighter positions\nas the initial positions.", // string describing what this option does
    .option_values = 0,                                                      // pointer to an array of strings
    .onOptionSelect = Record_InitState,
};

static const EventOption Record_Load = {
    .option_kind = OPTKIND_FUNC,                                                             // the type of option this is; menu, string list, integer list, etc
    .value_num = 0,                                                                          // number of values for this option
    .option_val = 0,                                                                         // value of this option
    .menu = 0,                                                                               // pointer to the menu that pressing A opens
    .option_name = "Restore Positions",                                                      // pointer to a string
    .desc = "Load the saved fighter positions and \nstart the sequence from the beginning.", // string describing what this option does
    .option_values = 0,                                                                      // pointer to an array of strings
    .onOptionSelect = Record_RestoreState,
};

static EventOption LabOptions_Record[] = {
    // swapped between Record_Save and Record_Load
    Record_Save,

    {
        .option_kind = OPTKIND_STRING,                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_HMNRecordMode) / 4,            // number of values for this option
        .option_val = 0,                                             // value of this option
        .menu = 0,                                                   // pointer to the menu that pressing A opens
        .option_name = "HMN Mode",                                   // pointer to a string
        .desc = "Toggle between recording and playback of\ninputs.", // string describing what this option does
        .option_values = LabValues_HMNRecordMode,                    // pointer to an array of strings
        .onOptionChange = Record_ChangeHMNMode,
    },
    {
        .option_kind = OPTKIND_STRING,                                                                                       // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_RecordSlot) / 4,                                                                       // number of values for this option
        .option_val = 1,                                                                                                     // value of this option
        .menu = 0,                                                                                                           // pointer to the menu that pressing A opens
        .option_name = "HMN Record Slot",                                                                                    // pointer to a string
        .desc = "Toggle which recording slot to save inputs \nto. Maximum of 6 and can be set to random \nduring playback.", // string describing what this option does
        .option_values = LabValues_RecordSlot,                                                                               // pointer to an array of strings
        .onOptionChange = Record_ChangeHMNSlot,
    },
    {
        .option_kind = OPTKIND_STRING,                               // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_CPURecordMode) / 4,            // number of values for this option
        .option_val = 0,                                             // value of this option
        .menu = 0,                                                   // pointer to the menu that pressing A opens
        .option_name = "CPU Mode",                                   // pointer to a string
        .desc = "Toggle between recording and playback of\ninputs.", // string describing what this option does
        .option_values = LabValues_CPURecordMode,                    // pointer to an array of strings
        .onOptionChange = Record_ChangeCPUMode,
    },
    {
        .option_kind = OPTKIND_STRING,                                                                                       // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_RecordSlot) / 4,                                                                       // number of values for this option
        .option_val = 1,                                                                                                     // value of this option
        .menu = 0,                                                                                                           // pointer to the menu that pressing A opens
        .option_name = "CPU Record Slot",                                                                                    // pointer to a string
        .desc = "Toggle which recording slot to save inputs \nto. Maximum of 6 and can be set to random \nduring playback.", // string describing what this option does
        .option_values = LabValues_RecordSlot,                                                                               // pointer to an array of strings
        .onOptionChange = Record_ChangeCPUSlot,
    },
    {
        .option_kind = OPTKIND_STRING,                     // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabOptions_OffOn) / 4,         // number of values for this option
        .option_val = 0,                                   // value of this option
        .menu = 0,                                         // pointer to the menu that pressing A opens
        .option_name = "Loop Input Playback",              // pointer to a string
        .desc = "Loop the recorded inputs when they end.", // string describing what this option does
        .option_values = LabOptions_OffOn,                 // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_STRING,                                              // the type of option this is; menu, string list, integer list, etc
        .value_num = sizeof(LabValues_AutoRestore) / 4,                             // number of values for this option
        .option_val = 0,                                                            // value of this option
        .menu = 0,                                                                  // pointer to the menu that pressing A opens
        .option_name = "Auto Restore",                                              // pointer to a string
        .desc = "Automatically restore saved positions.",                           // string describing what this option does
        .option_values = LabValues_AutoRestore,                                     // pointer to an array of strings
        .onOptionChange = 0,
    },
    {
        .option_kind = OPTKIND_FUNC,                                                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                                                          // number of values for this option
        .option_val = 0,                                                                         // value of this option
        .menu = 0,                                                                               // pointer to the menu that pressing A opens
        .option_name = "Re-Save Positions",                                                      // pointer to a string
        .desc = "Save the current position, without\nremoving recorded inputs.",                 // string describing what this option does
        .option_values = 0,                                                                      // pointer to an array of strings
        .onOptionSelect = Record_ResaveState,
    },
    {
        .option_kind = OPTKIND_FUNC,                                                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                                                          // number of values for this option
        .option_val = 0,                                                                         // value of this option
        .menu = 0,                                                                               // pointer to the menu that pressing A opens
        .option_name = "Delete Positions",                                                       // pointer to a string
        .desc = "Delete the current initial position\nand recordings.",                          // string describing what this option does
        .option_values = 0,                                                                      // pointer to an array of strings
        .onOptionSelect = Record_DeleteState,
    },
    {
        .option_kind = OPTKIND_FUNC,                                                             // the type of option this is; menu, string list, integer list, etc
        .value_num = 0,                                                                          // number of values for this option
        .option_val = 0,                                                                         // value of this option
        .menu = 0,                                                                               // pointer to the menu that pressing A opens
        .option_name = "Export",                                                                 // pointer to a string
        .desc = "Export the recording to a memory card\nfor later use or to share with others.", // string describing what this option does
        .option_values = 0,                                                                      // pointer to an array of strings
        .onOptionSelect = Export_Init,
    },
};
static EventMenu LabMenu_Record = {
    .name = "Recording",                                           // the name of this menu
    .option_num = sizeof(LabOptions_Record) / sizeof(EventOption), // number of options this menu contains
    .scroll = 0,                                                   // runtime variable used for how far down in the menu to start
    .state = 0,                                                    // bool used to know if this menu is focused, used at runtime
    .cursor = 0,                                                   // index of the option currently selected, used at runtime
    .options = &LabOptions_Record,                                 // pointer to all of this menu's options
    .prev = 0,                                                     // pointer to previous menu, used at runtime
};

float get_angle_out_of_deadzone(float angle, int lastSDIWasCardinal);
