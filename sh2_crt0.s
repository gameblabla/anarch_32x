! SEGA 32X support code for SH2
! by Chilly Willy
! Rom header and SH2 init/exception code - must be first in object list

        .text

! Standard MD Header at 0x000

        .incbin "m68k_crt0.bin", 0, 0x3C0

! Standard Mars Header at 0x3C0

        .ascii  "MOD Player Test "              /* module name */
        .long   0x00000000                      /* version */
        .long   _stext                          /* Source (in ROM) */
        .long   0x00000000                      /* Destination (in SDRAM) */
        .long   _sdata                          /* Size */
        .long   0x06000240                      /* Master SH2 Jump */
        .long   0x06000244                      /* Slave SH2 Jump */
        .long   0x06000000                      /* Master SH2 VBR */
        .long   0x06000120                      /* Slave SH2 VBR */

! Standard MD startup code at 0x3F0

        .incbin "m68k_crt1.bin"


        .data

! Master Vector Base Table at 0x06000000

        .long   mstart      /* Cold Start PC */
        .long   0x0603FF00  /* Cold Start SP */
        .long   mstart      /* Manual Reset PC */
        .long   0x0603FF00  /* Manual Reset SP */
        .long   main_err    /* Illegal instruction */
        .long   0x00000000  /* reserved */
        .long   main_err    /* Invalid slot instruction */
        .long   0x20100400  /* reserved */
        .long   0x20100420  /* reserved */
        .long   main_err    /* CPU address error */
        .long   main_err    /* DMA address error */
        .long   main_err    /* NMI vector */
        .long   main_err    /* User break vector */
        .space  76          /* reserved */
        .long   main_err    /* TRAPA #32 */
        .long   main_err    /* TRAPA #33 */
        .long   main_err    /* TRAPA #34 */
        .long   main_err    /* TRAPA #35 */
        .long   main_err    /* TRAPA #36 */
        .long   main_err    /* TRAPA #37 */
        .long   main_err    /* TRAPA #38 */
        .long   main_err    /* TRAPA #39 */
        .long   main_err    /* TRAPA #40 */
        .long   main_err    /* TRAPA #41 */
        .long   main_err    /* TRAPA #42 */
        .long   main_err    /* TRAPA #43 */
        .long   main_err    /* TRAPA #44 */
        .long   main_err    /* TRAPA #45 */
        .long   main_err    /* TRAPA #46 */
        .long   main_err    /* TRAPA #47 */
        .long   main_err    /* TRAPA #48 */
        .long   main_err    /* TRAPA #49 */
        .long   main_err    /* TRAPA #50 */
        .long   main_err    /* TRAPA #51 */
        .long   main_err    /* TRAPA #52 */
        .long   main_err    /* TRAPA #53 */
        .long   main_err    /* TRAPA #54 */
        .long   main_err    /* TRAPA #55 */
        .long   main_err    /* TRAPA #56 */
        .long   main_err    /* TRAPA #57 */
        .long   main_err    /* TRAPA #58 */
        .long   main_err    /* TRAPA #59 */
        .long   main_err    /* TRAPA #60 */
        .long   main_err    /* TRAPA #61 */
        .long   main_err    /* TRAPA #62 */
        .long   main_err    /* TRAPA #63 */
        .long   main_irq    /* Level 1 IRQ */
        .long   main_irq    /* Level 2 & 3 IRQ's */
        .long   main_irq    /* Level 4 & 5 IRQ's */
        .long   main_irq    /* PWM interupt */
        .long   main_irq    /* Command interupt */
        .long   main_irq    /* H Blank interupt */
        .long   main_irq    /* V Blank interupt */
        .long   main_irq    /* Reset Button */

