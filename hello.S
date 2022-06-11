  .text
  .global init
init:
  addi  sp, sp, -208
  sw    a0, 0(sp)
  lw    t0, 0(sp)
  sw    t0, 4(sp)
  li    t0, 0
  sw    t0, 8(sp)
  j     while_entry0

while_entry0:
  lw    t0, 8(sp)
  sw    t0, 12(sp)
  lw    t0, 12(sp)
  li    t1, 10
  slt   t0, t0, t1
  sw    t0, 16(sp)
  lw    t0, 16(sp)
  bnez  t0, while_body0
  j     while_end0

while_body0:
  li    t0, 0
  sw    t0, 20(sp)
  j     while_entry1

while_end0:
  addi  sp, sp, 208
  ret

while_entry1:
  lw    t0, 20(sp)
  sw    t0, 24(sp)
  lw    t0, 24(sp)
  li    t1, 10
  slt   t0, t0, t1
  sw    t0, 28(sp)
  lw    t0, 28(sp)
  bnez  t0, while_body1
  j     while_end1

while_body1:
  li    t0, 0
  sw    t0, 32(sp)
  j     while_entry2

while_end1:
  lw    t0, 8(sp)
  sw    t0, 36(sp)
  lw    t0, 36(sp)
  li    t1, 1
  add   t0, t0, t1
  sw    t0, 40(sp)
  lw    t0, 40(sp)
  sw    t0, 8(sp)
  j     while_entry0

while_entry2:
  lw    t0, 32(sp)
  sw    t0, 44(sp)
  lw    t0, 44(sp)
  li    t1, 10
  slt   t0, t0, t1
  sw    t0, 48(sp)
  lw    t0, 48(sp)
  bnez  t0, while_body2
  j     while_end2

while_body2:
  lw    t0, 4(sp)
  sw    t0, 52(sp)
  lw    t0, 8(sp)
  sw    t0, 56(sp)
  addi  t0, sp, 52
  lw    t0, 0(t0)
  lw    t1, 56(sp)
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 60(sp)
  lw    t0, 20(sp)
  sw    t0, 64(sp)
  addi  t0, sp, 60
  lw    t0, 0(t0)
  lw    t1, 64(sp)
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 68(sp)
  lw    t0, 32(sp)
  sw    t0, 72(sp)
  addi  t0, sp, 68
  lw    t0, 0(t0)
  lw    t1, 72(sp)
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 76(sp)
  lw    t0, 76(sp)
  lw    t0, 0(t0)
  sw    t0, 80(sp)
  lw    t0, 8(sp)
  sw    t0, 84(sp)
  lw    t0, 84(sp)
  li    t1, 100
  mul   t0, t0, t1
  sw    t0, 88(sp)
  lw    t0, 20(sp)
  sw    t0, 92(sp)
  lw    t0, 92(sp)
  li    t1, 10
  mul   t0, t0, t1
  sw    t0, 96(sp)
  lw    t0, 88(sp)
  lw    t1, 96(sp)
  add   t0, t0, t1
  sw    t0, 100(sp)
  lw    t0, 32(sp)
  sw    t0, 104(sp)
  lw    t0, 100(sp)
  lw    t1, 104(sp)
  add   t0, t0, t1
  sw    t0, 108(sp)
  lw    t0, 108(sp)
  lw    t1, 76(sp)
  sw    t0, 0(t1)
  lw    t0, 32(sp)
  sw    t0, 112(sp)
  lw    t0, 112(sp)
  li    t1, 1
  add   t0, t0, t1
  sw    t0, 116(sp)
  lw    t0, 116(sp)
  sw    t0, 32(sp)
  j     while_entry2

while_end2:
  lw    t0, 20(sp)
  sw    t0, 120(sp)
  lw    t0, 120(sp)
  li    t1, 1
  add   t0, t0, t1
  sw    t0, 124(sp)
  lw    t0, 124(sp)
  sw    t0, 20(sp)
  j     while_entry1

  .text
  .global f1
