    # To be inserted at 800da07c
    .include "../Globals.s"

    .set playerdata, 31

    backupall

    # CHECK IF ENABLED
    li r0, OSD.GrabBreakout # OSD Menu ID
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

    # The grab timer is set at some number when initially grabbed,
    # then decrements by 1 for each frame not being pummelled, and for each input.
    # The grab breaks when the timer reaches zero.
    bl Data
    .float 0.5
    .float 1.0
    .long 0    # initial grab timer
    .long 0    # frame counter for grab
Data:
    mflr r3

    lfs f0, 0x1A4C (playerdata)

    lfs f1, 0x8(r3)
    fcmpo cr0, f0, f1
    ble GrabUpdate

NewGrab:
    stfs f0, 0x8(r3)
    li r4, 0
    stw r4, 0xC(r3)

GrabUpdate:
    # increment frame counter
    lfs f1, 0xC(r3)
    lfs f2, 0x4(r3)
    fadds f1, f1, f2
    stfs f1, 0xC(r3)

    # calculate rounded timer
    # We show the timer as an integer, rounded up, because the fractional part never changes and isn't important.
    # Instead of messing with rounding modes or converting to an integer, 
    # we simply add 0.5 then let c formatting do the rounding.
    lfs f2, 0x0(r3)
    fadds f1, f0, f2

    # calculate mash rate
    lfs f2, 0x8(r3)
    lfs f3, 0xC(r3)
    fsubs f2, f2, f0
    fdivs f2, f2, f3

    li r3, OSD.GrabBreakout
    lbz r4, 0xC(playerdata)
    li r5, MSGCOLOR_WHITE
    bl Text
    mflr r6
    Message_Display
    b Exit

Text:
    blrl
    .string "Grab Breakout\nGrab Timer: %.0f\nMash Rate: %.1f/f"
    .align 2

Exit:
    restoreall
    lbz r0, 0x2226 (r31)
