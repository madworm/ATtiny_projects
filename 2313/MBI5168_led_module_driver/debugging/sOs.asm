
source.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	12 c0       	rjmp	.+36     	; 0x26 <__ctors_end>
   2:	2a c0       	rjmp	.+84     	; 0x58 <__bad_interrupt>
   4:	29 c0       	rjmp	.+82     	; 0x58 <__bad_interrupt>
   6:	28 c0       	rjmp	.+80     	; 0x58 <__bad_interrupt>
   8:	6b c0       	rjmp	.+214    	; 0xe0 <__vector_4>
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
  34:	ee e8       	ldi	r30, 0x8E	; 142
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
  54:	41 d0       	rcall	.+130    	; 0xd8 <main>
  56:	99 c0       	rjmp	.+306    	; 0x18a <_exit>

00000058 <__bad_interrupt>:
  58:	d3 cf       	rjmp	.-90     	; 0x0 <__vectors>

0000005a <_Z16setup_timer1_ctcv>:
  5a:	2f b7       	in	r18, 0x3f	; 63
  5c:	f8 94       	cli
  5e:	8e b5       	in	r24, 0x2e	; 46
  60:	84 60       	ori	r24, 0x04	; 4
  62:	8e bd       	out	0x2e, r24	; 46
  64:	8e b5       	in	r24, 0x2e	; 46
  66:	8c 7f       	andi	r24, 0xFC	; 252
  68:	8e bd       	out	0x2e, r24	; 46
  6a:	8f b5       	in	r24, 0x2f	; 47
  6c:	8c 7f       	andi	r24, 0xFC	; 252
  6e:	8f bd       	out	0x2f, r24	; 47
  70:	8e b5       	in	r24, 0x2e	; 46
  72:	88 60       	ori	r24, 0x08	; 8
  74:	8e bd       	out	0x2e, r24	; 46
  76:	8e b5       	in	r24, 0x2e	; 46
  78:	8f 7e       	andi	r24, 0xEF	; 239
  7a:	8e bd       	out	0x2e, r24	; 46
  7c:	8f b5       	in	r24, 0x2f	; 47
  7e:	8f 70       	andi	r24, 0x0F	; 15
  80:	8f bd       	out	0x2f, r24	; 47
  82:	80 e0       	ldi	r24, 0x00	; 0
  84:	91 e0       	ldi	r25, 0x01	; 1
  86:	9b bd       	out	0x2b, r25	; 43
  88:	8a bd       	out	0x2a, r24	; 42
  8a:	89 b7       	in	r24, 0x39	; 57
  8c:	80 64       	ori	r24, 0x40	; 64
  8e:	89 bf       	out	0x39, r24	; 57
  90:	2f bf       	out	0x3f, r18	; 63
  92:	08 95       	ret

00000094 <_Z5setupv>:
  94:	b8 9a       	sbi	0x17, 0	; 23
  96:	c0 9a       	sbi	0x18, 0	; 24
  98:	b9 9a       	sbi	0x17, 1	; 23
  9a:	c1 9a       	sbi	0x18, 1	; 24
  9c:	be 9a       	sbi	0x17, 6	; 23
  9e:	bf 9a       	sbi	0x17, 7	; 23
  a0:	bd 98       	cbi	0x17, 5	; 23
  a2:	c5 9a       	sbi	0x18, 5	; 24
  a4:	78 94       	sei
  a6:	d9 df       	rcall	.-78     	; 0x5a <_Z16setup_timer1_ctcv>
  a8:	80 e0       	ldi	r24, 0x00	; 0
  aa:	90 e0       	ldi	r25, 0x00	; 0
  ac:	2f ef       	ldi	r18, 0xFF	; 255
  ae:	fc 01       	movw	r30, r24
  b0:	e0 5a       	subi	r30, 0xA0	; 160
  b2:	ff 4f       	sbci	r31, 0xFF	; 255
  b4:	20 83       	st	Z, r18
  b6:	01 96       	adiw	r24, 0x01	; 1
  b8:	88 30       	cpi	r24, 0x08	; 8
  ba:	91 05       	cpc	r25, r1
  bc:	c1 f7       	brne	.-16     	; 0xae <_Z5setupv+0x1a>
  be:	80 e0       	ldi	r24, 0x00	; 0
  c0:	90 e0       	ldi	r25, 0x00	; 0
  c2:	20 ed       	ldi	r18, 0xD0	; 208
  c4:	37 e0       	ldi	r19, 0x07	; 7
  c6:	f9 01       	movw	r30, r18
  c8:	31 97       	sbiw	r30, 0x01	; 1
  ca:	f1 f7       	brne	.-4      	; 0xc8 <_Z5setupv+0x34>
  cc:	01 96       	adiw	r24, 0x01	; 1
  ce:	43 e1       	ldi	r20, 0x13	; 19
  d0:	88 38       	cpi	r24, 0x88	; 136
  d2:	94 07       	cpc	r25, r20
  d4:	c1 f7       	brne	.-16     	; 0xc6 <_Z5setupv+0x32>
  d6:	08 95       	ret

