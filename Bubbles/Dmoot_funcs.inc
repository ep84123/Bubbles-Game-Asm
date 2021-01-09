include Func.inc
.code

;receives player and a key, if the key is pressed, adds the value to the given place in the direction
;add 1 to the result if key is pressed
actByKey proc key:dword, adrObj:dword, drc:dword,value:dword
	;checks if key is pressed
	mov ebx,adrObj
	add ebx,key
	invoke GetAsyncKeyState,[ebx]
	mov dl,1
	cmp eax,0
	je TOHERE
	mov dl,0
	;adds value to direction
	mov ecx,value
	mov ebx,adrObj
	add ebx,drc
	add [ebx], ecx
	TOHERE:
	add result,dl
ret
actByKey endp

;update a player's direction according to the keys' state and to the object's speed 
;if two keys are pressed, decrease the direction so the total velocity would alway be the same
keyboard proc adrObj:dword,factorspd:dword
	;drc,result =0
	mov ebx,adrObj
	mov [ebx+DM.drcdec.x],0
	mov [ebx+DM.drcdec.y],0
	mov result,0

	;checks the state of each key and updates drc using actbykey proc
	invoke actByKey, DM.keys.right, adrObj, DM.drcdec.x,res.deci
	invoke actByKey, DM.keys.down, adrObj, DM.drcdec.y,res.deci
	mov ebx,res.deci
	neg ebx
	invoke actByKey, DM.keys.up, adrObj, DM.drcdec.y,ebx
	mov ebx,res.deci
	neg ebx
	invoke actByKey, DM.keys.left, adrObj, DM.drcdec.x,ebx
	;if two keys are pressed mul drc by 1/sqrt(2)
	cmp result,2
	jne FINALE
	invoke muldrc,adrObj,7071
	FINALE:
	;mul by object's velocity
	invoke muldrc,adrObj,factorspd
	ret
keyboard endp

;updates the direction corresponding to the key's states and the location using updateloc proc
;checks if collided with the walls. if collided it saves the updated direction
;and recalculate the direction according to the walls state. then sub the saved direction from it and update the location again  
dmootmove proc dmadr:dword
	mov ebx,dmadr
	cmp [ebx + DM.animindex],-1
	je ATHOME
	cmp [ebx + DM.animindex],-2
	je INBUBBLE
	cmp [ebx + DM.freeze],0
	jg FREEZED
	mov [ebx + DM.lastcoladr],ebx
	mov ecx,[ebx+DM.data]
	invoke keyboard, dmadr,(DmootData PTR [ecx]).spdec
	invoke updateloc, dmadr
	mov ebx,dmadr
	mov ecx,[ebx+DM.data]
	invoke checkwalls, dmadr,(ObjData PTR [ecx]).imgw,(ObjData PTR [ecx]).imgh
	cmp result,0
	je FREEDMOOT
	;saves the direction
	mov ecx,dmadr
	mov ebx,[ecx + DM.drcdec.x]
	mov cos,ebx
	mov ebx,[ecx + DM.drcdec.y]
	mov sin,ebx
	;updates the direction
	invoke handlewalls,dmadr
	;sub the saved direction from the updated and recalculate location
	mov ebx,cos
	mov ecx,dmadr
	sub [ecx + DM.drcdec.x],ebx
	mov ebx,sin
	sub [ecx + DM.drcdec.y],ebx
	invoke updateloc, dmadr
	FREEDMOOT:

	mov ebx,dmadr
	mov ecx,[ebx+DM.data]
	invoke checkwalls,dmadr,(DmootData PTR [ecx]).homx,(DmootData PTR [ecx]).homy
	mov eax,0
	mov al,result
	mov ebx, 3
	cdq
	div ebx
	mov ecx,dmadr
	mov [ecx+DM.transp],eax
	add [ecx+DM.hometime],eax
	mov ebx,[ecx+DM.hometime]
	mul ebx
	mov [ecx+DM.hometime],eax
	cmp eax,res.hometimelimit
	jl ATHOME
	mov [ecx + DM.transp],0;makes the Dmoot vulnerable
	;sub [ecx + DM.score],1;takes a point out of the Dmoot score
	;mov [ecx + DM.animindex],-1;removes the DMoot from the game
	ATHOME:
ret
	FREEZED:
	mov ebx,dmadr
	mov ecx,1
	add [ebx + DM.freeze],ecx
	mov edx,[ebx + DM.freeze]
	cmp res.freezeTime,edx
	jg ATHOME
	mov [ebx + DM.freeze],0
	ret
	INBUBBLE:
	mov ecx,dmadr
	mov eax,1
	add [ecx+DM.hometime],eax
	mov eax,[ecx+DM.hometime]
	cmp eax,res.inbubbletime
	jl ATHOME
	mov [ecx +DM.animindex],0
	mov [ecx+DM.hometime],0
	ret


dmootmove endp

;invokes dmootmove for each player in the given array 
allDmootMove proc arrayadr:dword,arrayLength:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayadr
	invoke dmootmove,[ebx]
	dec arrayLength
	add arrayadr,SIZEOF dword
	jmp AGAIN
	FINISHED:
ret
allDmootMove endp

;initializes the player with the init data and the CopyMem proc
;sets the player transparent and freezed
initializeDmoot proc adrdm:dword
	mov eax,adrdm
	mov [eax + DM.freeze],1
	mov [eax+DM.transp],1
	mov ebx,[eax + DM.data]
	invoke CopyMem,ebx,SIZEOF ObjInit,adrdm
ret
initializeDmoot endp

;invokes initializeDmoot for each player in the given array 
initializeAllDmoot proc arrayAdr:dword,arrayLength:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayAdr
	invoke initializeDmoot,[ebx]
	dec arrayLength
	add arrayAdr,SIZEOF dword
	jmp AGAIN
	FINISHED:
ret
initializeAllDmoot endp

;handle with all the events of a player: collusion with bubble and flicker
;acts correspondingly if lastcoladr is not the address of the player (it collided with a bubble)
dmooteventhandler proc adrdm:dword,counter:dword
	mov ebx,adrdm
	;if player is dead - FINISHED
	cmp [ebx + DM.animindex],-1
	je NOTDRAW

	cmp [ebx + DM.animindex],-2
	je GETINBUB

	;if freezed make if flicker
	cmp [ebx + DM.freeze],0
	jg FREEZED

	;handle the player anim
	invoke handleAnim,adrdm,counter
	;check for a collusion
	mov ebx,adrdm
	cmp [ebx + DM.lastcoladr],ebx
	je DRAWDMOOT

	;there was a collision
	;check if transparanted
	cmp [ebx + DM.transp],0
	jg DRAWDMOOT
	
	;if score = 0 player dies
	cmp [ebx + DM.score],0
	je DMOOTDIES
	;decrease score and n
	mov ecx,1
	sub [ebx + DM.score],ecx
	mov ecx,[ebx + DM.score]
	dec n

	;copy the last bubble data to the bubble the player collided with. the bubble the player collided with will disappear
	imul ecx,n,TYPE bubbles
	add ecx,offset bubbles
	;the bubble that disapears will be transparanted
	mov [ecx + DM.transp],1
	invoke CopyMem,ecx,TYPE bubbles,[ebx + DM.lastcoladr]

	;reset the last collision addrres to the player's offset
	mov ebx,adrdm
	mov [ebx + DM.lastcoladr],ebx

	;checks if no bubbles n=0
	cmp n,0
	jne REMAIN

	;n = 1 and player dies, not draw player
	inc n
	DMOOTDIES:
	dec AlivePlayersNum
	mov ebx,adrdm
	mov [ebx + DM.animindex],-1
	jmp NOTDRAW

	REMAIN:
	;if n>0 initialized and draw player
	invoke initializeDmoot,adrdm
	DRAWDMOOT:
	invoke drawImg,adrdm
	NOTDRAW:
ret
	FREEZED:
	;player flickers
	;modulu 2 of the (freeze/60) is the true/false condition to draw the player  
	mov ebx,adrdm
	mov eax,[ebx+ DM.freeze]
	mov ecx,60
	cdq
	div ecx
	mov ecx,2
	cdq
	div ecx
	cmp edx,0
	je DRAWDMOOT
ret
	GETINBUB:
	mov ebx,adrdm
	mov ecx,[ebx + DM.lastcoladr]
	mov edx,[ebx + DM.data]
	mov ebx, (ObjData PTR [edx]).offpic
	invoke drd_imageDraw, ebx,[ecx+DM.pos.x],[ecx+DM.pos.y]
ret
dmooteventhandler endp

;invokes dmooteventhandler for each player in the given array 
allDmootEventHandler proc arrayadr:dword, arrayLength:dword,counter:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayadr
	invoke dmooteventhandler,[ebx],counter
	dec arrayLength
	add arrayadr,SIZEOF dword
	jmp AGAIN
	FINISHED:
ret
allDmootEventHandler endp;

;adds bubble and initialize the player if collide with the goal
;if the player is alive checks if collide with goal
;if collides initialize the player and increase the score and adds a bubble
dmootAddBubble proc adrdm:dword
	mov ebx,adrdm
	;check if alive
	cmp [ebx+DM.animindex],0
	jl DONTADDBUBBLE
	invoke collcheck,adrdm,offset goal
	cmp result,1
	jne DONTADDBUBBLE
	;initialize and increase score
	invoke initializeDmoot,adrdm
	mov ebx,adrdm
	mov ecx,1
	add [ebx+DM.score],ecx
	call addBubble
	DONTADDBUBBLE:
ret
dmootAddBubble endp

;invokes dmootAddBubble for each player in the given array
allDmootAddBubble proc arrayAdr:dword,arrayLength:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayAdr
	invoke dmootAddBubble,[ebx]
	add arrayAdr,SIZEOF dword
	dec arrayLength
	jmp AGAIN
	FINISHED:
ret
allDmootAddBubble endp

;checks if there is a collision between a player and an object
;if collide it changes the player's lastcoladr to the object's offset 
dmootCollCheck proc adrdm:dword,adr:dword
	invoke collcheck,adrdm,adr
	cmp result,0
	je NOcollision
	mov ebx,adrdm
	mov ecx,adr
	mov [ebx+DM.lastcoladr],ecx
	cmp [ebx+DM.transp],0
	jg NOcollision
	mov [ebx+DM.animindex],-2
	NOcollision:
ret
dmootCollCheck endp

;invokes dmootCollCheck for each player in the given array. with a given object
allDmootCollCheck proc arrayAdr:dword,arrayLength:dword,adr:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayAdr
	invoke dmootCollCheck,[ebx],adr
	dec arrayLength
	add arrayAdr,SIZEOF dword
	jmp AGAIN
	FINISHED:
ret
allDmootCollCheck endp

;prints each player's score in the given array using printNum proc 
AllDmootPrintScores proc posx:dword, posy:dword, arrayadr:dword, arrayLength:dword
	AGAIN:
	cmp arrayLength,0
	je FINISHED
	mov ebx,arrayadr
	mov ebx,[ebx]
	;check if alive
	cmp [ebx + DM.animindex],-1
	je NOTPRINT
	invoke printNum,[ebx+DM.score],posx,posy
	add posy,60
	NOTPRINT:
	dec arrayLength
	add arrayadr,SIZEOF dword
	jmp AGAIN
	FINISHED:
ret
AllDmootPrintScores endp
