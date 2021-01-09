include Bubble_funcs.inc
.code

;adds a bubble if needed
;checks if there are players alive, if true invokes allDmootAddBubble
;if not checks if it's time to add a bubble
bubbleAdder proc counter:dword
	cmp AlivePlayersNum,0
	jng AUTOADDER
	invoke allDmootAddBubble,offset offplayers,LENGTHOF offplayers
	jmp FINISHED
	AUTOADDER:
	mov ebx,res.addbubblecount
	mov eax,counter
	cdq
	div ebx
	cmp edx,0
	jne FINISHED
	call addBubble
	FINISHED:
ret
bubbleAdder endp

;receives a bubble as an input and changes it's anim,calculate the energy,reset it's transp,
;multiplies its direction by the energyconst,and draw it
drawInnerLoop proc adr:dword,counter:dword,energyconst:dword
	invoke handleAnim,adr,counter
	invoke calEnergy,adr
	invoke resetbubtransp,adr

	invoke muldrc,adr,energyconst
	
	invoke drawImg,adr 
ret
drawInnerLoop endp

updateInnerLoop proc adr:dword,index:dword
	invoke handlewalls, adr
	invoke allDmootCollCheck,offset offplayers,LENGTHOF offplayers,adr
	
	inc index
	mov ebx,index
	cmp ebx,n
	je AFTER
	LOOP2:
	imul esi, index, TYPE bubbles
	add esi, offset bubbles ;esi is the address of the bubble j
	invoke collisionHandler,adr,esi
	inc index
	mov ebx, index
	cmp ebx,n
	jl LOOP2
	AFTER:
	invoke updateloc, adr
ret
updateInnerLoop endp

;invokes drawInnerLoop for each bubble 
drawLoop proc energyconst:dword,counter:dword
	mov energy,0
	mov i,0 
	loop3:
		imul edi, i, TYPE bubbles
		add edi, offset bubbles ; edi is the address of the bubble i
		invoke drawInnerLoop,edi,counter,energyconst
		inc i
		mov ebx,i
		cmp ebx,n
		jl loop3
ret
drawLoop endp
