.globl ss_sendmsg
	.ent ss_sendmsg
ss_sendmsg:
	li $2,258
        syscall
        nop
        jr $31
        nop
	.end ss_sendmsg

.globl ss_recvmsg
	.ent ss_recvmsg
ss_recvmsg:
	li $2,259
        syscall
        nop
        jr $31
        nop
	.end ss_recvmsg

.globl ss_getmyactualid
	.ent ss_getmyactualid
ss_getmyactualid:
	li $2,260
        syscall
        nop
        jr $31
        nop
	.end ss_getmyactualid

.globl ss_getmythreadid
	.ent ss_getmythreadid
ss_getmythreadid:
	li $2,261
        syscall
        nop
        jr $31
        nop
	.end ss_getmythreadid

.globl ss_msgstatus
	.ent ss_msgstatus
ss_msgstatus:
	li $2,262
        syscall
        nop
        jr $31
        nop
	.end ss_msgstatus

.globl ss_sendmsg_f
	.ent ss_sendmsg_f
ss_sendmsg_f:
	li $2,263
        syscall
        nop
        jr $31
        nop
	.end ss_sendmsg_f

.globl ss_recvmsg_f
	.ent ss_recvmsg_f
ss_recvmsg_f:
	li $2,264
        syscall
        nop
        jr $31
        nop
	.end ss_recvmsg_f

.globl ss_msgstatus_f
	.ent ss_msgstatus_f
ss_msgstatus_f:
	li $2,265
        syscall
        nop
        jr $31
        nop
	.end ss_msgstatus_f