! Slave Vector Base Table at 0x06000120

        .long   sstart      /* Cold Start PC */
        .long   0x06040000  /* Cold Start SP */
        .long   sstart      /* Manual Reset PC */
        .long   0x06040000  /* Manual Reset SP */
        .long   slav_err    /* Illegal instruction */
        .long   0x00000000  /* reserved */
        .long   slav_err    /* Invalid slot instruction */
        .long   0x20100400  /* reserved */
        .long   0x20100420  /* reserved */
        .long   slav_err    /* CPU address error */
        .long   slav_err    /* DMA address error */
        .long   slav_err    /* NMI vector */
        .long   slav_err    /* User break vector */
        .space  76          /* reserved */
        .long   slav_err    /* TRAPA #32 */
        .long   slav_err    /* TRAPA #33 */
        .long   slav_err    /* TRAPA #34 */
        .long   slav_err    /* TRAPA #35 */
        .long   slav_err    /* TRAPA #36 */
        .long   slav_err    /* TRAPA #37 */
        .long   slav_err    /* TRAPA #38 */
        .long   slav_err    /* TRAPA #39 */
        .long   slav_err    /* TRAPA #40 */
        .long   slav_err    /* TRAPA #41 */
        .long   slav_err    /* TRAPA #42 */
        .long   slav_err    /* TRAPA #43 */
        .long   slav_err    /* TRAPA #44 */
        .long   slav_err    /* TRAPA #45 */
        .long   slav_err    /* TRAPA #46 */
        .long   slav_err    /* TRAPA #47 */
        .long   slav_err    /* TRAPA #48 */
        .long   slav_err    /* TRAPA #49 */
        .long   slav_err    /* TRAPA #50 */
        .long   slav_err    /* TRAPA #51 */
        .long   slav_err    /* TRAPA #52 */
        .long   slav_err    /* TRAPA #53 */
        .long   slav_err    /* TRAPA #54 */
        .long   slav_err    /* TRAPA #55 */
        .long   slav_err    /* TRAPA #56 */
        .long   slav_err    /* TRAPA #57 */
        .long   slav_err    /* TRAPA #58 */
        .long   slav_err    /* TRAPA #59 */
        .long   slav_err    /* TRAPA #60 */
        .long   slav_err    /* TRAPA #61 */
        .long   slav_err    /* TRAPA #62 */
        .long   slav_err    /* TRAPA #63 */
        .long   slav_irq    /* Level 1 IRQ */
        .long   slav_irq    /* Level 2 & 3 IRQ's */
        .long   slav_irq    /* Level 4 & 5 IRQ's */
        .long   slav_irq    /* PWM interupt */
        .long   slav_irq    /* Command interupt */
        .long   slav_irq    /* H Blank interupt */
        .long   slav_irq    /* V Blank interupt */
        .long   slav_irq    /* Reset Button */

! The main SH2 starts here at 0x06000240

mstart:
        bra     mcont
        nop

! The slave SH2 starts here at 0x06000244

sstart:
        bra     scont
        nop

! Each section of code below has its own data table so that the code
! can be extended without worrying about the offsets becoming too big.
! This results in duplicate entries, but not so many that we care. :)

mcont:
! clear interrupt flags
        mov.l   _master_int_clr,r1
        mov.w   r0,@-r1     /* PWM INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* CMD INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* H INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* V INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* VRES INT clear */
        mov.w   r0,@r1

        mov.l   _master_stk,r15
! purge cache and turn it off
        mov.l   _master_cctl,r0
        mov     #0x10,r1
        mov.b   r1,@r0

! clear bss
        mov     #0,r0
        mov.l   _master_bss_start,r1
        mov.l   _master_bss_end,r2
0:
        mov.l   r0,@r1
        cmp/eq  r1,r2
        bf/s    0b
        add     #4,r1

! wait for 68000 to finish init
        mov.l   _master_sts,r0
        mov.l   _master_ok,r1
1:
        mov.l   @r0,r2
        nop
        nop
        cmp/eq  r1,r2
        bt      1b

! let Slave SH2 run
        mov     #0,r1
        mov.l   r1,@(4,r0)  /* clear slave status */

        mov     #0x80,r0
        mov.l   _master_adapter,r1
        mov.b   r0,@r1      /* set FM */
        mov     #0x00,r0
        mov.b   r0,@(1,r1)  /* set int enables */
        mov     #0x20,r0
        ldc     r0,sr       /* allow ints */

! purge cache, turn it on, and run main()
        mov.l   _master_cctl,r0
        mov     #0x11,r1
        mov.b   r1,@r0
        mov.l   _master_go,r0
        jmp     @r0
        nop

        .align   2
_master_int_clr:
        .long   0x2000401E  /* one word passed last int clr reg */
