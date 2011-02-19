
source.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	12 c0       	rjmp	.+36     	; 0x26 <__ctors_end>
   2:	2a c0       	rjmp	.+84     	; 0x58 <__bad_interrupt>
   4:	29 c0       	rjmp	.+82     	; 0x58 <__bad_interrupt>
   6:	28 c0       	rjmp	.+80     	; 0x58 <__bad_interrupt>
   8:	28 c0       	rjmp	.+80     	; 0x5a <__vector_4>
   a:	26 c0       	rjmp	.+76     	; 0x58 <__bad_interrupt>
   c:	25 c0       	rjmp	.+74     	; 0x58 <__bad_interrupt>
   e:	24 c0       	rjmp	.+72     	; 0x58 <__bad_interrupt>
  10:	23 c0       	rjmp	.+70     	; 0x58 <__bad_interrupt>
  12:	22 c0       	rjmp	.+68     	; 0x58 <__bad_interrupt>
  14:	21 c0       	rjmp	.+66     	; 0x58 <__bad_interrupt>
  16:	20 c0       	rjmp	.+64     	; 0x58 <__bad_interrupt>
  18:	1f c0       	rjmp	.+62     	; 0x58 <__bad_interrupt>
  1a:	1e c0       	rjmp	.+60     	; 0x58 <__bad_interrupt>
  1c:	1d c0       	rjmp	.+58     	; 0x58 <__bad_interrupt>
  1e:	1c c0       	rjmp	.+56     	; 0x58 <__bad_interrupt>
  20:	1b c0       	rjmp	.+54     	; 0x58 <__bad_interrupt>
  22:	1a c0       	rjmp	.+52     	; 0x58 <__bad_interrupt>
  24:	19 c0       	rjmp	.+50     	; 0x58 <__bad_interrupt>

00000026 <__ctors_end>:
  26:	11 24       	eor	r1, r1
  28:	1f be       	out	0x3f, r1	; 63
  2a:	cf ed       	ldi	r28, 0xDF	; 223
  2c:	cd bf       	out	0x3d, r28	; 61

0000002e <__do_copy_data>:
  2e:	10 e0       	ldi	r17, 0x00	; 0
  30:	a0 e6       	ldi	r26, 0x60	; 96
  32:	b0 e0       	ldi	r27, 0x00	; 0
  34:	ee e9       	ldi	r30, 0x9E	; 158
  36:	f1 e0       	ldi	r31, 0x01	; 1
  38:	02 c0       	rjmp	.+4      	; 0x3e <.do_copy_data_start>

0000003a <.do_copy_data_loop>:
  3a:	05 90       	lpm	r0, Z+
  3c:	0d 92       	st	X+, r0

0000003e <.do_copy_data_start>:
  3e:	a0 36       	cpi	r26, 0x60	; 96
  40:	b1 07       	cpc	r27, r17
  42:	d9 f7       	brne	.-10     	; 0x3a <.do_copy_data_loop>

00000044 <__do_clear_bss>:
  44:	10 e0       	ldi	r17, 0x00	; 0
  46:	a0 e6       	ldi	r26, 0x60	; 96
  48:	b0 e0       	ldi	r27, 0x00	; 0
  4a:	01 c0       	rjmp	.+2      	; 0x4e <.do_clear_bss_start>

0000004c <.do_clear_bss_loop>:
  4c:	1d 92       	st	X+, r1

0000004e <.do_clear_bss_start>:
  4e:	a8 36       	cpi	r26, 0x68	; 104
  50:	b1 07       	cpc	r27, r17
  52:	e1 f7       	brne	.-8      	; 0x4c <.do_clear_bss_loop>
  54:	62 d0       	rcall	.+196    	; 0x11a <main>
  56:	a1 c0       	rjmp	.+322    	; 0x19a <_exit>

00000058 <__bad_interrupt>:
  58:	d3 cf       	rjmp	.-90     	; 0x0 <__vectors>

0000005a <__vector_4>:
  5a:	1f 92       	push	r1
  5c:	0f 92       	push	r0
  5e:	0f b6       	in	r0, 0x3f	; 63
  60:	0f 92       	push	r0
  62:	11 24       	eor	r1, r1
  64:	2f 93       	push	r18
  66:	3f 93       	push	r19
  68:	4f 93       	push	r20
  6a:	8f 93       	push	r24
  6c:	9f 93       	push	r25
  6e:	c1 98       	cbi	0x18, 1	; 24
  70:	20 e0       	ldi	r18, 0x00	; 0
  72:	40 e4       	ldi	r20, 0x40	; 64
  74:	3b e1       	ldi	r19, 0x1B	; 27
  76:	80 91 60 00 	lds	r24, 0x0060
  7a:	90 e0       	ldi	r25, 0x00	; 0
  7c:	28 17       	cp	r18, r24
  7e:	08 f4       	brcc	.+2      	; 0x82 <__vector_4+0x28>
  80:	91 e0       	ldi	r25, 0x01	; 1
  82:	80 91 61 00 	lds	r24, 0x0061
  86:	28 17       	cp	r18, r24
  88:	08 f4       	brcc	.+2      	; 0x8c <__vector_4+0x32>
  8a:	45 c0       	rjmp	.+138    	; 0x116 <__stack+0x37>
  8c:	9d 7f       	andi	r25, 0xFD	; 253
  8e:	80 91 62 00 	lds	r24, 0x0062
  92:	28 17       	cp	r18, r24
  94:	f0 f1       	brcs	.+124    	; 0x112 <__stack+0x33>
  96:	9b 7f       	andi	r25, 0xFB	; 251
  98:	80 91 63 00 	lds	r24, 0x0063
  9c:	28 17       	cp	r18, r24
  9e:	b8 f1       	brcs	.+110    	; 0x10e <__stack+0x2f>
  a0:	97 7f       	andi	r25, 0xF7	; 247
  a2:	80 91 64 00 	lds	r24, 0x0064
  a6:	28 17       	cp	r18, r24
  a8:	80 f1       	brcs	.+96     	; 0x10a <__stack+0x2b>
  aa:	9f 7e       	andi	r25, 0xEF	; 239
  ac:	80 91 65 00 	lds	r24, 0x0065
  b0:	28 17       	cp	r18, r24
  b2:	48 f1       	brcs	.+82     	; 0x106 <__stack+0x27>
  b4:	9f 7d       	andi	r25, 0xDF	; 223
  b6:	80 91 66 00 	lds	r24, 0x0066
  ba:	28 17       	cp	r18, r24
  bc:	10 f1       	brcs	.+68     	; 0x102 <__stack+0x23>
  be:	9f 7b       	andi	r25, 0xBF	; 191
  c0:	80 91 67 00 	lds	r24, 0x0067
  c4:	28 17       	cp	r18, r24
  c6:	d0 f4       	brcc	.+52     	; 0xfc <__stack+0x1d>
  c8:	89 2f       	mov	r24, r25
  ca:	80 68       	ori	r24, 0x80	; 128
  cc:	c4 98       	cbi	0x18, 4	; 24
  ce:	8f b9       	out	0x0f, r24	; 15
  d0:	4e b9       	out	0x0e, r20	; 14
  d2:	76 99       	sbic	0x0e, 6	; 14
  d4:	03 c0       	rjmp	.+6      	; 0xdc <__vector_4+0x82>
  d6:	3d b9       	out	0x0d, r19	; 13
  d8:	76 9b       	sbis	0x0e, 6	; 14
  da:	fd cf       	rjmp	.-6      	; 0xd6 <__vector_4+0x7c>
  dc:	8f b1       	in	r24, 0x0f	; 15
  de:	c4 9a       	sbi	0x18, 4	; 24
  e0:	2f 5f       	subi	r18, 0xFF	; 255
  e2:	20 38       	cpi	r18, 0x80	; 128
  e4:	41 f6       	brne	.-112    	; 0x76 <__vector_4+0x1c>
  e6:	c1 9a       	sbi	0x18, 1	; 24
  e8:	9f 91       	pop	r25
  ea:	8f 91       	pop	r24
  ec:	4f 91       	pop	r20
  ee:	3f 91       	pop	r19
  f0:	2f 91       	pop	r18
  f2:	0f 90       	pop	r0
  f4:	0f be       	out	0x3f, r0	; 63
  f6:	0f 90       	pop	r0
  f8:	1f 90       	pop	r1
  fa:	18 95       	reti
  fc:	89 2f       	mov	r24, r25
  fe:	8f 77       	andi	r24, 0x7F	; 127
 100:	e5 cf       	rjmp	.-54     	; 0xcc <__vector_4+0x72>
 102:	90 64       	ori	r25, 0x40	; 64
 104:	dd cf       	rjmp	.-70     	; 0xc0 <__vector_4+0x66>
 106:	90 62       	ori	r25, 0x20	; 32
 108:	d6 cf       	rjmp	.-84     	; 0xb6 <__vector_4+0x5c>
 10a:	90 61       	ori	r25, 0x10	; 16
 10c:	cf cf       	rjmp	.-98     	; 0xac <__vector_4+0x52>
 10e:	98 60       	ori	r25, 0x08	; 8
 110:	c8 cf       	rjmp	.-112    	; 0xa2 <__vector_4+0x48>
 112:	94 60       	ori	r25, 0x04	; 4
 114:	c1 cf       	rjmp	.-126    	; 0x98 <__vector_4+0x3e>
 116:	92 60       	ori	r25, 0x02	; 2
 118:	ba cf       	rjmp	.-140    	; 0x8e <__vector_4+0x34>

