include Math_funcs.inc

; For definition of "decimalic", see Math_funcs.inc
; In this file, "character" usually refers to a character on the screen, and not a char.
.code

;checks if all the bubbles are in. if not increases n (which adds a bubble)
addBubble proc
	cmp n,LENGTHOF bubbles
	je RETADDBUBBLE
	inc n
	RETADDBUBBLE:
ret
addBubble endp

;checks if a character hits the walls return the state in result
;receives the character's address, width, and hight
;if it hits the vertical wall: result = 1. if the horizontal = 2. if both = 3. if none result = 0
;uses the position on the screen and not the decimalic position
checkwalls proc adrObj:dword, objwidth:dword, objhight:dword
	mov result,0
	mov ebx,adrObj
	mov ecx,objwidth
	mov edx,objhight
	mov eax,[ebx+DM.pos.x] 
	cmp eax,0
	jng RES1
	add eax,ecx
	cmp eax,res.wbg
	jnl RES1
	RETRES1:
	mov eax,[ebx+DM.pos.y] 
	cmp eax,0
	jng RES2
	add eax,edx
	cmp eax,res.hbg
	jnl RES2
ret

	RES1:
	add result,1
	jmp RETRES1

	RES2:
	add result,2
	ret

checkwalls endp

;checks collision between two characters and updates the result in result
;receives the character's addresses and the distance of the collision
;uses the position on the screen and not the decimalic position
;if the distance between the characters is lower or equal to the distance the result is 1 (true) else it is 0(false)
;the equation for comparing the distances is: (dx)^2 + (dy)^2 <= distance^2
;dx is the x difference between the characters.

collisioncheck proc adrObj:dword, adrObj2:dword, distance:dword
	mov eax,adrObj
	mov edx,adrObj2
	mov result,0
	mov ebx,[eax+DM.pos.x]
	mov ecx,[edx+DM.pos.x]
	sub ebx,[edx+DM.pos.x]
	imul ebx,ebx
	mov ecx,[eax+DM.pos.y]
	sub ecx,[edx+DM.pos.y]
	imul ecx,ecx
	add ebx,ecx
	mov ecx,distance
	imul ecx,ecx
	cmp ebx,ecx
	jg RESS0
	mov result,1
	RESS0:
ret
collisioncheck endp

;invokes collisioncheck with the two given addresses after calculating the default collision distance of the Objects
;the total collision distance is the sum of each object's collision distance 
collcheck proc adrObj:dword,adrObj2:dword
	mov ebx,adrObj
	mov ebx,[ebx + DM.data]
	mov ebx,(ObjData PTR [ebx]).coldist
	mov ecx,adrObj2
	mov ecx,[ecx + DM.data]
	add ebx,(ObjData PTR [ecx]).coldist
	invoke collisioncheck,adrObj,adrObj2,ebx
ret
collcheck endp

;receives a character's address and rotates its decimalic direction (drcdec) according to the sin and cos of the angle
;after the rotation x becomes the length vector in the rotation angle, y becomes the length vector in the rotation angle + 90
;follows the equations below when X is the new drcdec.x and x is the old one, same for y
;X = x*cos + y*sin
;Y = y*cos - x*sin
;all the numbers are decimalic - that's why dividing by res.deci, make sure it stays decimalic

rotate proc adrObj:dword
	mov ebx,adrObj
	mov ecx,[ebx + DM.drcdec.x]
	mov eax,[ebx + DM.drcdec.y]
	imul ecx, cos
	imul sin
	add eax,ecx
	cdq
	idiv res.deci
	mov ecx,[ebx + DM.drcdec.x]
	mov [ebx + DM.drcdec.x],eax
	mov eax,[ebx + DM.drcdec.y]
	imul ecx, sin
	imul cos
	sub eax,ecx
	cdq
	idiv res.deci
	mov [ebx + DM.drcdec.y],eax
ret
rotate endp

;updates the location of a character according to its direction
;receives the character's address as an input
;for both x and y it sums up the accurate position and direction(posdec,drcdec)
;it updates posdec to the new accurate location and rounds it to display it on the screen
;the round function puts the rounded value in the character's (not decimalic) position 

updateloc proc adrObj:dword
	mov eax,adrObj
	mov ebx,[eax+DM.posdec.x]
	add ebx,[eax+DM.drcdec.x]
	mov [eax+DM.posdec.x],ebx
	add eax,DM.pos.x
	invoke round,ebx, eax
	mov eax,adrObj
	mov ebx,[eax+DM.posdec.y]
	add ebx,[eax+DM.drcdec.y]
	mov [eax+DM.posdec.y],ebx
	add eax,DM.pos.y
	invoke round,ebx, eax
ret
updateloc endp

;if there is a collision this function is called to make sure both bubbles are solid
;receives a bubble's address and check its transp state 
;if (transp == true) then it increases the result and changes transp state so the bubble would remain transp
;the bubble shouldn't remain transp because it collided with another bubble

transphandler proc adrObj:dword
	mov eax,adrObj
	cmp [eax + DM.transp],1
	jl NOTHING
	inc result
	add [eax + DM.transp],1
	NOTHING:
ret
transphandler endp

;loads all the pictures of an object/ or just a bunch of pictures
;receives the addresses of the: first picture path, the pictures array in the ram,
;the color to be transparanted, and after how many chars in the path is the picture index
;j is the loop index and i is the address of the current picture in the ram