_master_stk:
        .long   0x0603FF00  /* Cold Start SP */
_master_sts:
        .long   0x20004020
_master_ok:
        .ascii  "M_OK"
_master_adapter:
        .long   0x20004000
_master_cctl:
        .long   0xFFFFFE92
_master_go:
        .long   _main

_master_bss_start:
        .long   __bss_start
_master_bss_end:
        .long   __end

scont:
! clear interrupt flags
        mov.l   _slave_int_clr,r1
        mov.w   r0,@-r1     /* PWM INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* CMD INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* H INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* V INT clear */
        mov.w   r0,@r1
        mov.w   r0,@-r1     /* VRES INT clear */
        mov.w   r0,@r1

        mov.l   _slave_stk,r15
! wait for Master SH2 and 68000 to finish init
        mov.l   _slave_sts,r0
        mov.l   _slave_ok,r1
1:
        mov.l   @r0,r2
        nop
        nop
        cmp/eq  r1,r2
        bt      1b

        mov.l   _slave_adapter,r1
        mov     #0x00,r0
        mov.b   r0,@(1,r1)  /* set int enables (different from master despite same address!) */
        mov     #0x20,r0
        ldc     r0,sr       /* IPL = 2 */

! purge cache, turn it on, and run slave()
        mov.l   _slave_cctl,r0
        mov     #0x11,r1
        mov.b   r1,@r0
        mov.l   _slave_go,r0
        jmp     @r0
        nop

        .align   2
_slave_int_clr:
        .long   0x2000401E  /* one word passed last int clr reg */
_slave_stk:
        .long   0x06040000  /* Cold Start SP */
_slave_sts:
        .long   0x20004024
_slave_ok:
        .ascii  "S_OK"
_slave_adapter:
        .long   0x20004000
_slave_cctl:
        .long   0xFFFFFE92
_slave_go:
        .long   _slave

! Master exception handler

main_err:
        rte
        nop

! Master IRQ handler

main_irq:
        mov.l   r0,@-r15

        stc     sr,r0       /* SR holds IRQ level in I3-I0 */
        shlr2   r0
        and     #0x38,r0
        cmp/eq  #0x28,r0
        bt      main_h_irq
        cmp/eq  #0x18,r0
        bt      main_pwm_irq
        cmp/eq  #0x30,r0
        bt      main_v_irq
        cmp/eq  #0x20,r0
        bt      main_cmd_irq
        cmp/eq  #0x38,r0
        bt      main_vres_irq

        mov.l   @r15+,r0
        rte
        nop

main_v_irq:
        mov.l   r1,@-r15

        mov.l   mvi_mars_adapter,r1
        mov.w   r0,@(0x16,r1)   /* clear V IRQ */
        nop
        nop
        nop
        nop

        ! handle V IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
mvi_mars_adapter:
        .long   0x20004000

main_h_irq:
        mov.l   r1,@-r15

        mov.l   mhi_mars_adapter,r1
        mov.w   r0,@(0x18,r1)   /* clear H IRQ */
        nop
        nop
        nop
        nop

        ! handle H IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
mhi_mars_adapter:
        .long   0x20004000

main_cmd_irq:
        mov.l   r1,@-r15

        mov.l   mci_mars_adapter,r1
        mov.w   r0,@(0x1A,r1)   /* clear CMD IRQ */
        nop
        nop
        nop
        nop

        ! handle CMD IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
mci_mars_adapter:
        .long   0x20004000

main_pwm_irq:
        mov.l   r1,@-r15

        mov.l   mpi_mars_adapter,r1
        mov.w   r0,@(0x1C,r1)   /* clear PWM IRQ */
        nop
        nop
        nop
        nop

        ! handle PWM IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
mpi_mars_adapter:
        .long   0x20004000

main_vres_irq:
        mov.l   mvri_mars_adapter,r1
        mov.w   r0,@(0x14,r1)   /* clear VRES IRQ */
        nop
        nop
        nop
        nop

        mov     #0x0F,r0
        shll2   r0
        shll2   r0
        ldc     r0,sr       /* disallow ints */

        mov.l   mvri_master_stk,r15
        mov.l   mvri_master_vres,r0
        jmp     @r0
        nop

        .align  2
mvri_mars_adapter:
        .long   0x20004000
