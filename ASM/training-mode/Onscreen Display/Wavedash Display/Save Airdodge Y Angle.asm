    # To be inserted at 80099b74
    .include "../../Globals.s"
    .include "../../../m-ex/Header.s"

    # Get Airdodge Angle
    mr r3, r31
    branchl r12, Joystick_Angle_Retrieve
    stfs f1, TM_AirdodgeAngle(r31)

    lwz r3, 0x10(r31)
    cmpwi r3, ASID_JumpF
    beq GetHopType
    cmpwi r3, ASID_JumpB
    beq GetHopType

InvalidHopType:
    li r3, 2
    stw r3, TM_ShortOrFullHop(r31)
    b Exit

GetHopType:
    # Get Hop Type
    # read state_var1, which is 0 for fullhop and 1 for shorthop
    lwz r3, 0x2340(r31)
    stw r3, TM_ShortOrFullHop(r31)

Exit:
    addi r3, r29, 0
