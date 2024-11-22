# To be inserted at 80099c7C
# Just after determining to start an item throw/zdrop in Interrupt_AS_EscapeAir_Airdodge

    .include "../Globals.s"
    .include "../../m-ex/Header.s"

    .set playerdata, 31
    .set player, 30
    .set isThrow, 29

    # r29 unused in host function,
    # so we can safely move before saving registers
    mr isThrow, r0

    backupall

    # CHECK IF ENABLED
    li r0, OSD.RollAirdodgeInterrupt
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
    li r3, OSD.RollAirdodgeInterrupt
    lbz r4, 0xC(playerdata)
    load r5, MSGCOLOR_WHITE

    cmpwi isThrow, 0
    bne Throw
ZDrop:
    bl Text_ZDrop
    mflr r6
    lhz r7, TM_FramesinCurrentAS(playerdata)
    b Display
Throw:
    bl Text_ItemThrow
    mflr r6
    lhz r7, TM_FramesInPrevASStart(playerdata)

Display:
    Message_Display
    b Exit

Text_ItemThrow:
    blrl
    .string "Airdodge Item Throw\nFrame %d"
    .align 2

Text_ZDrop:
    blrl
    .string "Airdodge Z-Drop\nFrame %d"
    .align 2

Exit:
    restoreall
    cmpwi r0, 0