mvri_master_stk:
        .long   0x0603FC00  /* Cold Start SP */
mvri_master_vres:
        .long   main_reset

! Slave exception handler

slav_err:
        rte
        nop

! Slave IRQ handler

slav_irq:
        mov.l   r0,@-r15

        stc     sr,r0       /* SR holds IRQ level I3-I0 */
        shlr2   r0
        and     #0x38,r0    /* I3-I1 */
        cmp/eq  #0x28,r0
        bt      slav_h_irq
        cmp/eq  #0x18,r0
        bt      slav_pwm_irq
        cmp/eq  #0x30,r0
        bt      slav_v_irq
        cmp/eq  #0x20,r0
        bt      slav_cmd_irq
        cmp/eq  #0x38,r0
        bt      slav_vres_irq

        mov.l   @r15+,r0
        rte
        nop

slav_v_irq:
        mov.l   r1,@-r15

        mov.l   svi_mars_adapter,r1
        mov.w   r0,@(0x16,r1)   /* clear V IRQ */
        nop
        nop
        nop
        nop

        ! handle V IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
svi_mars_adapter:
        .long   0x20004000

slav_h_irq:
        mov.l   r1,@-r15

        mov.l   shi_mars_adapter,r1
        mov.w   r0,@(0x18,r1)   /* clear H IRQ */
        nop
        nop
        nop
        nop

        ! handle H IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
shi_mars_adapter:
        .long   0x20004000

slav_cmd_irq:
        mov.l   r1,@-r15

        mov.l   sci_mars_adapter,r1
        mov.w   r0,@(0x1A,r1)   /* clear CMD IRQ */
        nop
        nop
        nop
        nop

        ! handle CMD IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
sci_mars_adapter:
        .long   0x20004000

slav_pwm_irq:
        mov.l   r1,@-r15

        mov.l   spi_mars_adapter,r1
        mov.w   r0,@(0x1C,r1)   /* clear PWM IRQ */
        nop
        nop
        nop
        nop

        ! handle PWM IRQ

        mov.l   @r15+,r1
        mov.l   @r15+,r0
        rte
        nop

        .align  2
spi_mars_adapter:
        .long   0x20004000

slav_vres_irq:
        mov.l   svri_mars_adapter,r1
        mov.w   r0,@(0x14,r1)   /* clear VRES IRQ */
        nop
        nop
        nop
        nop

        mov     #0x0F,r0
        shll2   r0
        shll2   r0
        ldc     r0,sr       /* disallow ints */

        mov.l   svri_slave_stk,r15
        mov.l   svri_slave_vres,r0
        jmp     @r0
        nop

        .align  2
svri_mars_adapter:
        .long   0x20004000
svri_slave_stk:
        .long   0x06040000  /* Cold Start SP */
svri_slave_vres:
        .long   slav_reset

! Fast memcpy function - copies longs, runs from sdram for speed
! On entry: r4 = dst, r5 = src, r6 = len (in longs)

        .align  4
        .global _fast_memcpy
_fast_memcpy:
        mov.l   @r5+,r3
        mov.l   r3,@r4
        dt      r6
        bf/s    _fast_memcpy
        add     #4,r4
        rts
        nop

! Cache clear line function
! On entry: r4 = ptr - should be 16 byte aligned

        .align  4
        .global _CacheClearLine
_CacheClearLine:
        mov.l   _cache_flush,r0
        or      r0,r4
        mov     #0,r0
        mov.l   r0,@r4
        rts
        nop

        .align  2

_cache_flush:
        .long   0x40000000

! Cache control function
! On entry: r4 = cache mode => 0x10 = CP, 0x08 = TW, 0x01 = CE

        .align  4
        .global _CacheControl
_CacheControl:
        mov.l   _sh2_cctl,r0
        mov.b   r4,@r0
        rts
        nop

        .align  2

_sh2_cctl:
        .long   0xFFFFFE92

! void MixSamples(void *mixer, int16_t *buffer, int32_t cnt, int32_t scale);
! On entry: r4 = mixer pointer
!           r5 = buffer pointer
!           r6 = count (number of stereo 16-bit samples)
!           r7 = scale (global volume - possibly fading, 0 - 64)
        .align  4
        .global _MixSamples
