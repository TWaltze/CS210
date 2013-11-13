;; Copyright (C) 2011 by Jonathan Appavoo, Boston University
;;
;; Permission is hereby granted, free of charge, to any person obtaining a copy
;; of this software and associated documentation files (the "Software"), to deal
;; in the Software without restriction, including without limitation the rights
;; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;; copies of the Software, and to permit persons to whom the Software is
;; furnished to do so, subject to the following conditions:
;;
;; The above copyright notice and this permission notice shall be included in
;; all copies or substantial portions of the Software.
;; 
;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
;; THE SOFTWARE.

.alias MEMSTART $0000
.alias ZPG      $0000
.alias ZPG_SIZE $0100
.alias STACK    $0100
.alias STACK_SIZE $0100
.alias TASK_STACK_SIZE   64

.alias TASK_0_STACK_START $FF
.alias TASK_0_STACK_START $BF
.alias TASK_0_STACK_START $7F
.alias TASK_0_STACK_START $3F
	
.alias CONTEXT_MEM_START $0200
.alias CONTEXT_MEM_END   $0300
	
;; struct Context_Desc {
;;   byte ac;
;;   byte y;
;;   byte x;
;;   address pc;
;;   byte sp;
;;   byte sr;
;;   byte pad
;; } *CONTEXTS = (struct Context_Desc *)CONTEXT_MEM_START
	
.alias CONTEXT_OFFSET_AC    0
.alias CONTEXT_OFFSET_X     1
.alias CONTEXT_OFFSET_Y     2
.alias CONTEXT_OFFSET_PC    3
.alias CONTEXT_OFFSET_SP    5
.alias CONTEXT_OFFSET_SR    6
.alias CONTEXT_OFFSET_VALID 7
	
.alias SIZE_OF_CONTEXT    8	; sizeof(stuct Context)
.alias MAX_CONTEXTS       4     ; the STACK_SIZE/TASK_STACK_SIZE
.alias CURRENTCONTEXT     $FF   ; variable representing index of current context is located
                        	; on last byte off the zero page

	
.alias NMIHDLR  $FC00
.alias IRQHDLR  $FD00
.alias DEVMEM   $FE00
.alias CONMEM   $FF00
.alias MEMEND   $FFFF
	
	;; MEMORY MAP
	;; 0000: HW DEFINED ZERO PAGE
	;;      ... 
	;; 0100: HW DEFINED STACK PAGE
	;;      ... 
	;; 0200: CONTEXT[0] CONTEXT DESCRIPTOR MEMORY START
        ;; 0208: CONTEXT[1]
	;; 0210: CONTEXT[2]
	;; 0218: CONTEXT[3]
	;; 0220: UNUSED
	;;      ... 
	;; 0300: START OF RESET MEMORY
	;;      ...
	;; 0400: START OF TASK MEMORY
	;;      ...
	;; FBFF: LAST BYTE OF TASK MEMORY
	;; FC00: NMIHDLR MEMORY START
	;;      ...
	;; FD00: INTHDLR MEMORY START
	;;      ...
	;; FE00: MEMORY MAPPED TIMER HARDWARE MEMORY
	;; FF00: MEMORY MAPPED CONSOLE I/O REGISTER
	;; FF01: UNUSED
	;;      ...
	;; FFFA: HW DEFINED NMI LOW  VECTOR ADDRESS
	;; FFFB: HW DEFINED NMI HIGH VECTOR ADDRESS
	;; FFFC: HW DEFINED RESET LOW  VECTOR ADDRESS
	;; FFFD: HW DEFINED RESET HIGH VECTOR ADDRESS
	;; FFFE: HW DEFINED IRQ/BRK LOW  VECTOR ADDRESS
	;; FFFF: HW DEFINED IRQ/BRK HIGH VECTOR ADDRESS


.org MEMSTART
	.advance ZPG + CURRENT_CONTEXT_IDX
	.byte    00             ; initialize current context to be the zeroth 
	
	.advance CONTEXT_0
	.byte   00		;ac
	.byte   00   		;x
	.byte   00              ;y
	.word   $0000		;pc
	.byte   TASK_0_STACK_START   ;sp
	.byte   00              ;sr
	.byte   00              ;valid
	
	.advance CONTEXT_1
	.byte   00		;ac
	.byte   00   		;x
	.byte   00              ;y
	.word   $0000		;pc
	.byte   TASK_1_STACK_START   ;sp
	.byte   00              ;sr
	.byte   00              ;valid

	.advance CONTEXT_2
	.byte   00		;ac
	.byte   00   		;x
	.byte   00              ;y
	.word   $0000		;pc
	.byte   TASK_2_STACK_START   ;sp
	.byte   00              ;sr
	.byte   00              ;valid

	.advance CONTEXT_3
	.byte   00		;ac
	.byte   00   		;x
	.byte   00              ;y
	.word   $0000		;pc
	.byte   TASK_3_STACK_START   ;sp
	.byte   00              ;sr
	.byte   00              ;valid
	
.advance RESET_MEM_START
        ; standard "house keeping code" as per description in NMOS programmers manual
INIT:   LDX #$FF   ; setup stack to bottom stack page ($01FF)
        TXS        ; load stack register
                   ; initialize devices here if they require it
        CLI        ; turn on interrupts 
        CLD        ; clear decimal mode flag
START:	JMP START  ; HANG OUT HERE UNTIL FIRST TIMER INTERRUPT
	
        ; TASK CODE AND DATA COMES HERE
	.advance TASK_MEMORY_START
PRINT:
	SEI			; avoid switching during prints (mutual exclusion)
	STA   $00
	STX   $01
	PUSHY
	LDY   #0
PRINT_NEXT_CHAR:	
	LDA  ($00),Y
	BEQ  PRINT_END		
	STA  CONMEM
	INY
	JMP PRINT_NEXT_CHAR
PRINT_END:
	LDA  $00
	PULLY
	CLI                     ; renable switching
	RTS
	
TASK_0_DATA:
TASK_0_GREET:	.byte "TASK 0: HELLO\n" .byte  0
TASK_0_STR0:	.byte "0\n", .byte 0
TASK_0_CODE_START:
	LDA   TASK_0_GREET
	LDX   TASK_0_GREET+1
	JSR   PRINT
	LDA   TASK_0_STR0
	LDX   TASK_0_STR0+1
	LDY   #10
	JSR   PRINT
	DEY
	BNE   TASK_0_LOOP
	JMP   TASK_0_CODE_START

TASK_1_DATA:
TASK_1_ID:	.byte '1   	;ASCII 0
TASK_1_CODE:
	LDA   TASK_1_ID
	JSR   PRINT
	JMP   TASK_1_CODE


TASK_2_DATA:
TASK_2_ID:	.byte '2   	;ASCII 0
TASK_2_CODE:
	LDA   TASK_2_ID
	JSR   PRINT
	JMP   TASK_2_CODE

TASK_3_DATA:
TASK_3_ID:	.byte '3   	;ASCII 0
TASK_3_CODE:
	LDA   TASK_3_ID
	JSR   PRINT
	JMP   TASK_3_CODE


	
START:  LDX   CONMEM    ; get input from console
	
	;;  get input from 6532
        LDY   DEVMEM+1  

	;; print what was received
        LDA   #$3C      	; '<'
        STA   CONMEM
        LDA   #$3A		; ':'
        STA   CONMEM
        STY   CONMEM		;store the actual value
        LDA   #$0A		; '\n'
	STA   CONMEM

	CPX   #$0A		; check if input was a return '\n'
	BEQ   START
	
	;; send what we have to send
	STX   DEVMEM

	;; print what we sent 
        LDA   #$3E		; '>'
        STA   CONMEM
	LDA   #$3A		; ':'
        STA   CONMEM
        STX   CONMEM		;store the actual value
        LDA   #$0a		; '\n'
	STA   CONMEM
	
        JMP   START
DONE:	BRK

.advance NMIHDLR
NMISR:  PHA
        LDA  #$4E   ; ascii N
        STA  CONMEM
        PLA
        BRK
        RTI

.advance IRQHDLR
IRQSR:  PHA
        LDA  #$49   ; ascii I
        STA  CONMEM
        PLA
        RTI

.advance MEMEND
.byte $00
