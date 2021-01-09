include drd.inc
includelib drd.lib
include data.inc
include Main_loops.inc
.code

load proc
	call setData
	invoke initializeAllDmoot,offset offplayers,LENGTHOF offplayers
	;intialize the fpu
	finit
	;load screen background
	invoke drd_init,res.wbg,res.hbg,0
	invoke drd_imageLoadFile,offset hd.bgpath,offset ram.pics.bg

	;load num strip
	invoke drd_imageLoadFile,offset hd.numspath,offset ram.pics.nums
	invoke drd_imageSetTransparent,offset ram.pics.nums,0000000h

	invoke loadAllObjPics,offset dataStructsOffsets, LENGTHOF dataStructsOffsets
ret
load endp

update proc counter:dword
	
	invoke drd_processMessages
	invoke allDmootMove,offset offplayers,LENGTHOF offplayers
	invoke bubbleAdder,counter
	
	mov i,0
	;invokes updateInnerLoop for each bubble
	LOOPp:
		imul edi, i, TYPE bubbles
		add edi, offset bubbles ;edi is the address of the bubble i
		invoke updateInnerLoop,edi,i
		inc i
		mov ebx,i
		cmp ebx,n
		jl LOOPp
ret
update endp

draw proc counter:dword
	;initialze the screen
	invoke drd_pixelsClear,0
	invoke drd_imageDraw,offset ram.pics.bg,0,0

	;calculate the multiplication constant for the bubbles drc according to the energy in the system
	mov ecx,res.deci
	mov eax,TeoreticEnergy
	mul res.energyLowBound
	div res.deci
	cmp energy,eax
	jg FINE
	mov ecx,res.energyLowConst
	jmp FINE2
	FINE:
	mov eax,TeoreticEnergy
	mul res.energyHighBound
	div res.deci
	cmp energy,eax
	jl FINE2
	mov ecx,res.energyHighConst
	FINE2:
	
	invoke drawLoop,ecx,counter
	call calTeoreticEnergy

	;if there is a player alive draw goal,handle players and print score
	cmp AlivePlayersNum,0
	jng SKIP
	invoke drawImg,offset goal
	invoke allDmootEventHandler,offset offplayers,LENGTHOF offplayers,counter
	invoke AllDmootPrintScores,20,150,offset offplayers,LENGTHOF offplayers
	SKIP:
	invoke drd_flip
ret
draw endp