f1:
  addi  sp, sp, -240
  sw    a0, 8(sp)
  sw    a1, 12(sp)
  sw    a2, 16(sp)
  sw    a3, 20(sp)
  sw    a4, 24(sp)
  sw    a5, 28(sp)
  sw    a6, 32(sp)
  sw    a7, 36(sp)
  lw    t0, 8(sp)
  sw    t0, 40(sp)
  lw    t0, 12(sp)
  sw    t0, 44(sp)
  lw    t0, 16(sp)
  sw    t0, 48(sp)
  lw    t0, 20(sp)
  sw    t0, 52(sp)
  lw    t0, 24(sp)
  sw    t0, 56(sp)
  lw    t0, 28(sp)
  sw    t0, 60(sp)
  lw    t0, 32(sp)
  sw    t0, 64(sp)
  lw    t0, 36(sp)
  sw    t0, 68(sp)
  lw    t0, 0(sp)
  sw    t0, 72(sp)
  lw    t0, 4(sp)
  sw    t0, 76(sp)
  lw    t0, 40(sp)
  sw    t0, 80(sp)
  addi  t0, sp, 80
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 84(sp)
  lw    t0, 84(sp)
  lw    t0, 0(t0)
  sw    t0, 88(sp)
  lw    t0, 44(sp)
  sw    t0, 92(sp)
  addi  t0, sp, 92
  lw    t0, 0(t0)
  li    t1, 1
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 96(sp)
  lw    t0, 96(sp)
  lw    t0, 0(t0)
  sw    t0, 100(sp)
  lw    t0, 88(sp)
  lw    t1, 100(sp)
  add   t0, t0, t1
  sw    t0, 104(sp)
  lw    t0, 48(sp)
  sw    t0, 108(sp)
  addi  t0, sp, 108
  lw    t0, 0(t0)
  li    t1, 2
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 112(sp)
  lw    t0, 112(sp)
  lw    t0, 0(t0)
  sw    t0, 116(sp)
  lw    t0, 104(sp)
  lw    t1, 116(sp)
  add   t0, t0, t1
  sw    t0, 120(sp)
  lw    t0, 52(sp)
  sw    t0, 124(sp)
  addi  t0, sp, 124
  lw    t0, 0(t0)
  li    t1, 3
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 128(sp)
  lw    t0, 128(sp)
  lw    t0, 0(t0)
  sw    t0, 132(sp)
  lw    t0, 120(sp)
  lw    t1, 132(sp)
  add   t0, t0, t1
  sw    t0, 136(sp)
  lw    t0, 56(sp)
  sw    t0, 140(sp)
  addi  t0, sp, 140
  lw    t0, 0(t0)
  li    t1, 4
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 144(sp)
  lw    t0, 144(sp)
  lw    t0, 0(t0)
  sw    t0, 148(sp)
  lw    t0, 136(sp)
  lw    t1, 148(sp)
  add   t0, t0, t1
  sw    t0, 152(sp)
  lw    t0, 60(sp)
  sw    t0, 156(sp)
  addi  t0, sp, 156
  lw    t0, 0(t0)
  li    t1, 5
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 160(sp)
  lw    t0, 160(sp)
  lw    t0, 0(t0)
  sw    t0, 164(sp)
  lw    t0, 152(sp)
  lw    t1, 164(sp)
  add   t0, t0, t1
  sw    t0, 168(sp)
  lw    t0, 64(sp)
  sw    t0, 172(sp)
  addi  t0, sp, 172
  lw    t0, 0(t0)
  li    t1, 6
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 176(sp)
  lw    t0, 176(sp)
  lw    t0, 0(t0)
  sw    t0, 180(sp)
  lw    t0, 168(sp)
  lw    t1, 180(sp)
  add   t0, t0, t1
  sw    t0, 184(sp)
  lw    t0, 68(sp)
  sw    t0, 188(sp)
  addi  t0, sp, 188
  lw    t0, 0(t0)
  li    t1, 7
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 192(sp)
  lw    t0, 192(sp)
  lw    t0, 0(t0)
  sw    t0, 196(sp)
  lw    t0, 184(sp)
  lw    t1, 196(sp)
  add   t0, t0, t1
  sw    t0, 200(sp)
  lw    t0, 72(sp)
  sw    t0, 204(sp)
  addi  t0, sp, 204
  lw    t0, 0(t0)
  li    t1, 8
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 208(sp)
  lw    t0, 208(sp)
  lw    t0, 0(t0)
  sw    t0, 212(sp)
  lw    t0, 200(sp)
  lw    t1, 212(sp)
  add   t0, t0, t1
  sw    t0, 216(sp)
  lw    t0, 76(sp)
  sw    t0, 220(sp)
  addi  t0, sp, 220
  lw    t0, 0(t0)
  li    t1, 9
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 224(sp)
  lw    t0, 224(sp)
  lw    t0, 0(t0)
  sw    t0, 228(sp)
  lw    t0, 216(sp)
  lw    t1, 228(sp)
  add   t0, t0, t1
  sw    t0, 232(sp)
  lw    a0, 232(sp)
  addi  sp, sp, 240
  ret

  .text
  .global f2
