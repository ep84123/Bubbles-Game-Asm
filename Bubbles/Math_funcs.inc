.code

; definition of "decimalic number":
; integer which represents a decimal fraction, and is in the ten-thousands.
; the decimalic number for a decimal fraction P is given by: (res.deci)*P. (res.deci is a constant)
; As example: 
; res.deci = 10000, P=1.2567 ==> decimalic number = 12567
; In all following function comments, assume that res.deci = 10000.


; function receives a number, divisor, and performs a special kind of division. 
; result = (number*(res.deci)) / divisor.
; num and divisor must be the same "kind"  of numbers. 
; a. regular numbers
; b. decimalic numbers.
; the result is put in the address specified by adrret. 


division proc num:dword, divisor:dword, adrret:dword
	mov eax,num
	mov ebx,res.deci
	imul ebx
	idiv divisor
	mov ebx,adrret
	mov [ebx],eax
ret
division endp

; receives a decimalic number (decnum) and rounds it.
; the result is put in the address specified by adrret. 

round proc decnum:dword,adrret:dword
	mov eax, res.deci
	cdq
	mov ebx,2
	div ebx
	add eax,decnum ;until here eax = decnum + res.deci/2 = decnum + 500 
	cdq
	idiv res.deci ;divied eax by res.deci, now eax is the rounded number  
	mov ebx,adrret
	mov [ebx],eax
	;put eax in the given address
ret
round endp

; multiplies the number in address "adr", by the decimalic number "muli" and divides it by res.deci.
; the result is put in "adr"
; example: "adr" contains value 220, muli=9000, result= (220*9000)/res.deci = 198 = 90% of 220
muladr proc adr:dword,muli:dword
	mov ecx,adr
	mov eax,[ecx]
	mov ebx,muli
	imul ebx
	idiv res.deci
	mov [ecx],eax
ret
muladr endp

;copy a given section in the memory to another place in the memory
;receives the address where the section starts and it's length and copies each byte to it's place at the target address
CopyMem proc adrstart:dword,Bytesnum:dword,adrtarget:dword
	mov ebx,adrstart
	mov ecx,adrtarget
	add Bytesnum,ebx
	LOOPcop:
	mov al,[ebx]
	mov [ecx],al
	add ebx,SIZEOF byte
	add ecx,SIZEOF byte
	cmp ebx,Bytesnum
	jl LOOPcop
ret
CopyMem endp

	