0000011a <main>:
 11a:	b8 9a       	sbi	0x17, 0	; 23
 11c:	c0 9a       	sbi	0x18, 0	; 24
 11e:	b9 9a       	sbi	0x17, 1	; 23
 120:	c1 9a       	sbi	0x18, 1	; 24
 122:	be 9a       	sbi	0x17, 6	; 23
 124:	bf 9a       	sbi	0x17, 7	; 23
 126:	bd 98       	cbi	0x17, 5	; 23
 128:	c5 9a       	sbi	0x18, 5	; 24
 12a:	78 94       	sei
 12c:	2f b7       	in	r18, 0x3f	; 63
 12e:	f8 94       	cli
 130:	8e b5       	in	r24, 0x2e	; 46
 132:	84 60       	ori	r24, 0x04	; 4
 134:	8e bd       	out	0x2e, r24	; 46
 136:	8e b5       	in	r24, 0x2e	; 46
 138:	8c 7f       	andi	r24, 0xFC	; 252
 13a:	8e bd       	out	0x2e, r24	; 46
 13c:	8f b5       	in	r24, 0x2f	; 47
 13e:	8c 7f       	andi	r24, 0xFC	; 252
 140:	8f bd       	out	0x2f, r24	; 47
 142:	8e b5       	in	r24, 0x2e	; 46
 144:	88 60       	ori	r24, 0x08	; 8
 146:	8e bd       	out	0x2e, r24	; 46
 148:	8e b5       	in	r24, 0x2e	; 46
 14a:	8f 7e       	andi	r24, 0xEF	; 239
 14c:	8e bd       	out	0x2e, r24	; 46
 14e:	8f b5       	in	r24, 0x2f	; 47
 150:	8f 70       	andi	r24, 0x0F	; 15
 152:	8f bd       	out	0x2f, r24	; 47
 154:	80 e0       	ldi	r24, 0x00	; 0
 156:	91 e0       	ldi	r25, 0x01	; 1
 158:	9b bd       	out	0x2b, r25	; 43
 15a:	8a bd       	out	0x2a, r24	; 42
 15c:	89 b7       	in	r24, 0x39	; 57
 15e:	80 64       	ori	r24, 0x40	; 64
 160:	89 bf       	out	0x39, r24	; 57
 162:	2f bf       	out	0x3f, r18	; 63
 164:	80 e0       	ldi	r24, 0x00	; 0
 166:	90 e0       	ldi	r25, 0x00	; 0
 168:	2f ef       	ldi	r18, 0xFF	; 255
 16a:	fc 01       	movw	r30, r24
 16c:	e0 5a       	subi	r30, 0xA0	; 160
 16e:	ff 4f       	sbci	r31, 0xFF	; 255
 170:	20 83       	st	Z, r18
 172:	01 96       	adiw	r24, 0x01	; 1
 174:	88 30       	cpi	r24, 0x08	; 8
 176:	91 05       	cpc	r25, r1
 178:	c1 f7       	brne	.-16     	; 0x16a <main+0x50>
 17a:	20 e0       	ldi	r18, 0x00	; 0
 17c:	30 e0       	ldi	r19, 0x00	; 0
 17e:	40 ed       	ldi	r20, 0xD0	; 208
 180:	57 e0       	ldi	r21, 0x07	; 7
 182:	ca 01       	movw	r24, r20
 184:	01 97       	sbiw	r24, 0x01	; 1
 186:	f1 f7       	brne	.-4      	; 0x184 <main+0x6a>
 188:	2f 5f       	subi	r18, 0xFF	; 255
 18a:	3f 4f       	sbci	r19, 0xFF	; 255
 18c:	83 e1       	ldi	r24, 0x13	; 19
 18e:	28 38       	cpi	r18, 0x88	; 136
 190:	38 07       	cpc	r19, r24
 192:	b9 f7       	brne	.-18     	; 0x182 <main+0x68>
 194:	f8 94       	cli
 196:	c1 9a       	sbi	0x18, 1	; 24
 198:	ff cf       	rjmp	.-2      	; 0x198 <main+0x7e>

0000019a <_exit>:
 19a:	f8 94       	cli

0000019c <__stop_program>:
 19c:	ff cf       	rjmp	.-2      	; 0x19c <__stop_program>
