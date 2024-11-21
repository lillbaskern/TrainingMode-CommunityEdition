#ifndef _RECOVERIES_C
#define _RECOVERIES_C

#include "../MexTK/mex.h"

// PUBLIC ######################################################

typedef enum Recover_Ret {
    RECOVER_IN_PROGRESS,
    RECOVER_UNIMPLEMENTED,
    RECOVER_FINISHED,
} Recover_Ret;

Recover_Ret Recover_Think(GOBJ *cpu, GOBJ *hmn);

// PRIVATE ######################################################

static Vec2 Vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.X+b.X, a.Y+b.Y}; }
static Vec2 Vec2_sub(Vec2 a, Vec2 b) { return (Vec2){a.X-b.X, a.Y-b.Y}; }

static GOBJ *recover_debug_gobjs[32] = { 0 };

void Recover_DebugPointThink(GOBJ *gobj) {
    int lifetime = (int)gobj->userdata;
    if (lifetime == 0) {
        GObj_Destroy(gobj);
        return;
    }

    gobj->userdata = lifetime - 1;
}

GOBJ *Recover_DebugPointSet(int id, Vec2 point, int lifetime) {
    if (id >= 32) assert("Recover_DebugPointSet: id too large");

    GOBJ *prev_gobj = recover_debug_gobjs[id];
    if (prev_gobj != 0) GObj_Destroy(prev_gobj);

    GOBJ *gobj = GObj_Create(0, 0, 0);
    recover_debug_gobjs[id] = gobj;

    gobj->userdata = lifetime;

    JOBJ *jobj = JOBJ_LoadJoint(event_vars->menu_assets->arrow);
    jobj->trans = (Vec3){ point.X, point.Y, 0.0 };
    jobj->scale = (Vec3){ 4.0, 4.0, 1.0 };
    GObj_AddObject(gobj, 3, jobj);
    JOBJ *jobj2 = JOBJ_LoadJoint(event_vars->menu_assets->arrow);
    jobj2->trans = (Vec3){ 0.0, 0.0, 0.0 };
    jobj2->scale = (Vec3){ -1.0, 1.0, 1.0 };
    JOBJ_AddChild(jobj, jobj2);
    GObj_AddGXLink(gobj, GXLink_Common, 0, 0);
    //GObj_AddProc(gobj, Recover_DebugPointThink, 4);

    return gobj;
}

// extra computed data, not contained in FighterData, needed for recovery decision making
typedef struct Recover_Data {
    Vec2 ledge;
    int direction; // zero if between ledges
    int jumps;
} Recover_Data;

// decompiled from GetLedgeCoords in Custom Even Code
static void Recover_LedgeCoords(Vec2 coords_out[2]) {
    static char ledge_ids[34][2] = {
        { 0xFF, 0xFF }, { 0xFF, 0xFF }, { 0x03, 0x07 }, { 0x33, 0x36 },
        { 0x03, 0x0D }, { 0x29, 0x45 }, { 0x05, 0x11 }, { 0x09, 0x1A },
        { 0x02, 0x06 }, { 0x15, 0x17 }, { 0x00, 0x00 }, { 0x43, 0x4C },
        { 0x00, 0x00 }, { 0x00, 0x00 }, { 0x0E, 0x0D }, { 0x00, 0x00 },
        { 0x00, 0x05 }, { 0x1E, 0x2E }, { 0x0C, 0x0E }, { 0x02, 0x04 },
        { 0x03, 0x05 }, { 0x00, 0x00 }, { 0x06, 0x12 }, { 0x00, 0x00 },
        { 0xD7, 0xE2 }, { 0x00, 0x00 }, { 0x00, 0x00 }, { 0x00, 0x00 },
        { 0x03, 0x05 }, { 0x03, 0x0B }, { 0x06, 0x10 }, { 0x00, 0x05 },
        { 0x00, 0x02 }, { 0x01, 0x01 },
    };

    int stage_id = Stage_GetExternalID();
    char left_id = ledge_ids[stage_id][0];
    char right_id = ledge_ids[stage_id][1];

    // we add/subtract a bit from the ledge, because we can snap from quite a bit further than the actual ledge
    Vec3 pos;
    Stage_GetLeftOfLineCoordinates(left_id, &pos);
    coords_out[0] = (Vec2) { pos.X, pos.Y };
    Stage_GetRightOfLineCoordinates(right_id, &pos);
    coords_out[1] = (Vec2) { pos.X, pos.Y };
}

bool Recover_IsAirActionable(GOBJ *cpu) {
    FighterData *data = cpu->userdata;
    int state = data->state_id;

    if (ASID_DAMAGEHI1 <= state && state <= ASID_DAMAGEFLYROLL) {
        float hitstun = *((float*)&data->state_var.state_var1);
        if (hitstun != 0.0) return false;
    }

    if (ASID_JUMPF <= state && state <= ASID_FALLAERIALB) return true;

    // TODO iasa

    return false;
}

