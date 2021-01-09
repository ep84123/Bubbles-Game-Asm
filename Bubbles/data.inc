.const

;struct that contains the paths of the pictures in the hard drive
HD struct
	bubAnim BYTE "pics/bubble/im01.png",0
	piter BYTE "pics/piterpen.png",0
	bgpath byte "pics/bg.png",0
	numspath byte  "pics/nums.png",0
	superman byte "pics/superman.png",0
	goal byte "pics/goal.png",0
	spiderman byte "pics/spiderman.png",0
HD ends

;struct that contains Img structs in the ram. (the pictures) 
Pics struct
	bubAnim Img 48 dup(<>)
	piter Img<>
	bg Img<>
	nums Img<>
	superman Img<>
	goal Img<>
	spiderman Img<>
Pics ends

; struct that contains the constants
Resorses struct
	;backround width
	wbg dword 1280

	;backround hight
	hbg dword 650

	;the decimal multiplication number
	deci dword 10000

	;how many runs the system needs to add a bubble
	addbubblecount dword 200

	;energy conserve consts
	energyLowBound dword 9980
	energyLowConst	dword 10010
	energyHighBound dword 10020
	energyHighConst	dword 9990

	;glich defense distance 
	defensedist dword 90

	;how many runs can a character stay transparanted at home
	hometimelimit dword 1000
	;how many runs acharacter can't move after initialized
	freezeTime dword 300

	inbubbletime dword 400
Resorses ends

; struct of keys, defines the keys of each player
Keys struct
	left dword ?
	right dword ?
	up dword ?
	down dword ?
Keys ends