000000d8 <main>:
  d8:	dd df       	rcall	.-70     	; 0x94 <_Z5setupv>
  da:	f8 94       	cli
  dc:	c1 9a       	sbi	0x18, 1	; 24
  de:	ff cf       	rjmp	.-2      	; 0xde <main+0x6>

000000e0 <__vector_4>:
  e0:	1f 92       	push	r1
  e2:	0f 92       	push	r0
  e4:	0f b6       	in	r0, 0x3f	; 63
  e6:	0f 92       	push	r0
  e8:	11 24       	eor	r1, r1
  ea:	2f 93       	push	r18
  ec:	3f 93       	push	r19
  ee:	4f 93       	push	r20
  f0:	5f 93       	push	r21
  f2:	6f 93       	push	r22
  f4:	7f 93       	push	r23
  f6:	8f 93       	push	r24
  f8:	9f 93       	push	r25
  fa:	af 93       	push	r26
  fc:	bf 93       	push	r27
  fe:	ef 93       	push	r30
 100:	ff 93       	push	r31
 102:	c1 98       	cbi	0x18, 1	; 24
 104:	50 e0       	ldi	r21, 0x00	; 0
 106:	61 e0       	ldi	r22, 0x01	; 1
 108:	70 e0       	ldi	r23, 0x00	; 0
 10a:	b0 e4       	ldi	r27, 0x40	; 64
 10c:	ab e1       	ldi	r26, 0x1B	; 27
 10e:	40 e0       	ldi	r20, 0x00	; 0
 110:	20 e0       	ldi	r18, 0x00	; 0
 112:	30 e0       	ldi	r19, 0x00	; 0
 114:	f9 01       	movw	r30, r18
 116:	e0 5a       	subi	r30, 0xA0	; 160
 118:	ff 4f       	sbci	r31, 0xFF	; 255
 11a:	80 81       	ld	r24, Z
 11c:	58 17       	cp	r21, r24
 11e:	48 f4       	brcc	.+18     	; 0x132 <__vector_4+0x52>
 120:	cb 01       	movw	r24, r22
 122:	02 2e       	mov	r0, r18
 124:	02 c0       	rjmp	.+4      	; 0x12a <__vector_4+0x4a>
 126:	88 0f       	add	r24, r24
 128:	99 1f       	adc	r25, r25
 12a:	0a 94       	dec	r0
 12c:	e2 f7       	brpl	.-8      	; 0x126 <__vector_4+0x46>
 12e:	48 2b       	or	r20, r24
 130:	09 c0       	rjmp	.+18     	; 0x144 <__vector_4+0x64>
 132:	cb 01       	movw	r24, r22
 134:	02 2e       	mov	r0, r18
 136:	02 c0       	rjmp	.+4      	; 0x13c <__vector_4+0x5c>
 138:	88 0f       	add	r24, r24
 13a:	99 1f       	adc	r25, r25
 13c:	0a 94       	dec	r0
 13e:	e2 f7       	brpl	.-8      	; 0x138 <__vector_4+0x58>
 140:	80 95       	com	r24
 142:	48 23       	and	r20, r24
 144:	2f 5f       	subi	r18, 0xFF	; 255
 146:	3f 4f       	sbci	r19, 0xFF	; 255
 148:	28 30       	cpi	r18, 0x08	; 8
 14a:	31 05       	cpc	r19, r1
 14c:	19 f7       	brne	.-58     	; 0x114 <__vector_4+0x34>
 14e:	c4 98       	cbi	0x18, 4	; 24
 150:	4f b9       	out	0x0f, r20	; 15
 152:	be b9       	out	0x0e, r27	; 14
 154:	01 c0       	rjmp	.+2      	; 0x158 <__vector_4+0x78>
 156:	ad b9       	out	0x0d, r26	; 13
 158:	76 9b       	sbis	0x0e, 6	; 14
 15a:	fd cf       	rjmp	.-6      	; 0x156 <__vector_4+0x76>
 15c:	8f b1       	in	r24, 0x0f	; 15
 15e:	c4 9a       	sbi	0x18, 4	; 24
 160:	5f 5f       	subi	r21, 0xFF	; 255
 162:	50 38       	cpi	r21, 0x80	; 128
 164:	a1 f6       	brne	.-88     	; 0x10e <__vector_4+0x2e>
 166:	c1 9a       	sbi	0x18, 1	; 24
 168:	ff 91       	pop	r31
 16a:	ef 91       	pop	r30
 16c:	bf 91       	pop	r27
 16e:	af 91       	pop	r26
 170:	9f 91       	pop	r25
 172:	8f 91       	pop	r24
 174:	7f 91       	pop	r23
 176:	6f 91       	pop	r22
 178:	5f 91       	pop	r21
 17a:	4f 91       	pop	r20
 17c:	3f 91       	pop	r19
 17e:	2f 91       	pop	r18
 180:	0f 90       	pop	r0
 182:	0f be       	out	0x3f, r0	; 63
 184:	0f 90       	pop	r0
 186:	1f 90       	pop	r1
 188:	18 95       	reti

0000018a <_exit>:
 18a:	f8 94       	cli

0000018c <__stop_program>:
 18c:	ff cf       	rjmp	.-2      	; 0x18c <__stop_program>