f2:
  addi  sp, sp, -240
  sw    a0, 8(sp)
  sw    a1, 12(sp)
  sw    a2, 16(sp)
  sw    a3, 20(sp)
  sw    a4, 24(sp)
  sw    a5, 28(sp)
  sw    a6, 32(sp)
  sw    a7, 36(sp)
  lw    t0, 8(sp)
  sw    t0, 40(sp)
  lw    t0, 12(sp)
  sw    t0, 44(sp)
  lw    t0, 16(sp)
  sw    t0, 48(sp)
  lw    t0, 20(sp)
  sw    t0, 52(sp)
  lw    t0, 24(sp)
  sw    t0, 56(sp)
  lw    t0, 28(sp)
  sw    t0, 60(sp)
  lw    t0, 32(sp)
  sw    t0, 64(sp)
  lw    t0, 36(sp)
  sw    t0, 68(sp)
  lw    t0, 0(sp)
  sw    t0, 72(sp)
  lw    t0, 4(sp)
  sw    t0, 76(sp)
  lw    t0, 40(sp)
  sw    t0, 80(sp)
  addi  t0, sp, 80
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 84(sp)
  addi  t0, sp, 84
  lw    t0, 0(t0)
  li    t1, 9
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 88(sp)
  lw    t0, 88(sp)
  lw    t0, 0(t0)
  sw    t0, 92(sp)
  lw    t0, 44(sp)
  sw    t0, 96(sp)
  addi  t0, sp, 96
  lw    t0, 0(t0)
  li    t1, 1
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 100(sp)
  lw    t0, 100(sp)
  lw    t0, 0(t0)
  sw    t0, 104(sp)
  lw    t0, 92(sp)
  lw    t1, 104(sp)
  add   t0, t0, t1
  sw    t0, 108(sp)
  lw    t0, 48(sp)
  sw    t0, 112(sp)
  lw    t0, 108(sp)
  lw    t1, 112(sp)
  add   t0, t0, t1
  sw    t0, 116(sp)
  lw    t0, 52(sp)
  sw    t0, 120(sp)
  addi  t0, sp, 120
  lw    t0, 0(t0)
  li    t1, 3
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 124(sp)
  lw    t0, 124(sp)
  lw    t0, 0(t0)
  sw    t0, 128(sp)
  lw    t0, 116(sp)
  lw    t1, 128(sp)
  add   t0, t0, t1
  sw    t0, 132(sp)
  lw    t0, 56(sp)
  sw    t0, 136(sp)
  addi  t0, sp, 136
  lw    t0, 0(t0)
  li    t1, 4
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 140(sp)
  lw    t0, 140(sp)
  lw    t0, 0(t0)
  sw    t0, 144(sp)
  lw    t0, 132(sp)
  lw    t1, 144(sp)
  add   t0, t0, t1
  sw    t0, 148(sp)
  lw    t0, 60(sp)
  sw    t0, 152(sp)
  addi  t0, sp, 152
  lw    t0, 0(t0)
  li    t1, 5
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 156(sp)
  addi  t0, sp, 156
  lw    t0, 0(t0)
  li    t1, 5
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 160(sp)
  addi  t0, sp, 160
  lw    t0, 0(t0)
  li    t1, 5
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 164(sp)
  lw    t0, 164(sp)
  lw    t0, 0(t0)
  sw    t0, 168(sp)
  lw    t0, 148(sp)
  lw    t1, 168(sp)
  add   t0, t0, t1
  sw    t0, 172(sp)
  lw    t0, 64(sp)
  sw    t0, 176(sp)
  addi  t0, sp, 176
  lw    t0, 0(t0)
  li    t1, 6
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 180(sp)
  lw    t0, 180(sp)
  lw    t0, 0(t0)
  sw    t0, 184(sp)
  lw    t0, 172(sp)
  lw    t1, 184(sp)
  add   t0, t0, t1
  sw    t0, 188(sp)
  lw    t0, 68(sp)
  sw    t0, 192(sp)
  addi  t0, sp, 192
  lw    t0, 0(t0)
  li    t1, 7
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 196(sp)
  lw    t0, 196(sp)
  lw    t0, 0(t0)
  sw    t0, 200(sp)
  lw    t0, 188(sp)
  lw    t1, 200(sp)
  add   t0, t0, t1
  sw    t0, 204(sp)
  lw    t0, 72(sp)
  sw    t0, 208(sp)
  lw    t0, 204(sp)
  lw    t1, 208(sp)
  add   t0, t0, t1
  sw    t0, 212(sp)
  lw    t0, 76(sp)
  sw    t0, 216(sp)
  addi  t0, sp, 216
  lw    t0, 0(t0)
  li    t1, 9
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 220(sp)
  addi  t0, sp, 220
  lw    t0, 0(t0)
  li    t1, 8
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  sw    t0, 224(sp)
  lw    t0, 224(sp)
  lw    t0, 0(t0)
  sw    t0, 228(sp)
  lw    t0, 212(sp)
  lw    t1, 228(sp)
  add   t0, t0, t1
  sw    t0, 232(sp)
  lw    a0, 232(sp)
  addi  sp, sp, 240
  ret

  .text
  .global main
