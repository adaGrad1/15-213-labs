/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
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
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


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

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

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
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
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
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  //DONE, 7 OPS
  return ~(~x & ~y) & ~(x & y);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  //DONE, 1 OP
  return 1 << 0x1F;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  return !((x)^(x+1)^(~0)) & !!(x^(~0));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  return !(~x & ((0xAA << 24) | (0xAA << 16) | (0xAA << 8) | (0xAA)));
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // first 4 bits are 0x30 and last 4 bits are <=9
  return !((x ^ 0x30) >> 4) & (((x & 0x0F) + ~0x09) >> 31);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  // DONE, uses ?? ops
  x = (!(x) << 31) >> 31;
  return (y & ~x) | (z & x);

}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int minus_y_dec = ~y;
  int msb_diff = ((x ^ y) >> 31) & 0x01; // if signs are different, we just look at signs to make sure we don't have a two's complement issue
  int msb_diff_toret = (x >> 31) & 0x01; // what to return if the signs are different -- just check if x is negative
  return (!msb_diff & (((x + minus_y_dec) >> 31) & 0x01)) | (msb_diff & msb_diff_toret) ; // if x-y-1 <0, then x-y <=0  so x <= y 
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // MSB is zero and ~x+1 has same MSB as ~x
  int not_wraparound = ((~x+1) ^ x) >> 31;
  return ((not_wraparound | (x >> 31)) ^ 0x01) & 0x01 ; // doesn't wrap around or MSB is nonzero
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // first we make the number positive, which does not affect the number of bits it would cost to represent it
  // we use a little trick -- if we bitshift 31 to the right, this creates a mask for negativity
  x = ((x >> 31) & (~x)) | (~(x >> 31) & x);
  // then, we do something like binary search. we bitshift right by 16 and see if it's zero. if it is, we know the cost is <=16. else, the cost is >16
  int to_add = 1;
  to_add = to_add + (!(!(x >> 16)) << 4);
  int mask = (!(x >> 16) << 31) >> 31; // iff x >> 16 is zero, this is a ones mask
  x = (mask & x) | (~mask & (x >> 16));

  to_add = to_add + (!(!(x >> 8)) << 3);
  mask = (!(x >> 8) << 31) >> 31;
  x = (mask & x) | (~mask & (x >> 8));

  to_add = to_add + (!(!(x >> 4)) << 2);
  mask = (!(x >> 4) << 31) >> 31;
  x = (mask & x) | (~mask & (x >> 4));

  to_add = to_add + (!(!(x >> 2)) << 1);
  mask = (!(x >> 2) << 31) >> 31;
  x = (mask & x) | (~mask & (x >> 2));

  to_add = to_add + !(!(x >> 1));
  mask = (!(x >> 1) << 31) >> 31;
  x = (mask & x) | (~mask & (x >> 1));

  to_add = to_add + (x & 0x01);

  return to_add;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned exp_mask = 0x7F800000;
  unsigned exp = uf & exp_mask;
  unsigned frac_mask = 0x007FFFFF;
  unsigned frac = uf & frac_mask;
  if(exp == 0){
    frac = frac << 1; //may overflow into exp bits slightly -- intended!
  }else if(exp != (0xFF << 23)){
    exp = exp + (1 << 23); //not NaN (aN?)
  }
  return (uf & 0x80000000) + exp + frac;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned exp_mask = 0x7F800000;
  unsigned frac_mask = 0x007FFFFF;
  //this is technically kind of wrong for unnormalized values, but it doesn't matter because it all goes to 0 regardless
  int exp_with_bias = ((uf & exp_mask) >> 23) - 127;
  if(exp_with_bias > 30){ // too big for int!
    return 0x80000000u;
  }
  if(exp_with_bias < 0){ // zero value -- we return here to avoid gigantic bitshifts with undefined behavior!
    return 0;
  }
  int to_bitshift = exp_with_bias - 23;
  int frac = frac_mask & uf;
  frac = frac | (1 << 23);
  int to_return;
  if(to_bitshift >= 0){
    to_return = frac << to_bitshift;
  } else{
    to_return = frac >> -to_bitshift;
  }
  if(uf & 0x80000000u){
    to_return = -to_return;
  }
  return to_return;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  // for x in (-126, 127) we just stick it in the exponent
  // for x >= 128, it's too big and we return inf
  // for x <= -127, we're in denorm territory
  if(x >= 128){
    return 0x7F800000u; // inf
  }
  if(x >= -126){
    return ((x + 127) << 23);
  }
  if(x >= -126-23){
    int to_shift = (126+23)-x;
    return 1 << to_shift;
  }
  return 0;
}
