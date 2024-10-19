****************************************************************************
*									   *	
*									   *
*		  Sound Abspiel Routine zu Sound FX		   	   *
*									   *
*		       � 1988 LINEL Switzerland				   *	
*									   *	
*									   *	
*									   *
****************************************************************************

;Einbau:
;	1.Sound FX laden
;	2.Song schreiben oder einladen
;	3.Sounddaten mit Save Datas abspeichern 
;	4.Sound FX�verlassen und Seka laden
;	5.Source des Programms laden in den wir den Song einbauen
;	  wollen.
;	6.Am Anfang des Programms 'bsr StartSound' einsetzen
;	7.Am Schluss des Programms 'bsr StopSound' einsetzen
;	8.Diesen AbspielSource laden
;	9.Das Label 'Laenge' auf die Laenge des Datenfiles setzen
;	10.Assemblieren
;	11.DatenFile mit 'ri' nach 'datas' laden
;	12.fertig

;------------------------------------------------------------------------


	Laenge = 60000			;L�nge der Sounddaten

;------------------------------------------------------------------------
Beispiel:
	bsr.s	StartSound
	bsr.s	Intro
	bsr.L	StopSound
	moveq	#0,d0
	rts
;-------------------------------------------------------------------------

Intro:
	move	d0,$dff180
	addq	#1,d0
	btst	#6,$bfe001
	bne.s	Intro
	rts

;---------------------------------------------------------------------------

;Ergebis in D0           0=alles Ok.        $ff = Timer nicht bekommen

StartSound:
	movem.l	d1-d7/a0-a6,-(SP)
	clr	ChipFlag		;Flag fuer Daten im Chip
	move.l	4,a6			;ExecBase
	move.l	SongPointer(pc),a1	;Zeiger auf Daten
	jsr	-534(A6)		;TypeOfMem()
	btst	#1,d0			;Sind Daten im Chip ?
	bne.s	ChipOK			;ja ->
	st	ChipFlag		;Flag fuer AllocMemChip setzen
	move.l	#Laenge,d0		;wenn Fast Memory ->
	addq.l	#4,d0			
	moveq	#2,d1			;Chip Speicher
	jsr	-198(a6)		;reservieren
	moveq	#-1,d5			;Fehlerflag
	tst.l	d0			;Speicher bekommen ?
	beq.L	EndStart		;wenn nicht -> Ende
	move.l	d0,a1			;Ziel
	move.l	SongPointer(pc),a0	;Quelle
	move.l	#Laenge,d1
	addq.l	#4,d1			;Reserve
	lsr.l	#2,d1			;divu #4
	subq	#1,d1			;f�r dbf
CopyMem:
	move.l	(a0)+,(A1)+		;Daten ins ChipMem
	move	(a0),$dff180
	dbf	d1,CopyMem		;kopieren
	move.l	d0,SongPointer

ChipOk:
	move.l	SongPointer(pc),a0	;Zeiger auf SongDaten
	add	#60,a0			;Laengentabelle ueberspringen
	move.b	470(a0),AnzPat+1	;Laenge des Sounds
	move	4(A0),DelayValue 	;Geschwindigkeit
	bsr	SongLen			;L�nge der Songdaten berechnen
	add.l	d0,a0			;Zur Adresse der Songstr.
	add.w	#600,a0			;Laenge der SongStr.
	move.l	SongPointer(pc),a2
	lea	Instruments(pc),a1	;Tabelle auf Samples
	moveq	#14,d7			;15 Instrumente
CalcIns:
	move.l	a0,(A1)+		;Startadresse des Instr.
	add.l	(a2)+,a0		;berechnen un speichern
	dbf	d7,CalcIns

	lea	CiaaResource(pc),a1	;'ciaa.resource'
	moveq	#0,d0			;Version egal
	jsr	-498(A6)		;OpenResource()
	move.l	d0,CiaaBase		;Resource Base speichern
	move.l	d0,a6
	bsr	PlayDisable		;Sound DMA abschalten
	lea	Interrupt(pc),a1	;Sound Interupt Structure
	moveq	#0,d0			;TimerA
	jsr	-6(A6)			;installieren
	move.l	d0,d5			;ergebnis speichern
	bsr	PlayInit		;Loop Bereich setzen
	bsr	PlayEnable		;Player erlauben
	bsr	InitTimer		;Timer starten
	moveq	#0,d0			;Ergebnisregister loeschen
EndStart:
	tst.l	d5			;ergebnis von Resource
	sne	d0			;ergebnis in d0 setzen 
	movem.l	(SP)+,d1-d7/a0-a6
	rts

;---------------------------------------------------------------------------

StopSound:
	movem.l	d1-d7/a0-a6,-(SP)
	move.l	4,a6			;ExecBase
	tst	ChipFlag		;mussten wir Speicher reservieren ?
	beq.s	NoFreeSong		
	move.l	#Laenge,d0		;L�nge der Daten
	addq.l	#4,d0			;Reserve
	move.l	SongPointer(pc),a1	;Zeiger auf Daten
	jsr	-210(a6)		;FreeMem()
NoFreeSong:
	move.l	CiaaBase(pc),a6		;Zeiger auf Ciaa Resource
	lea	Interrupt(pc),a1	;Zeiger auf Int. Strukture
	moveq	#0,d0			;Timer A
	jsr	-12(A6)			;Interupt entfernen
	bsr	PlayDisable		;Player sperren
	moveq	#0,d0			;Alles Ok	
	movem.l	(SP)+,d1-d7/a0-a6
	rts
;---------------------------------------------------------------------------

SongLen:
	movem.l	d1-d7/a0-a6,-(SP)
	move.l	SongPointer,a0
	lea	532(A0),a0
	move	AnzPat(pc),d2		;wieviel Positions
	subq	#1,d2			;f�r dbf
	moveq	#0,d1
	moveq	#0,d0
SongLenLoop:
	move.b	(a0)+,d0		;Patternnummer holen
	cmp.b	d0,d1			;ist es die h�chste ?
	bhi.s	LenHigher		;nein!
	move.b	d0,d1			;ja
LenHigher:
	dbf	d2,SongLenLoop
	move.l	d1,d0			;Hoechste BlockNummer nach d0
	addq	#1,d0			;plus 1
	mulu	#1024,d0		;Laenge eines Block
	movem.l	(SP)+,d1-d7/a0-a6
	rts

;--------------------------------------------------------------------	


Interrupt:
	dc.l	0			;letzter Node
	dc.l	0			;n�chster Node
	dc.b	2			;Node Type = Interrupt
	dc.b	0 			;Priorit�t
	dc.l	InterruptName		;Name
	dc.l	0			;Zeiger auf Daten
	dc.l	IntCode			;Interrupt Routine

;-------------------------------------------------------------------

InitTimer:
	move.b	#%10000001,$bfee01	;Timer starten
	lea	DelayValue(pc),a1
	move.b	1(a1),$bfe401		;Timer A low
	move.b	0(a1),$bfe501		;Timer A high
	rts

;--------------------------------------------------------------------

PlayInit:
	lea	Instruments(pc),a0	;Zeiger auf instr.Tabelle
	moveq	#14,d7			;15 Instrumente
InitLoop:	
	move.l	(A0)+,a1		;Zeiger holen
	clr.l	(A1)			;erstes Longword l�schen
	dbf	d7,InitLoop
	rts

;-----------------------------------------------------------------------

PlayEnable:
	lea	$dff000,a0		;AMIGA
	move.w	#-1,PlayLock		;player zulassen
	clr	$a8(A0)			;Alle Voloumenregs. auf 0
	clr	$b8(A0)
	clr	$c8(a0)
	clr	$d8(a0)
	clr.w	Timer			;zahler auf 0
	clr.l	TrackPos		;zeiger auf pos
	clr.l	PosCounter		;zeiger innehalb des pattern
	rts
;----------------------------------------------------------------------

PlayDisable:
	lea	$dff000,a0		;AMIGA
	clr.w	PlayLock		;player sperren
	clr	$a8(a0)			;volumen auf 0
	clr	$b8(a0)
	clr	$c8(a0)
	clr	$d8(a0)
	move.w	#$f,$96(A0)		;dma sperren
	rts

;---------------------------------------------------------------------

IntCode:
	bsr	PlaySong		;Note spielen
	moveq	#0,d0			;kein Fehler
	rts

;----------------------------------------------------------------------


;hier werden 5 * effekte gespielt und einmal der song

PlaySong:				;HauptAbspielRoutine
	movem.l	d0-d7/a0-a6,-(SP)
	addq.w	#1,Timer		;z�hler erh�hen
	cmp.w	#6,Timer		;schon 6?
	bne.s	CheckEffects		;wenn nicht -> effekte
	clr.w	Timer			;sonst z�hler l�schen
	bsr 	PlaySound		;und sound spielen
NoPlay:	movem.l	(SP)+,d0-d7/a0-a6
	rts

;-------------------------------------------------------------------
CheckEffects:
	moveq	#3,d7			;4 kan�le
	lea	StepControl0,a4
	lea	ChannelData0(pc),a6	;zeiger auf daten f�r 0
	lea	$dff0a0,a5		;Kanal 0
EffLoop:
	movem.l	d7/a5,-(SP)
	bsr.s	MakeEffekts		;Effekt spielen
	movem.l	(Sp)+,d7/a5
NoEff:
	add	#8,a4
	add	#$10,a5			;n�chster Kanal
	add	#22,a6			;N�chste KanalDaten
	dbf	d7,EffLoop
	movem.l	(a7)+,d0-d7/a0-a6
	rts

MakeEffekts:
	move	(A4),d0
	beq.s	NoStep
	bmi.s	StepItUp
	add	d0,2(A4)
	move	2(A4),d0
	move	4(A4),d1
	cmp	d0,d1
	bhi.s	StepOk
	move	d1,d0
StepOk:
	move	d0,6(a5)
	MOVE	D0,2(A4)
	rts

StepItUp:
	add	d0,2(A4)
	move	2(A4),d0
	move	4(A4),d1
	cmp	d0,d1
	blt.s	StepOk
	move	d1,d0
	bra.s	StepOk



NoStep:
	move.b	2(a6),d0
	and.b	#$0f,d0
	cmp.b	#1,d0
	beq	appreggiato
	cmp.b	#2,d0
	beq	pitchbend
	cmp.b	#3,d0
	beq	LedOn
	cmp.b	#4,d0
	beq	LedOff
	cmp.b	#7,d0
	beq.s	SetStepUp
	cmp.b	#8,d0
	beq.s	SetStepDown
	rts

LedOn:
	bset	#1,$bfe001
	rts
LedOff:
	bclr	#1,$bfe001
	rts

SetStepUp:
	moveq	#0,d4
StepFinder:
	clr	(a4)
	move	(A6),2(a4)
	moveq	#0,d2
	move.b	3(a6),d2
	and	#$0f,d2
	tst	d4
	beq.s	NoNegIt
	neg	d2
NoNegIt:	
	move	d2,(a4)
	moveq	#0,d2
	move.b	3(a6),d2
	lsr	#4,d2
	move	(a6),d0
	lea	NoteTable,a0

StepUpFindLoop:
	move	(A0),d1
	cmp	#-1,d1
	beq.s	EndStepUpFind
	cmp	d1,d0
	beq.s	StepUpFound
	addq	#2,a0
	bra.s	StepUpFindLoop
StepUpFound:
	lsl	#1,d2
	tst	d4
	bne.s	NoNegStep
	neg	d2
NoNegStep:
	move	(a0,d2.w),d0
	move	d0,4(A4)
	rts

EndStepUpFind:
	move	d0,4(A4)
	rts
	
SetStepDown:
	st	d4
	bra.s	StepFinder


StepControl0:
	dc.l	0,0
StepControl1:
	dc.l	0,0
StepControl2:
	dc.l	0,0
StepControl3:
	dc.l	0,0


appreggiato:
	lea	ArpeTable,a0
	moveq	#0,d0
	move	Timer,d0
	subq	#1,d0
	lsl	#2,d0
	move.l	(A0,d0.l),a0
	jmp	(A0)

Arpe4:	lsl.l	#1,d0
	clr.l	d1
	move.w	16(a6),d1
	lea.l	NoteTable,a0
Arpe5:	move.w	(a0,d0.l),d2
	cmp.w	(a0),d1
	beq.s	Arpe6
	addq.l	#2,a0
	bra.s	Arpe5



Arpe1:	clr.l	d0
	move.b	3(a6),d0
	lsr.b	#4,d0
	bra.s	Arpe4


Arpe2:	clr.l	d0
	move.b	3(a6),d0
	and.b	#$0f,d0
	bra.s	Arpe4

Arpe3:	move.w	16(a6),d2
	
Arpe6:	move.w	d2,6(a5)
	rts


pitchbend:
	clr.l	d0
	move.b	3(a6),d0
	lsr.b	#4,d0
	tst.b	d0
	beq.s	pitch2
	add.w	d0,(a6)
	move.w	(a6),6(a5)
	rts
pitch2:	clr.l	d0
	move.b	3(a6),d0
	and.b	#$0f,d0
	tst.b	d0
	beq.s	pitch3
	sub.w	d0,(a6)
	move.w	(a6),6(a5)
pitch3:	rts


;--------------------------------------------------------------------

PlaySound:
	move.l	SongPointer(pc),a0	;Zeiger auf SongFile
	add	#60,a0			;Laengentabelle ueberspringen
	move.l	a0,a3
	move.l	a0,a2
	lea	600(A0),a0		;Zeiger auf BlockDaten
	add	#472,a2			;zeiger auf Patterntab.
	add	#12,a3			;zeiger auf Instr.Daten
	move.l	TrackPos(pc),d0		;Postionzeiger
	clr.l	d1
	move.b	(a2,d0.l),d1		;dazugeh�rige PatternNr. holen
	moveq	#10,d7
	lsl.l	d7,d1			;*1024 / l�nge eines Pattern
	add.l	PosCounter,d1		;Offset ins Pattern
	clr.w	DmaCon
	lea	StepControl0,a4
	lea	$dff0a0,a5		;Zeiger auf Kanal0
	lea	ChannelData0(pc),a6	;Daten f�r Kanal0
	moveq	#3,d7			;4 Kan�le
SoundHandleLoop:
	bsr	PlayNote		;aktuelle Note spielen
	add	#8,a4
	add.l	#$10,a5			;n�chster Kanal
	add.l	#22,a6			;n�chste Daten
	dbf	d7,SoundHandleLoop	;4*
	
	move	DmaCon(pc),d0		;DmaBits
	bset	#15,d0			;Clear or Set Bit setzen
	move.w	d0,$dff096		;DMA ein!

	move	#300,d0			;Verz�gern (genug f�r MC68030)
Delay2:
	dbf	d0,Delay2

	lea	ChannelData3(pc),a6
	lea	$dff0d0,a5
	moveq	#3,d7
SetRegsLoop:
	move.l	10(A6),(a5)		;Adresse
	move	14(A6),4(A5)		;l�nge
NoSetRegs:
	sub	#22,a6			;n�chste Daten
	sub	#$10,a5			;n�chster Kanal
	dbf	d7,SetRegsLoop
	tst	PlayLock
	beq.s	NoEndPattern
	add.l	#16,PosCounter		;PatternPos erh�hen
	cmp.l	#1024,PosCounter	;schon Ende ?
	blt.s	NoEndPattern

	clr.l	PosCounter		;PatternPos l�schen
	addq.l	#1,TrackPos		;Position erh�hen
NoAddPos:
	move.w	Anzpat(pc),d0		;AnzahlPosition
	move.l	TrackPos(pc),d1		;Aktuelle Pos
	cmp.w	d0,d1			;Ende?
	bne.s	NoEndPattern		;nein!
	clr.l	TrackPos		;ja/ Sound von vorne
NoEndPattern:
	rts



PlayNote:
	clr.l	(A6)
	tst	PlayLock		;Player zugellassen ?
	beq.s	NoGetNote		;
	move.l	(a0,d1.l),(a6)		;Aktuelle Note holen
NoGetNote:
	addq.l	#4,d1			;PattenOffset + 4
	clr.l	d2
	cmp	#-3,(A6)		;Ist Note = 'PIC' ?
	beq	NoInstr2		;wenn ja -> ignorieren
	move.b	2(a6),d2		;Instr Nummer holen	
	and.b	#$f0,d2			;ausmaskieren
	lsr.b	#4,d2			;ins untere Nibble
	tst.b	d2			;kein Intrument ?
	beq.L	NoInstr2		;wenn ja -> �berspringen
	
	clr.l	d3
	lea.l	Instruments(pc),a1	;Instr. Tabelle
	move.l	d2,d4			;Instrument Nummer
	subq	#1,d2
	lsl	#2,d2			;Offset auf akt. Instr.
	mulu	#30,d4			;Offset Auf Instr.Daten
	move.l	(a1,d2.w),4(a6)		;Zeiger auf akt. Instr.
	move.w	(a3,d4.l),8(a6)		;Instr.L�nge
	move.w	2(a3,d4.l),18(a6)	;Volume
	move.w	4(a3,d4.l),d3		;Repeat
	tst	d3			;kein Repeat?
	beq.s	NoRepeat		;Nein!
					;Doch!
	
	move.l	4(a6),d2		;akt. Instr.
	add.l	d3,d2			;Repeat dazu
	move.l	d2,10(a6)		;Repeat Instr.
	move.w	6(a3,d4),14(a6)		;rep laenge
	move.w	18(a6),d3 		;Volume in HardReg.
	bra.s	NoInstr

NoRepeat:
	move.l	4(a6),d2		;Instrument	
	add.l	d3,d2			;rep Offset
	move.l	d2,10(a6)		;in Rep. Pos.
	move.w	6(a3,d4.l),14(a6)	;rep Laenge
	move.w	18(a6),d3 		;Volume in Hardware

CheckPic:
NoInstr:
	move.b	2(A6),d2
	and	#$0f,d2
	cmp.b	#5,d2
	beq.s	ChangeUpVolume
	cmp.b	#6,d2
	bne.L	SetVolume2
	moveq	#0,d2
	move.b	3(A6),d2
	sub	d2,d3		
	tst	d3
	bpl	SetVolume2	
	clr	d3
	bra.L	SetVolume2
ChangeUpVolume:
	moveq	#0,d2
	move.b	3(A6),d2
	add	d2,d3
	tst	d3
	cmp	#64,d3
	ble.L	SetVolume2
	move	#64,d3
SetVolume2:
	move	d3,8(A5)
	
NoInstr2:
	cmp	#-3,(A6)		;Ist Note = 'PIC' ?
	bne.s	NoPic		
	clr	2(A6)			;wenn ja -> Note auf 0 setzen
	bra.s	NoNote	
NoPic:
	tst	(A6)			;Note ?
	beq.s	NoNote			;wenn 0 -> nicht spielen
	
	clr	(a4)
	move.w	(a6),16(a6)		;eintragen
	move.w	20(a6),$dff096		;dma abschalten
	move.l	d7,-(SP)
	move	#300,d7			;genug f�r MC68030
Delay1:
	dbf	d7,Delay1		;delay
	move.l	(SP)+,d7
	cmp	#-2,(A6)		;Ist es 'STP'
	bne.s	NoStop			;Nein!
	clr	8(A5)
	bra	Super
NoStop:
	move.l	4(a6),0(a5)		;Intrument Adr.
	move.w	8(a6),4(a5)		;L�nge
	move.w	0(a6),6(a5)		;Period
Super:
	move.w	20(a6),d0		;DMA Bit
	or.w	d0,DmaCon		;einodern
NoNote:
	rts

;--------------------------------------------------------------------
ArpeTable:
	dc.l	Arpe1
	dc.l	Arpe2
	dc.l	Arpe3
	dc.l	Arpe2
	dc.l	Arpe1


ChannelData0:
	blk.l	5,0			;Daten f�r Note
	dc.w	1			;DMA - Bit
ChannelData1:	
	blk.l	5,0			;u.s.w
	dc.w	2
ChannelData2:	
	blk.l	5,0			;etc.
	dc.w	4
ChannelData3:	
	blk.l	5,0			;a.s.o
	dc.w	8
Instruments:
	blk.l	15,0			;Zeiger auf die 15 Instrumente
PosCounter:
	dc.l	0			;Offset ins Pattern
TrackPos:
	dc.l	0			;Position Counter
Timer:
	dc.w	0			;Z�hler 0-5
DmaCon:
	dc.w	0			;Zwischenspeicher f�r DmaCon
AnzPat:
	dc.w	1			;Anzahl Positions
PlayLock:
	dc.w	0			;Flag fuer 'Sound erlaubt'
DelayValue:
	dc.w	14565
SongPointer:
	dc.l	LenghtTable
ChipFlag:
	dc.w	0
CiaaBase:
	dc.l	0
InterruptName:
	dc.b	"Chris's SoundInterrupt",0
CiaaResource:
	dc.b	'ciaa.resource',0


even

Reserve:
	dc.w	856,856,856,856,856,856,856,856,856,856,856,856
NoteTable:
	dc.w	856,808,762,720,678,640,604,570,538,508,480,453   ;1.Okt
	dc.w	428,404,381,360,339,320,302,285,269,254,240,226   ;2.Okt
	dc.w	214,202,190,180,170,160,151,143,135,127,120,113	  ;3.Okt
	dc.w	113,113,113,113,113,113,113,113,113,113,113,113	  ;Reserve
	dc.w	-1

LenghtTable:

Datas:
	Songstr = lenghttable+60
	
	blk.b	Laenge,0		;hierhin die Daten!!!

EndSongStr:
