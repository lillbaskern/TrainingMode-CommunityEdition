    # To be inserted at 80096d44
    # There is potential for a general purpose special move->NIL OSD here

    .include "../Globals.s"
    .include "../../m-ex/Header.s"

    .set playerdata, 31
    .set player, 30

    backupall
    mr playerdata, r3

    # CHECK IF ENABLED
    li r0, OSD.FighterSpecificTech
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

    lwz r3, 0x4(playerdata)
    cmpwi r3, Bowser.Int
    bne Exit

    # ensure whirling fortress air was the previous action state
    lhz r3, 0x23FC(playerdata)
    cmpwi r3, 360
    bne Exit

    lfs f1, 0x894(r31)
    fctiwz f1, f1
    stfd f1, 0x00F0 (sp)
    lwz r7, 0x00F4 (sp)
    addi r7, r7, 1

    li r3, OSD.FighterSpecificTech
    lbz r4, 0xC(playerdata)

    cmpwi r7, 1
    bne MissedNIL
SuccessfulNIL:
    li r5, MSGCOLOR_GREEN
    bl Text_SuccessfulNIL
    mflr r6
    b Display
MissedNIL:
    li r5, MSGCOLOR_RED
    bl Text_MissedNIL
    mflr r6

Display:
    Message_Display
    b Exit

Text_SuccessfulNIL:
    blrl
    .string "Successful NIL\nSp Fall Frames: %d"
    .align 2

Text_MissedNIL:
    blrl
    .string "Missed NIL:\nSp Fall Frames: %d"
    .align 2

Exit:
    restoreall
    lwz r0, 0x2350 (r3)
