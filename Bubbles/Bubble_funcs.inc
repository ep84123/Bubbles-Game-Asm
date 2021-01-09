include Dmoot_funcs.inc
.code

;calculate the decimal energy of a bubble and adds it to energy variable
calEnergy proc adrObj:dword
	mov ebx,adrObj
	mov eax,[ebx + DM.drcdec.x]
	imul eax
	div res.deci
	mov ecx,eax
	mov eax,[ebx + DM.drcdec.y]
	imul eax
	div res.deci
	add eax,ecx
	add energy,eax
ret 
calEnergy endp

;reset bubble transp, if transp = 1 meaning the bubble didn't collide, it has space to become solid
;if transp>1 (it collided) then transp = 1. if transp = 0, it stays 0 
resetbubtransp proc adrbub:dword
	mov ecx,adrbub
	mov ebx,1
	cmp [ecx + DM.transp],ebx
	jg MOV1
	mov ebx,0
	MOV1:
	mov [ecx + DM.transp],ebx
ret
resetbubtransp endp

;check the conditions for a collision between two bubbles
;result: 0 =>no collision, 1 => collision
checkconditions proc adr:dword,adr2:dword
	;basic collision check according to position
	invoke collcheck, adr,adr2 
	cmp result,0
	je RETMAIN
	mov result, 0

	;if one of the bubbles is transparent => no collusion
	invoke transphandler, adr
	invoke transphandler, adr2
	cmp result,0
	jg RETMAIN

	;if bubbles weren't last to collide with each other
	;prevents bubble from colliding twice in a row 
	mov ebx,adr
	mov ecx,adr2
	cmp ebx,[ecx+ DM.lastcoladr]
	jne FINECOND
	cmp ecx,[ebx+ DM.lastcoladr]
	je RETMAIN
	FINECOND:
	mov [ebx + DM.lastcoladr], ecx
	mov [ecx + DM.lastcoladr], ebx

	;if two bubbles overlap, one become transparent and ==> no collision
	invoke collisioncheck, adr,adr2,res.defensedist
	cmp result,0
	je NOTSPECIALCASE
	mov ecx,adr
	mov [ecx + DM.transp],2
	jmp RETMAIN
	NOTSPECIALCASE:
	mov result,0
ret
	RETMAIN:
	mov result,1
	ret
checkconditions endp

;calculates the cos and sin of an the collusion angle and puts it in cos and sin variables
calangle proc adr:dword,adr2:dword
	;calculate the squared distance between the bubbles into dist
	mov ebx,[eax+DM.pos.x]
	mov ecx,[edx+DM.pos.x]
	sub ebx,[edx+DM.pos.x]
	imul ebx,ebx
	mov ecx,[eax+DM.pos.y]
	sub ecx,[edx+DM.pos.y]
	imul ecx,ecx
	add ebx,ecx
	mov dist,ebx

	;takes a squre root of dist using the fpu - converts it todecimal display
	fild dist
	fsqrt
	fild res.deci
	fmulp st(1), st(0)
	fistp dist

	;cos = dx/dist (decimal)
	mov ecx,adr
	mov eax, [ecx + DM.pos.x]
	mov ecx,adr2
	sub eax, [ecx + DM.pos.x]
	mul res.deci
	invoke division, eax,dist,offset cos

	;sin = dx/dist (decimal)
	mov ecx,adr
	mov eax,[ecx + DM.pos.y]
	mov ecx,adr2
	sub eax,[ecx + DM.pos.y]
	mul res.deci
	invoke division, eax,dist,offset sin
ret
calangle endp

;switches the velocity vectors of the bubbles that are vertical to the collision 
swapVelVector proc, adr:dword,adr2:dword
	;rotates the direction's vectors
	invoke rotate, adr
	invoke rotate, adr2

	;switches the x values
	mov eax,adr
	mov edx,adr2
	mov ebx,[eax + DM.drcdec.x]
	mov ecx,[edx + DM.drcdec.x]
	mov [edx + DM.drcdec.x],ebx
	mov [eax + DM.drcdec.x],ecx
	neg sin

	;rotates back
	invoke rotate, adr
	invoke rotate, adr2
ret
swapVelVector endp

;handle a collision event between two given bubbles
;checks condition - if there's a collision: calculate angle and switch the vectors 
collisionHandler proc adr:dword,adr2:dword
	invoke checkconditions, adr,adr2
	cmp result,1
	je GOTOMAIN
	invoke calangle, adr,adr2
	invoke swapVelVector,adr,adr2
	GOTOMAIN:
ret
collisionHandler endp