bool Recover_CanDrift(GOBJ *cpu) {
    FighterData *data = cpu->userdata;
    int state = data->state_id;
    int frame = (int)(data->state.frame / data->state.rate);

    if (ASID_DAMAGEHI1 <= state && state <= ASID_DAMAGEFLYROLL) {
        float hitstun = *((float*)&data->state_var.state_var1);
        return hitstun == 0.0;
    }

    if (state == ASID_ESCAPEAIR)
        return frame >= 35;

    switch (data->kind) {
    case FTKIND_FOX:
        if (341 <= state && state <= 346) return true; // laser
        if (357 <= state && state <= 358) return true; // firefox end
        return false;
    }

    return false;
}

void Recover_ThinkFox(Recover_Data *rec_data, GOBJ *cpu, GOBJ *hmn) {
    FighterData *cpu_data = cpu->userdata;
    FighterData *hmn_data = hmn->userdata;

    #define FOX_UPB_DISTANCE 88.0
    #define FOX_SIDEB_DISTANCE_1_4 27.0
    #define FOX_SIDEB_DISTANCE_2_4 46.0
    #define FOX_SIDEB_DISTANCE_3_4 65.0
    #define FOX_SIDEB_DISTANCE_4_4 83.0

    Vec2 pos = { cpu_data->phys.pos.X, cpu_data->phys.pos.Y };
    int state = cpu_data->state_id;
    int frame = (int)(cpu_data->state.frame / cpu_data->state.rate);

    Vec2 ledge_grab_offset = { -15.0 * rec_data->direction, -7.0 };
    Vec2 ledge_grab_point = Vec2_add(rec_data->ledge, ledge_grab_offset);
    Recover_DebugPointSet(0, ledge_grab_point, 1);
    Vec2 vec_to_ledge_grab = Vec2_sub(ledge_grab_point, pos);

    float dist_to_ledge = sqrtf(
          vec_to_ledge_grab.X * vec_to_ledge_grab.X 
        + vec_to_ledge_grab.Y * vec_to_ledge_grab.Y
    );

    bool double_jump_rising = (state == ASID_JUMPAERIALF || state == ASID_JUMPAERIALB) && cpu_data->phys.self_vel.Y > 0.0;

    // Because the cpu is often set to double jump out of hitstun, 
    // we hardcode it to not upb immediately because we want the counter-action to "complete".
    if (Recover_IsAirActionable(cpu) && !double_jump_rising) {
        if (dist_to_ledge < FOX_UPB_DISTANCE && rec_data->direction != 0) {
            cpu_data->cpu.held |= HSD_BUTTON_B;
            cpu_data->cpu.lstickY = 127;
            return;
        }

        if (rec_data->jumps > 0 && pos.Y < 0.0) {
            cpu_data->cpu.held |= HSD_BUTTON_Y;
            cpu_data->cpu.lstickX = 127 * rec_data->direction;
            return;
        }

        cpu_data->cpu.lstickX = 127 * rec_data->direction;
    } else if (Recover_CanDrift(cpu)) {
        cpu_data->cpu.lstickX = 127 * rec_data->direction;
    } else if (state == 354 && frame >= 41) {
        // pick firefox angle
        cpu_data->cpu.lstickX = (s8)(vec_to_ledge_grab.X * 127.0 / dist_to_ledge);
        cpu_data->cpu.lstickY = (s8)(vec_to_ledge_grab.Y * 127.0 / dist_to_ledge);
    } else {
        // survival DI
        cpu_data->cpu.lstickY = 90;
        cpu_data->cpu.lstickX = 90 * rec_data->direction;
    }
}

Recover_Ret Recover_Think(GOBJ *cpu, GOBJ *hmn) {
    FighterData *cpu_data = cpu->userdata;
    FighterData *hmn_data = hmn->userdata;

    if (cpu_data->phys.air_state == 0) // is grounded
        return RECOVER_FINISHED;
    
    Vec2 ledges[2];
    Recover_LedgeCoords(ledges);
    float xpos = cpu_data->phys.pos.X;

    Recover_Data data;
    data.ledge = ledges[xpos > 0.0];
    if (xpos < ledges[0].X)
        data.direction = 1;
    else if (xpos <= ledges[1].X)
        data.direction = 0;
    else
        data.direction = -1;
    data.jumps = cpu_data->attr.max_jumps - cpu_data->jump.jumps_used;

    switch (cpu_data->kind) {
        case FTKIND_FOX:
            Recover_ThinkFox(&data, cpu, hmn);
            break;
        default:
            return RECOVER_UNIMPLEMENTED;
    }

    return RECOVER_IN_PROGRESS;
}

#endif
