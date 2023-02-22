        .text

        .align  2
fileName1:
        .asciz  "sfx_click.raw.32x"
fileName2:
        .asciz  "sfx_door.raw.32x"
fileName3:
        .asciz  "sfx_explosion.raw.32x"

fileName4:
        .asciz  "sfx_monster.raw.32x"
fileName5:
        .asciz  "sfx_noise.raw.32x"
fileName6:
        .asciz  "sfx_plasma.raw.32x"
fileName7:
        .asciz  "sfx_shot.raw.32x"


        .align  4
file1:
        .incbin "snds/sfx_click.raw.32x"
fileEnd1:

        .align  4
file2:
        .incbin "snds/sfx_door.raw.32x"
fileEnd2:

        .align  4
file3:
        .incbin "snds/sfx_explosion.raw.32x"
fileEnd3:


        .align  4
file4:
        .incbin "snds/sfx_monster.raw.32x"
fileEnd4:

        .align  4
file5:
        .incbin "snds/sfx_noise.raw.32x"
fileEnd5:

        .align  4
file6:
        .incbin "snds/sfx_plasma.raw.32x"
fileEnd6:

        .align  4
file7:
        .incbin "snds/sfx_shot.raw.32x"
fileEnd7:

        .align  4

        .global _fileName
_fileName:
        .long   fileName1
        .long   fileName2
        .long   fileName3
        .long   fileName4
        .long   fileName5
        .long   fileName6
        .long   fileName7

        
        .global _fileSize
_fileSize:
        .long   fileEnd1 - file1
        .long   fileEnd2 - file2
        .long   fileEnd3 - file3
        .long   fileEnd4 - file4
        .long   fileEnd5 - file5
        .long   fileEnd6 - file6
        .long   fileEnd7 - file7
        
        .global _filePtr
_filePtr:
        .long   file1
        .long   file2
        .long   file3
        .long   file4
        .long   file5
        .long   file6
        .long   file7

        .align  4
