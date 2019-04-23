	.file	1 "fftmpi.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -w -o

gcc2_compiled.:
__gnu_compiled_c:
	.rdata
	.align	2
$LC0:
	.ascii	"node_id is %d\n\000"
	.sdata
	.align	2
$LC1:
	.ascii	"fft_id2\000"
	.text
	.align	2
	.globl	main

	.text

	.loc	1 8
	.ent	main
main:
	.frame	$fp,1088,$31		# vars= 1056, regs= 2/0, args= 24, extra= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,1088
	sw	$31,1084($sp)
	sw	$fp,1080($sp)
	move	$fp,$sp
	jal	__main
	jal	ss_getmyactualid
	sw	$2,24($fp)
	li	$2,0x00000005		# 5
	sw	$2,32($fp)
	li	$4,0x00000400		# 1024
	jal	malloc
	sw	$2,1064($fp)
	li	$4,0x00000030		# 48
	jal	malloc
	sw	$2,1068($fp)
	li	$4,0x00000030		# 48
	jal	malloc
	sw	$2,1072($fp)
	la	$4,$LC0
	lw	$5,24($fp)
	jal	printf
	sw	$0,28($fp)
$L2:
	lw	$2,28($fp)
	slt	$3,$2,128
	bne	$3,$0,$L5
	j	$L3
$L5:
	lw	$2,28($fp)
	move	$3,$2
	sll	$2,$3,3
	addu	$3,$fp,40
	addu	$2,$3,$2
	lw	$3,32($fp)
	sw	$3,0($2)
	lw	$2,28($fp)
	move	$3,$2
	sll	$2,$3,3
	addu	$3,$fp,40
	addu	$2,$3,$2
	lw	$3,32($fp)
	addu	$4,$3,2
	sw	$4,4($2)
	lw	$2,32($fp)
	addu	$3,$2,2
	sw	$3,32($fp)
$L4:
	lw	$3,28($fp)
	addu	$2,$3,1
	move	$3,$2
	sw	$3,28($fp)
	j	$L2
$L3:
	addu	$2,$fp,40
	lw	$3,1068($fp)
	sw	$3,16($sp)
	li	$4,0x00000002		# 2
	move	$5,$2
	li	$6,0x00000400		# 1024
	la	$7,$LC1
	jal	ss_sendmsg_f
$L6:
	lw	$4,24($fp)
	li	$5,0x00000002		# 2
	la	$6,$LC1
	jal	ss_msgstatus_f
	li	$3,0x00000008		# 8
	bne	$2,$3,$L8
	j	$L7
$L8:
	j	$L6
$L7:
	li	$4,0x00000002		# 2
	la	$5,$LC1
	lw	$6,1064($fp)
	lw	$7,1072($fp)
	jal	ss_recvmsg_f
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,1084($sp)
	lw	$fp,1080($sp)
	addu	$sp,$sp,1088
	j	$31
	.end	main
