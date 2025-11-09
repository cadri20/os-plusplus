[bits 32]

global read_port
global write_port
global fake_outsw

read_port:
	mov edx, [esp + 4]
	in al, dx
	ret

write_port:
	mov edx, [esp + 4]
	mov al, [esp + 4 + 4]
	out dx, al
	ret

fake_outsw:
	push ebp
	mov ebp, esp
	pushad
	mov dx, [ebp + 8]
	mov ecx, [ebp + 12]
	mov esi, [ebp + 16]

	dec ecx

fake_outsw_loop:
	outsw

	jmp $+2
	nop

	dec ecx
	jns fake_outsw_loop

	popad
	leave 
	ret



