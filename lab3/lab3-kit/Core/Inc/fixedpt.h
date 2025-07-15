#ifndef __FIXED_PT_H
#define __FIXED_PT_H

#include <stdint.h>

// Change these values to set the fixed point format.
#define FXD_TOTAL_BITS  32
#define FXD_FRAC_BITS		28

#if FXD_FRAC_BITS >= FXD_TOTAL_BITS
#error "FIXEDPT_WBITS must be less than or equal to FIXEDPT_BITS"
#endif

/* 
 * Create a new 'type' for fixed point.
 * WARNING: typedef in C does not do any type checking. If you try to do 
 * `fixed' + `int', the compiler will not throw an error. You will have to 
 * make sure you use the right types.
 */
 
#if FXD_TOTAL_BITS == 8
typedef char fixedpt;
typedef short int FXDMUL_T;
#elif FXD_TOTAL_BITS == 16
typedef short int fixedpt;
typedef int FXDMUL_T;
#elif FXD_TOTAL_BITS == 32
typedef int fixedpt;
typedef int64_t FXDMUL_T;
#endif

// Macros to convert from int <-> fixed and float <-> fixed.
#define FXD_FROM_FLOAT(T) ((fixedpt)((T)*((fixedpt)1 << FXD_FRAC_BITS)))
#define FXD_TO_FLOAT(T) ((float)((T)*((float)(1)/(float)(1 << FXD_FRAC_BITS))))
#define FXD_FROM_INT(I) ((fixedpt)(I) << FXD_FRAC_BITS)
#define FXD_TO_INT(F) ((F) >> FXD_FRAC_BITS)

// Macros for basic arithmetic operations. 
#define FXD_ADD(A,B) ((A)+(B))
#define FXD_SUB(A,B) ((A)-(B))
#define FXD_MUL(A,B) ((fixedpt)(((FXDMUL_T)(A)*(B)) >> FXD_FRAC_BITS))
#define FXD_DIV(A,B) (((A)/(B)) << FXD_FRAC_BITS)

// Other useful macros such as getting just the fractional part. 
#define FXD_FRAC_MASK	(((fixedpt)1 << FXD_FRAC_BITS) - 1)
#define FXD_FRAC_PART(A) ((fixedpt)(A) & FXD_FRAC_MASK)
#define FIXEDPT_ONE	((fixedpt)((fixedpt)1 << FXD_FRAC_BITS))
#define FIXEDPT_CONST(R) ((fixedpt)((R) * FIXEDPT_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define FIXEDPT_PI	FIXEDPT_CONST(3.14159265358979323846)

#endif