main:
  li    t0, -4320
  add   sp, sp, t0
  sw    ra, 0(sp)
  li    t0, 0
  li    t1, 4004
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 0
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4008
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4008
  add   t0, t0, sp
  lw    a0, 0(t0)
  call  init
  li    t0, 4004
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4012
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 0
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4016
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4016
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4020
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4020
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4024
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 1
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4028
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4028
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 1
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4032
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4032
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4036
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 2
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4040
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4040
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 2
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4044
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4044
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4048
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 3
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4052
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4052
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 3
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4056
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4056
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4060
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 4
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4064
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4064
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4068
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4068
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4072
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 5
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4076
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4076
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 5
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4080
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4080
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4084
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 6
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4088
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4088
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 6
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4092
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4092
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4096
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 7
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4100
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4100
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 7
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4104
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4104
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4108
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 8
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4112
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4112
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 8
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4116
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4116
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4120
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 9
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4124
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4124
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 9
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4128
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4128
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4132
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4024
  add   t0, t0, sp
  lw    a0, 0(t0)
  li    t0, 4036
  add   t0, t0, sp
  lw    a1, 0(t0)
  li    t0, 4048
  add   t0, t0, sp
  lw    a2, 0(t0)
  li    t0, 4060
  add   t0, t0, sp
  lw    a3, 0(t0)
  li    t0, 4072
  add   t0, t0, sp
  lw    a4, 0(t0)
  li    t0, 4084
  add   t0, t0, sp
  lw    a5, 0(t0)
  li    t0, 4096
  add   t0, t0, sp
  lw    a6, 0(t0)
  li    t0, 4108
  add   t0, t0, sp
  lw    a7, 0(t0)
  li    t0, 4120
  add   t0, t0, sp
  lw    t0, 0(t0)
  sw    t0, -240(sp)
  li    t0, 4132
  add   t0, t0, sp
  lw    t0, 0(t0)
  sw    t0, -236(sp)
  call  f1
  li    t0, 4136
  add   t0, t0, sp
  sw    a0, 0(t0)
  li    t1, 4012
  add   t1, t1, sp
  lw    t0, 0(t1)
  li    t2, 4136
  add   t2, t2, sp
  lw    t1, 0(t2)
  add   t0, t0, t1
  li    t2, 4140
  add   t2, t2, sp
  sw    t0, 0(t2)
  li    t0, 4140
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4004
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4004
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4144
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 0
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4148
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4148
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4152
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 1
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4156
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4156
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 1
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4160
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4160
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4164
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 2
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4168
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4168
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 2
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4172
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4172
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 2
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4176
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4176
  add   t0, t0, sp
  lw    t0, 0(t0)
  lw    t0, 0(t0)
  li    t1, 4180
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 3
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4184
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4184
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 3
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4188
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4188
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4192
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 4
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4196
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4196
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4200
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4200
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4204
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 0
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4208
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 6
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4212
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4212
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 6
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4216
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4216
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4220
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 7
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4224
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4224
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 7
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4228
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4228
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4232
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 8
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4236
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4236
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 8
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4240
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4240
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 8
  li    t2, 4
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4244
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4244
  add   t0, t0, sp
  lw    t0, 0(t0)
  lw    t0, 0(t0)
  li    t1, 4248
  add   t1, t1, sp
  sw    t0, 0(t1)
  addi  t0, sp, 4
  li    t1, 9
  li    t2, 400
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4252
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4252
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 0
  li    t2, 40
  mul   t1, t1, t2
  add   t0, t0, t1
  li    t1, 4256
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4152
  add   t0, t0, sp
  lw    a0, 0(t0)
  li    t0, 4164
  add   t0, t0, sp
  lw    a1, 0(t0)
  li    t0, 4180
  add   t0, t0, sp
  lw    a2, 0(t0)
  li    t0, 4192
  add   t0, t0, sp
  lw    a3, 0(t0)
  li    t0, 4204
  add   t0, t0, sp
  lw    a4, 0(t0)
  li    t0, 4208
  add   t0, t0, sp
  lw    a5, 0(t0)
  li    t0, 4220
  add   t0, t0, sp
  lw    a6, 0(t0)
  li    t0, 4232
  add   t0, t0, sp
  lw    a7, 0(t0)
  li    t0, 4248
  add   t0, t0, sp
  lw    t0, 0(t0)
  sw    t0, -240(sp)
  li    t0, 4256
  add   t0, t0, sp
  lw    t0, 0(t0)
  sw    t0, -236(sp)
  call  f2
  li    t0, 4260
  add   t0, t0, sp
  sw    a0, 0(t0)
  li    t1, 4144
  add   t1, t1, sp
  lw    t0, 0(t1)
  li    t2, 4260
  add   t2, t2, sp
  lw    t1, 0(t2)
  add   t0, t0, t1
  li    t2, 4264
  add   t2, t2, sp
  sw    t0, 0(t2)
  li    t0, 4264
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4004
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4004
  add   t0, t0, sp
  lw    t0, 0(t0)
  li    t1, 4268
  add   t1, t1, sp
  sw    t0, 0(t1)
  li    t0, 4268
  add   t0, t0, sp
  lw    a0, 0(t0)
  call  putint
  li    a0, 10
  call  putch
  li    a0, 0
  lw    ra, 0(sp)
  li    t0, 4320
  add   sp, sp, t0
  ret

