  .data
  .global a_1
a_1:
  .word 10

  .text
  .global inc
inc:
  addi  sp, sp, -32
  la    t0, a_1
  lw    t0, 0(t0)
  sw    t0, 0(sp)
  lw    t0, 0(sp)
  li    t1, 1
  add   t0, t0, t1
  sw    t0, 4(sp)
  lw    t0, 4(sp)
  la    t1, a_1
  sw    t0, 0(t1)
  la    t0, a_1
  lw    t0, 0(t0)
  sw    t0, 8(sp)
  lw    a0, 8(sp)
  addi  sp, sp, 32
  ret

  .text
  .global print_a
print_a:
  addi  sp, sp, -32
  la    t0, a_1
  lw    t0, 0(t0)
  sw    t0, 0(sp)
  sw    ra, 4(sp)
  lw    a0, 0(sp)
  call  putint
  li    a0, 10
  call  putch
  lw    ra, 4(sp)
  addi  sp, sp, 32
  ret

  .text
  .global main
main:
  addi  sp, sp, -96
  li    t0, 0
  sw    t0, 0(sp)
  j     while_entry0

while_entry0:
  lw    t0, 0(sp)
  sw    t0, 4(sp)
  lw    t0, 4(sp)
  li    t1, 10
  slt   t0, t0, t1
  sw    t0, 8(sp)
  lw    t0, 8(sp)
  bnez  t0, while_body0
  j     while_end0

while_body0:
  sw    ra, 12(sp)
  call  inc
  sw    a0, 16(sp)
  li    t0, 1
  sw    t0, 20(sp)
  lw    t0, 20(sp)
  sw    t0, 24(sp)
  lw    t0, 24(sp)
  li    t1, 2
  add   t0, t0, t1
  sw    t0, 28(sp)
  lw    t0, 28(sp)
  sw    t0, 20(sp)
  lw    t0, 20(sp)
  sw    t0, 32(sp)
  lw    a0, 32(sp)
  call  putint
  li    a0, 10
  call  putch
  call  print_a
  lw    t0, 0(sp)
  sw    t0, 36(sp)
  lw    t0, 36(sp)
  li    t1, 1
  add   t0, t0, t1
  sw    t0, 40(sp)
  lw    t0, 40(sp)
  sw    t0, 0(sp)
  j     while_entry0

while_end0:
  li    a0, 0
  lw    ra, 12(sp)
  addi  sp, sp, 96
  ret

