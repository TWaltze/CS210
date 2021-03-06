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
.alias STACK    $0100
.alias CODE     $0200
.alias NMIHDLR  $0500
.alias IRQHDLR  $0600
.alias DEVMEM   $E000
.alias CONMEM   $FF00
.alias MEMEND   $FFFF

.org MEMSTART

.advance CODE
        ; standard "house keeping code" as per description in NMOS programmers manual
INIT:   LDX #$FF   ; setup stack to bottom stack page ($01FF)
        TXS        ; load stack register
                   ; initialize devices here if they require it
        CLI        ; turn on interrupts 
        CLD        ; clear decimal mode flag
        ; Application code comes next
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
