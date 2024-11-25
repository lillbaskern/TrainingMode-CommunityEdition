    # To be inserted at 8011bebc

    .include "../Globals.s"
    .include "../../m-ex/Header.s"

    .set playerdata, 31

    backupall

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

    # DISPLAY OSD
    li r3, OSD.FighterSpecificTech
    lbz r4, 0xC(playerdata)

    bl Text
    mflr r6
    lhz r7, 0x23F8(playerdata)

    cmpwi r7, 15
    bgt Exit
    li r5, MSGCOLOR_RED
    cmpwi r7, 1
    bne Display
    li r5, MSGCOLOR_GREEN

Display:
    Message_Display
    b Exit

Text:
    blrl
    .string "Act OoFloat\nFrame %d"
    .align 2

Exit:
    restoreall
    lfs	f1, 0x2230 (r31)