_MixSamples:
        mov.l   r8,@-r15
        mov.l   r9,@-r15
        mov.l   r10,@-r15
        mov.l   r11,@-r15
        mov.l   r12,@-r15
        mov.l   r13,@-r15
        mov.l   r14,@-r15

        mov.l   @r4,r8          /* data pointer */
        mov.l   @(4,r4),r9      /* position */
        mov.l   @(8,r4),r10     /* increment */
        mov.l   @(12,r4),r11    /* length */
        mov.l   @(16,r4),r12    /* loop_length */
        mov.w   @(20,r4),r0     /* volume:pan */

        /* calculate left/right volumes from volume, pan, and scale */
        mov     r0,r13
        shlr8   r13
        extu.b  r13,r13         /* ch_vol */
        mov     r13,r14
        extu.b  r0,r0           /* pan */

        .ifdef  INTENSITY_CROSSFADE
        mov     #0xFF,r1
        extu.b  r1,r1
        sub     r0,r1           /* 255 - pan */
        mulu.w  r0,r0
        sts     macl,r0         /* pan**2 */
        mulu.w  r0,r14
        sts     macl,r0         /* pan**2 * ch_vol */
        mul.l   r0,r7
        sts     macl,r14        /* pan**2 * ch_vol * scale */
        shlr16  r14
!       shlr2   r14
        shlr2   r14             /* right volume = pan**2 * ch_vol * scale / 256 / 64 / 64 */

        mulu.w  r1,r1
        sts     macl,r0         /* (255 - pan)**2 */
        mulu.w  r0,r13
        sts     macl,r0         /* (255 - pan)**2 * ch_vol */
        mul.l   r0,r7
        sts     macl,r13        /* (255 - pan)**2 * ch_vol * scale */
        shlr16  r13
!       shlr2   r13
        shlr2   r13             /* left volume = (255 - pan)**2 * ch_vol * scale / 256 / 64 / 64 */
        .endif

        .ifdef  LINEAR_CROSSFADE
        mov     #0xFF,r1
        extu.b  r1,r1
        sub     r0,r1           /* 255 - pan */
        mulu.w  r0,r14
        sts     macl,r0         /* pan * ch_vol */
        mul.l   r0,r7
        sts     macl,r14        /* pan * ch_vol * scale */
        shlr8   r14
!       shlr2   r14
        shlr2   r14             /* right volume = pan * ch_vol * scale / 64 / 64 */

        mulu.w  r1,r13
        sts     macl,r0         /* (255 - pan) * ch_vol */
        mul.l   r0,r7
        sts     macl,r13        /* (255 - pan) * ch_vol * scale */
        shlr8   r13
!       shlr2   r13
        shlr2   r13             /* left volume = (255 - pan) * ch_vol * scale / 64 / 64 */
        .endif

        .ifdef  EQUAL_DISTANCE_CROSSFADE
        mov     r0,r1
        mov.l   edc_left_ptr,r2
        mov.b   @(r0,r2),r0
        extu.b  r0,r0           /* epan = edc_left[pan] */
        mulu.w  r0,r13
        sts     macl,r0         /* epan * ch_vol */
        mul.l   r0,r7
        sts     macl,r13        /* epan * ch_vol * scale */
        shlr8   r13
!       shlr2   r13
        shlr2   r13             /* left volume = epan * ch_vol * scale / 64 / 64 */

        mov     r1,r0
        mov.l   edc_right_ptr,r2
        mov.b   @(r0,r2),r1
        extu.b  r1,r1           /* epan = edc_right[pan] */
        mulu.w  r1,r14
        sts     macl,r0         /* epan * ch_vol */
        mul.l   r0,r7
        sts     macl,r14        /* epan * ch_vol * scale */
        shlr8   r14
!       shlr2   r14
        shlr2   r14             /* right volume = epan * ch_vol * scale / 64 / 64 */
        .endif

        /* mix r6 stereo samples */
