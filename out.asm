format ELF64
section '.text' executable
extrn putchar
extrn exit
public _start
_start:
	mov edi, 'r'
	call putchar
	mov edi, 'a'
	call putchar
	mov edi, 'h'
	call putchar
	mov edi, 'u'
	call putchar
	mov edi, 'l'
	call putchar
	mov edi, 10
	call putchar
	mov edi, [z]
	call exit
section '.data' writeable
z dd 0
