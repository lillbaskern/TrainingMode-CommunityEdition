    # To be inserted at 800CCAC4
    # This ASM is injected to "800CCAAC:Fighter_IASACheck_AllAerial" function which is called from some air action states' IASA function.
    # So this can affect to other action states' IASA besides "Fall".
    .include "../../Globals.s"

    .set player, 31

    backup

    # fulfill requirements of the static function before branching
    mr r4, player              # store player to r4
    bl Text                    # store the address of OSD text to r6
    mflr r6
    li r10, OSD.ActOoAirborne  # store the OSD kind ID to r10

    branchl r12, 0x8000550c
    b Exit

Text:
    blrl
    .string "Special OoFall\nFrame %d"
    .align 2

Exit:
    restore
    cmpwi r3, 0
