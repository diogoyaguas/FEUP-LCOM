#include "i8042.h"
.global _handler_a

.text


_handler_a:

			xor %eax, %eax
			inb $STATUS_PORT, %al
			rcr $1, %eax
			jnc OutBNF
			inb $KBD_OUT_BUF, %al
			jmp endfunc
OutBNF:
			movl $1, %eax
			jmp endfunc

endfunc:	ret