loadanims proc adrhd:dword,adram:dword,color:dword,animnum:dword,imgconst:dword
	mov j,0
	mov eax,adram
	mov i,eax
	LOADLOOP:
	;foreach picture - load the picture and transparent the given color
	invoke drd_imageLoadFile,adrhd,i
	invoke drd_imageSetTransparent,i,color
	inc j
	add i,SIZEOF Img
	
	;puts in al the tens digit and in ah the one's digit of j+1
	mov ax,WORD PTR j
	inc ax
	mov bl,10
	div bl

	;converts to ascii char
	add al,48
	add ah,48

	;movs the chars to the index place in path string
	mov ecx,adrhd
	add ecx,imgconst
	mov [ecx],al
	mov [ecx +1],ah

	;checks if finished
	mov ecx,animnum
	cmp j,ecx
	jl LOADLOOP
ret
loadanims endp

;invokes loadanims for a type of object(Data struct)
loadAnims proc adrdata:dword
	mov ebx,adrdata
	invoke loadanims,(ObjData PTR [ebx]).offpathd,(ObjData PTR [ebx]).offpic,(ObjData PTR [ebx]).transpColor,(ObjData PTR [ebx]).animnum,(ObjData PTR [ebx]).imgconst	
ret
loadAnims endp

;load all anims foreach type of object in a given array
loadAllObjPics proc adrdata:dword,num:dword
	AGAIN:
	cmp num,0
	je ENDLOAD
	mov ebx,adrdata
	invoke loadAnims,[ebx]
	add adrdata,SIZEOF dword
	dec num
	jmp AGAIN
	ENDLOAD:
ret
loadAllObjPics endp

;multiplies a direction aith a given decimal factor of an object using muladr proc
muldrc proc adrdm:dword,factordec:dword
	mov ebx,adrdm
	add ebx,DM.drcdec.x
	invoke muladr,ebx,factordec
	mov ebx,adrdm
	add ebx,DM.drcdec.y
	invoke muladr,ebx,factordec
ret
muldrc endp

;updates the anim index of an object corresponding to the counter value (how many runs)
;and the animconst specified in the type of object Data struct
handleAnim proc adrObj:dword,counter:dword
	;divides counter by animconst 
	mov ecx,adrObj
	mov eax,counter
	cdq
	mov ebx,[ecx+DM.data]
	mov ebx,(ObjData PTR [ebx]).animconst
	div ebx

	;if remainder is 0 increase object's animindex and make sure it is valid (not over the max animindex)
	cmp edx,0
	jne NOTCHANGEANIM
	mov ecx,adrObj
	mov eax,[ecx+DM.animindex]
	inc eax
	cdq
	mov ebx,[ecx+DM.data]
	mov ebx,(ObjData PTR [ebx]).animnum
	div ebx
	mov [ecx+DM.animindex],edx
	NOTCHANGEANIM:
ret
handleAnim endp

;receives an object's address, check for collision with walls and acts correspondingly
handlewalls proc adr:dword
	;check collision with checkwalls proc
	mov ebx,adr
	mov ecx,[ebx+DM.data]
	invoke checkwalls,adr,(ObjData PTR [ecx]).imgw,(ObjData PTR [ecx]).imgh
	mov ebx,adr
	mov ecx,[ebx+DM.data]
	mov edx,(ObjData PTR [ecx]).wallconst

	;result >= 2 if collided with horizontal wall 
	cmp result,2
	jl NOTNEGY
	mov [ebx + DM.lastcoladr], ebx ;makes sure a collision won't be cancelled
	;muls y direction by the object's wall const
	mov ecx,[ebx+DM.drcdec.y]
	imul ecx,edx
	mov [ebx+DM.drcdec.y],ecx
	sub result, 2
	NOTNEGY:
	;result = 1 if  collided with vertical wall
	;does the same as y for x

	cmp result,1
	jne NOTNEGX
	mov [ebx + DM.lastcoladr], ebx ;makes sure a collision won't be cancelled
	mov ecx,[ebx+DM.drcdec.x]
	imul ecx,edx
	mov [ebx+DM.drcdec.x],ecx
	NOTNEGX:
ret
handlewalls endp

;calculate the theoretic energy in the system with the bubble's initial velocities
calTeoreticEnergy proc
	;TeoreticEnergy = n*(vel x^2 + vel y ^2)
	mov ebx,ram.data.bubbled.init.drcdec.x
	imul ebx,ebx
	mov eax,ram.data.bubbled.init.drcdec.y
	mul eax
	add eax,ebx
	cdq
	idiv res.deci
	imul eax,n
	mov TeoreticEnergy,eax
ret
calTeoreticEnergy endp

;print a two digits number on the screen according to the input
printNum proc num:dword,posx:dword,posy:dword
	;puts in eax the tens digit and in edx the one's digit of num
	mov eax,num
	mov ebx,10
	cdq
	div ebx
	;converts them to the x pos of the digit in the strip
	imul edx,18
	mov num,edx ;saves edx in num
	imul eax,18
	;draw the digits according to the data
	invoke drd_imageDrawCrop,offset ram.pics.nums,posx,posy,eax,0,18,40
	add posx,18
	invoke drd_imageDrawCrop,offset ram.pics.nums,posx,posy,num,0,18,40
ret
printNum endp

;draw an object's img acoording to it's data 
drawImg proc adrObj:dword
	mov ecx,adrObj
	imul ebx,[ecx+DM.animindex],SIZEOF Img
	mov edx,[ecx + DM.data]
	add ebx, (ObjData PTR [edx]).offpic
	invoke drd_imageDraw, ebx,[ecx+DM.pos.x],[ecx+DM.pos.y]
ret
drawImg endp