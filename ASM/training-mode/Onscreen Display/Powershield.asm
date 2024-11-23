    # To be inserted at 800776a0
    # This file was extracted from the dolphin ASM disassembly in Training Mode Beta v2.0.
    # I believe UnclePunch forgot to commit this OSD to the repo.

    .include "../Globals.s"
    .include "../../m-ex/Header.s"

    .set playerdata, 31

    backupall
    mr playerdata, r5

    # CHECK IF ENABLED
    li r0, OSD.Powershield
    lwz r4, -0x77C0(r13)
    lwz r4, 0x1F24(r4)
    li r3, 1
    slw r0, r3, r0
    and. r0, r0, r4
    beq Exit

    # CHECK FOR FOLLOWER
    mr r3, playerdata
    branchl r12, 0x80005510
    cmpwi r3, 0x1
    beq Exit

    lwz r3, 0x10(r31)
    cmpwi r3, 178
    beq MissedPowershield_Type2
    cmpwi r3, 341
    beq MissedPowershield_Type2
    cmpwi r3, 182
    beq MissedPowershield_Type1
    cmpwi r3, 345
    beq MissedPowershield_Type1
    b Exit

MissedPowershield_Type1:
    li r3, OSD.Powershield
    lbz r4, 0xC(playerdata)
    li r5, MSGCOLOR_RED

    lbz r6, 0x2218(playerdata)
    rlwinm. r6, r6, 0, 27, 27
    beq ReflectEnded
Early:
    bl Text_MissedPowershield_Early
    mflr r6

    lfs f1, 0x2340(playerdata)
    fctiwz f1, f1
    stfd f1, 0x00F0(sp)
    lwz r7, 0x00F4(sp)
    addi r7, r7, 1
    b Display
ReflectEnded:
    bl Text_MissedPowershield_ReflectEnded
    mflr r6

    lis r7, 0xFFA2
    stw r7, 0x0030 (r29)
    lfs f1, 0x2340 (r31)
    fctiwz f1, f1
    stfd f1, 0x00F0 (sp)
    lwz r7, 0x00F4 (sp)
    subi r7, r7, 1
    b Display

MissedPowershield_Type2:
    li r3, OSD.Powershield
    lbz r4, 0xC(playerdata)
    li r5, MSGCOLOR_RED
    bl Text_MissedPowershield_NoHardPress
    mflr r6
    b Display

Display:
    Message_Display

    # Set top text white
    lwz r3, 0x2C(r3)
    lwz r3, MsgData_Text(r3)
    li r4, 0
    bl Color_White
    mflr r5
    branchl r12, Text_ChangeTextColor
    b Exit

Text_MissedPowershield_Early:
    blrl
    .string "Missed Powershield\nEarly: %df"
    .align 2
    blrl
Text_MissedPowershield_ReflectEnded:
    blrl
    .string "Missed Powershield\nReflect Ended: %df"
    .align 2
Text_MissedPowershield_NoHardPress:
    blrl
    .string "Missed Powershield\nNo Hard Press"
    .align 2

Color_White:
    blrl
    .long 0xFFFFFFFF

Exit:
    restoreall
    mr r28, r4
