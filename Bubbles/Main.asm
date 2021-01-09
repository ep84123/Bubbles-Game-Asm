include \masm32\include\masm32rt.inc
include main_code.inc
.code


main proc
	;creates a local variable which counts the number of runs
	push ebp
	mov ebp,esp
	sub esp,4
	mov DWORD PTR [ebp - 4],0
	invoke load
	loopi:
		inc DWORD PTR [ebp - 4]
		invoke update,[ebp - 4]
		invoke draw,[ebp - 4]
	jmp loopi
	mov esp,ebp
	pop ebp
ret
main endp
end main