;struct defines a bubble - or any object that acts like a bubble
Bubble struct
	;the position of the bubble in regular display
	pos POINT<>
	;the direction & position of the bubble in decimal display
	drcdec POINT<>
	posdec POINT <>

	;the offset of the Data struct describing the bubble
	data dword ?
	;to match the Bubble struct with the Dmoot struct so DM struct would be relevant 
	;and to be able to initialize the keys the Dmoot struct declaration statement need to fill place in bubble
	nonsense dword 1,1,1,1;just filling in place (:

	;contains the address of the last bubble to collide with this bubble 
	;in order to prevent bubbles from colliding many times in a row because they still touch
	;when initialized or after a collision with the wall it contains the address of the bubble itself
	lastcoladr dword 0

	;the bubble is transparent (it can't collide with other bubbles) if transp >=1
	transp dword 1 ;

	;the current picture index of the bubble
	animindex dword 0
Bubble ends

;struct describes characters (players)
Dmoot struct
	;same as bubble
	pos POINT<>
	drcdec POINT<>
	posdec POINT <>
	data dword ?

	;the keys of the player
	keys Keys<>

	lastcoladr dword 0

	;the character can't collide with bubbles if transp = 1 
	transp dword 1

	;the current picture index of the character
	animindex dword 0

	;player's score if less than 0 player dies
	score dword 1

	;the number of runs that the player has spent at home (transparent) in a row 
	;if greater than res.hometimelimit the player is not transparent anymore
	homeTime dword 0

	;the number of runs initializing the character to home that the char can't move
	freeze dword 0
Dmoot ends

;the initial position and direction data of a type of an object
ObjInit struct
	pos POINT<>
	drcdec POINT<>
	posdec POINT<>
ObjInit ends

;the data describes a general type of object
ObjData struct
	;an init struct - the program relies that this is the first item in the struct
	init ObjInit<>

	;picture hight and width
	imgh dword ?
	imgw dword ?

	;the collision distance of the object - mostly its frame parameter
	coldist dword ?
	
	;the location in the string of the picture path in the hd is the index of the picture(if exists)
	imgconst dword ?

	;a constant for hitting a wall - multiplies the vertical velocity to the wall by it (-1 for bubble,0 for player) 
	wallconst dword ?

	;the offset of the object picture/s in the ram and of the path in the hd 
	offpic dword ?
	offpathd dword ?
	;transparanted color from the object's picture/s
	transpColor dword ?

	;number of pictures the object has
	animnum dword ?

	;number of runs for changing the picture with the next one 
	animconst dword ?
ObjData ends

;struct defining a type of chracter
DmootData struct
	;general object struct - the program relies that this is the first item in the struct
	data ObjData <>
	;character's decimal speed
	spdec dword ?
	;size of a character's home x - width, y - hight
	homx dword ?
	homy dword ?
DmootData ends

;a struct that helps navigate between the Dmoot and bubble structs
;contains the place in the struct 
DM struct
	pos POINT<0,4>
	drcdec POINT<8,12>
	posdec POINT<16,20>
	data dword 24
	keys Keys<28,32,36,40>
	lastcoladr dword 44
	transp dword 48
	animindex dword 52
	score dword 56
	hometime dword 60
	freeze dword 64
DM ends

;struct contains all the object types of the program - some values needs to be initialized later
Data struct
	bubbled ObjData<<<1,1>,<5000,5000>,<10000,10000>>,100,100,50,14,-1,?,?,0000000h,?,83>
	piterd DmootData<<<<1180,550>,<0,0>,<11800000,5500000>>,100,100,20,1,0,?,?,0ffffffh,?,1>,20000,200,200>
	supermand DmootData<<<<1180,550>,<0,0>,<11800000,5500000>>,100,100,20,1,0,?,?,0000000h,?,1>,20000,200,200>
	spidermand DmootData<<<<1180,550>,<0,0>,<11800000,5500000>>,100,100,20,1,0,?,?,0ffffffh,?,1>,20000,200,200>
	goald ObjData<<<0,0>,<0,0>,<0,0>>,80,80,30,1,-1,?,?,0000000h,?,1>
Data ends
	
;ram struct - contains the data and the pictures
Ram struct
	data Data<>
	pics Pics<>
Ram ends

.data
;create global variables
;uses to store results and save them during processes
result BYTE 0
;indexes of loops
i dword 0
j dword 0
sin dword 0
cos dword 0
;number of bubbles
n dword 1

;energy variables
energy dword 5000
TeoreticEnergy dword 0
dist dword 0

;create all the structs
res Resorses<>
hd HD<>
ram Ram<>

;array contains the addresses of all of the structs describing objects
dataStructsOffsets dword offset ram.data.bubbled,offset ram.data.piterd,offset ram.data.supermand,offset ram.data.goald,offset ram.data.spidermand

;create players
piter Dmoot <<?,?>,<?,?>,<?,?>,offset ram.data.piterd,<VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN>>
piter2 Dmoot <<?,?>,<?,?>,<?,?>,offset ram.data.piterd,<VK_A,VK_D,VK_W,VK_S>>
piter3 Dmoot <<?,?>,<?,?>,<?,?>,offset ram.data.piterd,<VK_G,VK_J,VK_Y,VK_H>>
superman Dmoot<<?,?>,<?,?>,<?,?>,offset ram.data.supermand,<VK_A,VK_D,VK_W,VK_S>>
spiderman Dmoot<<?,?>,<?,?>,<?,?>,offset ram.data.spidermand,<VK_H,VK_K,VK_U,VK_J>>

;array contains player's addresses
offplayers dword offset piter,offset superman,offset spiderman
AlivePlayersNum dword LENGTHOF offplayers

;create bubbles - array using dup and a goal "bubble"
goal Bubble <<0,0>,<0,0>,<0,0>,offset ram.data.goald>
bubbles Bubble 30 dup(<<1,1>,<5000,5000>,<10000,10000>,offset ram.data.bubbled>)

.code
;initialize the locations of the pictures in the ram and in the hd in the object structs
;and also the number of pictures for each object type
setData proc
	mov ram.data.bubbled.offpic,offset ram.pics.bubAnim
	mov ram.data.goald.offpic,offset ram.pics.goal
	mov ram.data.piterd.data.offpic,offset ram.pics.piter
	mov ram.data.supermand.data.offpic,offset ram.pics.superman
	mov ram.data.spidermand.data.offpic,offset ram.pics.spiderman

	mov ram.data.piterd.data.offpathd,offset hd.piter
	mov ram.data.supermand.data.offpathd,offset hd.superman
	mov ram.data.spidermand.data.offpathd,offset hd.spiderman
	mov ram.data.bubbled.offpathd,offset hd.bubAnim
	mov ram.data.goald.offpathd,offset hd.goal

	mov ram.data.bubbled.animnum,LENGTHOF ram.pics.bubAnim
	mov ram.data.goald.animnum,LENGTHOF ram.pics.goal
	mov ram.data.piterd.data.animnum,LENGTHOF ram.pics.piter
	mov ram.data.supermand.data.animnum,LENGTHOF ram.pics.superman
	mov ram.data.spidermand.data.animnum,LENGTHOF ram.pics.spiderman
ret
setData endp