mix_loop:
        /* process one sample */
        mov     r9,r0
        shlr8   r0
        shll2   r0
        shlr8   r0
        mov.b   @(r0,r8),r3
        /* scale sample for left output */
        muls.w  r3,r13
        mov.w   @r5,r1
        sts     macl,r0
        shlr8   r0
        exts.w  r0,r0
        add     r0,r1
        mov.w   r1,@r5
        add     #2,r5
        /* scale sample for right output */
        muls.w  r3,r14
        mov.w   @r5,r1
        sts     macl,r0
        shlr8   r0
        exts.w  r0,r0
        add     r0,r1
        mov.w   r1,@r5
        add     #2,r5

        /* advance position and check for loop */
        add     r10,r9                  /* position += increment */
mix_chk:
        cmp/hs  r11,r9
        bt      mix_wrap                /* position >= length */
mix_next:
        /* next sample */
        dt      r6
        bf      mix_loop
        bra     mix_exit
        mov.l   r9,@(4,r4)              /* update position field */

mix_wrap:
        /* check if loop sample */
        mov     r12,r0
        cmp/eq  #0,r0
        bf/s    mix_chk                 /* loop sample */
        sub     r12,r9                  /* position -= loop_length */
        /* sample done playing */
        mov.l   r12,@r4                 /* clear data pointer field */

mix_exit:
        mov.l   @r15+,r14
        mov.l   @r15+,r13
        mov.l   @r15+,r12
        mov.l   @r15+,r11
        mov.l   @r15+,r10
        mov.l   @r15+,r9
        mov.l   @r15+,r8
        rts
        nop

! void LockMixer(int16_t id)
! Entry: r4 = id

        .global _LockMixer
_LockMixer:
        exts.w  r4,r4
        mov.l   mixer_state,r1
0:
        mov.w   @r1,r0
        cmp/eq  #1,r0                   /* loop until unlocked */
        bf      0b

        mov.w   r4,@r1
        mov.w   @r1,r0
        cmp/eq  r4,r0
        bf      0b                      /* race condition - we lost */

        rts
        nop

! void UnlockMixer(void)

        .global _UnlockMixer
_UnlockMixer:
        mov     #1,r0
        mov.l   mixer_state,r1
        rts
        mov.w   r0,@r1

        .align  2

        .equ    MARS_SYS_COMM6, 0x20004026

mixer_state:
        .long   MARS_SYS_COMM6

        .ifdef  EQUAL_DISTANCE_CROSSFADE
edc_left_ptr:
        .long   edc_left
edc_right_ptr:
        .long   edc_right
        .include "edc_table.i"
        .endif

        .text

main_reset:
! do any master SH2 specific reset code here

        mov.l   slav_st,r0
        mov.l   slav_ok,r1
0:
        mov.l   @r0,r2
        nop
        nop
        cmp/eq  r1,r2
        bf      0b          /* wait for slave */

        ! recopy rom data to sdram

        mov.l   rom_header,r1
        mov.l   @r1,r2      /* src relative to start of rom */
        mov.l   @(4,r1),r3  /* dst relative to start of sdram */
        mov.l   @(8,r1),r4  /* size (longword aligned) */
        mov.l   rom_start,r1
        add     r1,r2
        mov.l   sdram_start,r1
        add     r1,r3
        shlr2   r4          /* number of longs */
        add     #-1,r4
1:
        mov.l   @r2+,r0
        mov.l   r0,@r3
        add     #4,r3
        dt      r4
        bf      1b

        mov.l   main_st,r0
        mov.l   main_ok,r1
        mov.l   r1,@r0      /* tell everyone reset complete */

        mov.l   main_go,r0
        jmp     @r0
        nop

slav_reset:
! do any slave SH2 specific reset code here

        mov.l   slav_st,r0
        mov.l   slav_ok,r1
        mov.l   r1,@r0      /* tell master to start reset */

        mov.l   main_st,r0
        mov.l   main_ok,r1
0:
        mov.l   @r0,r2
        nop
        nop
        cmp/eq  r1,r2
        bf      0b          /* wait for master to do the work */

        mov.l   slav_go,r0
        jmp     @r0
        nop

        .align  2
main_st:
        .long   0x20004020
main_ok:
        .ascii  "M_OK"
main_go:
        .long   mstart
rom_header:
        .long   0x220003D4
rom_start:
        .long   0x22000000
sdram_start:
        .long   0x26000000

slav_st:
        .long   0x20004024
slav_ok:
        .ascii  "S_OK"
slav_go:
        .long   sstart


        .global _start
_start:

