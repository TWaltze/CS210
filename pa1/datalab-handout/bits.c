/*
 * CS:APP Data Lab
 *
 * Tyler Waltze
 * U91283106
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
// Rating: 1
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  // OR combination of complements is opposite
  // an AND
  return ~(~x | ~y);
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  // Minimum two's complement is 0x80000000.
  // 8 is initially in 3rd spot (0-31), and it
  // needs to be in the 31st spot. So move it
  // over 28 spots to the left.
  return 8<<28;
}
// Rating: 2
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  // Convert n-bytes to bits by multiplying by 8.
  // Use shifts to multiply by a power of 2 (2^3 = 8).
  // Remove all bits to the left of relevant x byte.
  // 0xFF = 0x000000FF
  return (x >> (n << 3)) & 0xFF;
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int remaining;
  int removed;

  // Find the number of bits left after using n bits
  remaining = 33 + ~n;

  // Take whatever is allowed to be left and
  // move them off x's bit stack. Then move them back
  // on as zeros.
  removed = (x << remaining) >> remaining;

  // Check that removed is still the same number as x.
  // If not, then there were more significant bits of
  // x than were allowed (remaining).
  return !(removed ^ x);
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  // x >> n won't work with signed integers.
  // Must account for "extra" 1-bit when negative.
  int mask;
  int compensator;

  // Mask to subtract extra bit (for the extra 1)
  mask = (1 << n) + ~0;

  // Hold a right most 1 (negative x) or 0 (positive x).
  compensator = (x >> 31) & mask;

  // Add the right most 1 or 0 and then do normal
  // right shift division.
  return (x + compensator) >> n;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  // Do steps to produce two's completment (flip, plus 1)
  return ~x + 1;
}
// Rating: 3
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
  int mask;

  // Do an arithmetic right shift
  x = x >> n;

  // Build basic mask of 0 followed by 1's. If its
  // leftmost wasn't 0, we'd get a bunch of 1's instead
  // of 0's from the arithmetic right shift.
  mask = 0x7f;
  mask = mask << 8;
  mask = mask + 0xff;
  mask = mask << 8;
  mask = mask + 0xff;
  mask = mask << 8;
  mask = mask + 0xff;

  // Shift right by n to create a mask of all 0's
  // where the above arithmetic right shift may have
  // created 1's.
  mask = mask >> n;

  // Compensate
  mask = mask << 1;
  mask = mask + 1;

  // Apply mask to remove any trailing 1's.
  return x & mask;
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  int mask;
  int sign;

  // Create a mask of all 1 followed by 0's
  mask = 1 << 31;

  // Only care about x's left most bit:
  // 0 for positive, 1 for negative
  sign = x & mask;

  // Fanciness for the specialness that is 0.
  // Goal is to just make sure both negative numbers
  // and 0 have a 1 in their bits, while positive
  // numbers have all 0's.
  x = (x + ~1 + 1) & mask;

  // Check if they have any 1's then reverse value
  return !(x | sign);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int x_sign;
  int y_sign;
  int diff_sign;

  int combined;
  int override_byte;

  // Find signs of x and y
  x_sign = x >> 31;
  y_sign = y >> 31;

  // y - x, then find sign of difference
  diff_sign = ((y + (~x) + 1)) >> 31;

  // Help with overflow
  combined = x_sign ^ y_sign;
  override_byte = combined & x_sign;

  return !((diff_sign | combined) ^ override_byte);
}
// Rating: 4
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
  int complement;

  complement = ~x + 1;

  // If x != 0, then the left most bit of x or complement will be 1.
  // Shift both to the right most bit and mask everything else.
  // This will leave either complement or x looking like 0x01.
  // OR x and complement then XOR it to mimic the !.
  return (((x >> 31) & 0x01) | ((complement >> 31) & 0x01)) ^ 0x01;
}
