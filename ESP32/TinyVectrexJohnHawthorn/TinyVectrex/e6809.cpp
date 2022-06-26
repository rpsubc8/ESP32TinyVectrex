#include "gbConfig.h"
#include <stdio.h>
#include "e6809.h"
#ifdef use_lib_optimice_readwrite8
 #include "vecx.h"
#else
#endif

#include "gbGlobals.h"

// code assumptions:
//  - it is assumed that an 'int' is at least 16 bits long.
//  - a 16-bit register has valid bits only in the lower 16 bits and an
//    8-bit register has valid bits only in the lower 8 bits. the upper
//    may contain garbage!
//  - all reading functions are assumed to return the requested data in
//    the lower bits with the unused upper bits all set to zero.
 

//#define einline __inline

//JJ enum {
//JJ 	FLAG_E		= 0x80,
//JJ 	FLAG_F		= 0x40,
//JJ 	FLAG_H		= 0x20,
//JJ 	FLAG_I		= 0x10,
//JJ 	FLAG_N		= 0x08,
//JJ 	FLAG_Z		= 0x04,
//JJ 	FLAG_V		= 0x02,
//JJ 	FLAG_C		= 0x01,
//JJ 	IRQ_NORMAL	= 0,
//JJ 	IRQ_SYNC	= 1,
//JJ 	IRQ_CWAI	= 2
//JJ };
#define FLAG_E 0x80
#define FLAG_F 0x40
#define FLAG_H 0x20
#define	FLAG_I 0x10
#define	FLAG_N 0x08
#define	FLAG_Z 0x04
#define	FLAG_V 0x02
#define	FLAG_C 0x01
#define	IRQ_NORMAL 0
#define	IRQ_SYNC 1
#define	IRQ_CWAI 2

///index registers
#ifdef use_lib_reg_16bits
 static unsigned short int reg_x;
 static unsigned short int reg_y;
 //user stack pointer
 static unsigned short int reg_u;
 //hardware stack pointer
 static unsigned short int reg_s; 
#else
 static unsigned reg_x;
 static unsigned reg_y;
 //user stack pointer
 static unsigned reg_u;
 //hardware stack pointer
 static unsigned reg_s;
#endif 

//program counter

#ifdef use_lib_regtiny
 static unsigned short int reg_pc;
#else
 static unsigned reg_pc;
#endif 

// accumulators

//JJ static unsigned reg_a;
//JJ static unsigned reg_b;
static unsigned short int reg_a;
static unsigned short int reg_b;

// direct page register
#ifdef use_lib_reg_dp_8bits
 static unsigned short int reg_dp;
#else
 static unsigned reg_dp;
#endif 

//condition codes

#ifdef use_lib_reg_cc_8bits 
 static unsigned char reg_cc;//8 bits
#else
 static unsigned reg_cc;//32 bits
#endif 

//flag to see if interrupts should be handled (sync/cwai).

static unsigned irq_status;

#ifdef use_lib_reg_16bits
 static unsigned short int *rptr_xyus[4] = {
  &reg_x,
  &reg_y,
  &reg_u,
  &reg_s
 };
#else
 static unsigned *rptr_xyus[4] = {
	&reg_x,
	&reg_y,
	&reg_u,
	&reg_s
} ;
#endif

//user defined read and write functions
#ifdef use_lib_optimice_readwrite8
#else
 unsigned char (*e6809_read8) (unsigned address);
 void (*e6809_write8) (unsigned address, unsigned char data);
#endif 

/* obtain a particular condition code. returns 0 or 1. */

//static einline unsigned get_cc (unsigned flag)
//JJ static inline unsigned get_cc (unsigned flag) //8 bits
#ifdef use_lib_reg_cc_8bits
 static inline unsigned char get_cc (unsigned char flag)
#else
 static inline unsigned get_cc (unsigned flag)
#endif 
{//2499 veces en un frame
 return (reg_cc / flag) & 1;
}

// set a particular condition code to either 0 or 1.
// value parameter must be either 0 or 1.

//JJ static einline void set_cc (unsigned flag, unsigned value)
#ifdef use_lib_reg_cc_8bits
 static inline void set_cc (unsigned char flag, unsigned char value)
#else
 static inline void set_cc (unsigned flag, unsigned value)
#endif 
{//15718 veces en un frame
 reg_cc &= ~flag;
 reg_cc |= value * flag;
}

// test carry

//JJ static einline unsigned test_c (unsigned i0, unsigned i1,
//JJ 								unsigned r, unsigned sub)
static inline unsigned char test_c (unsigned char i0, unsigned char i1,
 							        unsigned char r, unsigned char sub)
{//336 veces en un frame
	//JJ unsigned flag;
	unsigned char flag;

	flag  = (i0 | i1) & ~r; //one of the inputs is 1 and output is 0
	flag |= (i0 & i1);      //both inputs are 1
	flag  = (flag >> 7) & 1;
	flag ^= sub; //on a sub, carry is opposite the carry of an add

	return flag;
}

//test negative

//JJ static einline unsigned test_n (unsigned r)
static inline unsigned char test_n (unsigned char r)
{//4989 veces en un frame
	return (r >> 7) & 1;
}

//test for zero in lower 8 bits

//JJstatic einline unsigned test_z8 (unsigned r)
//static inline unsigned char test_z8 (unsigned char r)
#ifdef use_lib_wifi
 static unsigned char test_z8 (unsigned char r)
#else
 static unsigned char IRAM_ATTR test_z8 (unsigned char r)
#endif
 {//4711 veces 1 frame
	//JJ unsigned flag;
	unsigned char flag;

	flag = ~r;
	flag = (flag >> 4) & (flag & 0xf);
	flag = (flag >> 2) & (flag & 0x3);
	flag = (flag >> 1) & (flag & 0x1);

	return flag;
 }

// test for zero in lower 16 bits

//JJ static einline unsigned test_z16 (unsigned r)
#ifdef use_lib_optimice_test_z16
 //static inline unsigned char test_z16 (unsigned short int r)
 #ifdef use_lib_wifi
  static unsigned char test_z16 (unsigned short int r)
 #else
  static unsigned char IRAM_ATTR test_z16 (unsigned short int r)
 #endif 
#else
 static inline unsigned char test_z16 (unsigned r)
#endif
{//317 veces 1 frame
	//JJ unsigned flag;
	unsigned short int flag;

	flag = ~r;
	flag = (flag >> 8) & (flag & 0xff);
	flag = (flag >> 4) & (flag & 0xf);
	flag = (flag >> 2) & (flag & 0x3);
	flag = (flag >> 1) & (flag & 0x1);

	//JJ return flag;
	return flag;
}

//overflow is set whenever the sign bits of the inputs are the same
//but the sign bit of the result is not same as the sign bits of the
//inputs. 
#ifdef use_lib_optimice_test_v
 //static inline unsigned char test_v (unsigned char i0, unsigned char i1, unsigned char r)
 #ifdef use_lib_wifi
  static unsigned char test_v (unsigned char i0, unsigned char i1, unsigned char r)
 #else
  static unsigned char IRAM_ATTR test_v (unsigned char i0, unsigned char i1, unsigned char r)
 #endif 
#else
 static inline unsigned test_v (unsigned i0, unsigned i1, unsigned r)
#endif 
{//800 o 1000 veces 1 frame
   #ifdef use_lib_optimice_test_v
    unsigned char flag;
   #else	
	unsigned flag;
   #endif	

	flag  = ~(i0 ^ i1); /* input sign bits are the same */
	flag &=  (i0 ^ r);  /* input sign and output sign not same */
	flag  = (flag >> 7) & 1;

	return flag;
}


//JJ static einline unsigned get_reg_d (void)
static inline unsigned short int get_reg_d (void)
{//190 veces 1 frame
	return (reg_a << 8) | (reg_b & 0xff);
}

//JJ static einline void set_reg_d (unsigned value)
static inline void set_reg_d (unsigned short int value)
{
	reg_a = value >> 8;
	reg_b = value;
}

/* read a byte ... the returned value has the lower 8-bits set to the byte
 * while the upper bits are all zero.
 */
#ifdef use_lib_optimice_readwrite8
#else
 static inline unsigned read8 (unsigned address)
 {
  return (*e6809_read8) (address & 0xffff);
 }
#endif 

/* write a byte ... only the lower 8-bits of the unsigned data
 * is written. the upper bits are ignored.
 */

#ifdef use_lib_optimice_readwrite8
#else
 static inline void write8 (unsigned address, unsigned data)
 {
  (*e6809_write8) (address & 0xffff, (unsigned char) data);
 }
#endif 

#ifdef use_lib_optimice_readwrite16
 static inline unsigned short int read16 (unsigned short int address)
 {//638 veces 1 frame
  //unsigned short int datahi;
  //datahi = read8 (address);
  //return ((datahi << 8) | (read8(address + 1)));
  return ((read8 (address)<<8) | (read8(address + 1)));
 }
#else
 static inline unsigned read16 (unsigned address)
 {
	unsigned datahi, datalo;

	datahi = read8 (address);
	datalo = read8 (address + 1);

	return (datahi << 8) | datalo;
 }
#endif

#ifdef use_lib_optimice_readwrite16
 static inline void write16 (unsigned short int address, unsigned short int data)
#else
 static inline void write16 (unsigned address, unsigned data)
#endif 
 {//12 veces 1 frame
	write8 (address, data >> 8);
	write8 (address + 1, data);
 }

//JJ static inline void push8 (unsigned *sp, unsigned data)
#ifdef use_lib_reg_16bits
 //push8 (sp, data);
 static inline void push8 (unsigned short int *sp, unsigned char data)
#else
 static inline void push8 (unsigned *sp, unsigned char data)
#endif 
{//94 veces 1 frame
	(*sp)--;
	write8 (*sp, data);
}

#ifdef use_lib_reg_16bits
 //reg_cc = pull8 (sp);
 static inline unsigned char pull8 (unsigned short int *sp)
#else
 static inline unsigned pull8 (unsigned *sp)
#endif
{//94 veces 1 frame
	//JJ unsigned data;
	unsigned char data;

	data = read8 (*sp);
	(*sp)++;

	return data;
}

//JJ static einline void push16 (unsigned *sp, unsigned data)
#ifdef use_lib_reg_16bits
 //push16 (sp, reg_pc);
 static inline void push16 (unsigned short int *sp, unsigned short int data)
#else 
 static inline void push16 (unsigned *sp, unsigned short int data)
#endif 
{//132 veces 1 frame
	push8 (sp, data);
	push8 (sp, data >> 8);
}

//JJ static einline unsigned pull16 (unsigned *sp)
#ifdef use_lib_reg_16bits
 static inline unsigned short int pull16 (unsigned short int *sp)
#else
 static inline unsigned short int pull16 (unsigned *sp)
#endif 
{//132 veces 1 frame
	//JJ unsigned datahi, datalo;	

	//datahi = pull8 (sp);	
	//datalo = pull8 (sp);    	
	//return (datahi << 8) | datalo;
	
	return (((unsigned short int)(pull8(sp)) <<8) | pull8 (sp));
}

//read a byte from the address pointed to by the pc
#ifdef use_lib_optimice_pc_read8
 //static inline unsigned char pc_read8 (void)
 //static unsigned char pc_read8 (void)
 #ifdef use_lib_wifi
  static unsigned char pc_read8(void)
 #else
  static unsigned char IRAM_ATTR pc_read8(void)
 #endif 
 {//15311 veces 1 frame
  //unsigned char data;
  //data = read8(reg_pc);
  //reg_pc++;
  //return data;
//  return (read8(reg_pc++));

  if ((reg_pc & 0xe000) == 0xe000)
  {//rom
   return rom[reg_pc++ & 0x1fff];
  }
  if (reg_pc < 0x8000)
  {//cart
   return (cart[reg_pc++]);
  }
  //return 0xFF;
  return ( read8(reg_pc++));
 }
#else
 static inline unsigned pc_read8 (void)
 {
	unsigned data;

	data = read8 (reg_pc);
	reg_pc++;

	return data;
 }
#endif 

//read a word from the address pointed to by the pc
#ifdef use_lib_optimice_pc_read16
 //static inline unsigned short int pc_read16 (void)
 #ifdef use_lib_wifi
  static unsigned short int pc_read16 (void)
 #else
  static unsigned short int IRAM_ATTR pc_read16 (void)
 #endif 
 {//548 veces 1 frame
//  unsigned short int data;
//  data = read16(reg_pc);
//  reg_pc += 2;
//  return data;

  unsigned short int data;
  if ((reg_pc & 0xe000) == 0xe000)
  {//rom
   data= rom[reg_pc++ & 0x1fff];
   data= (data<<8) | rom[reg_pc++ & 0x1fff];
   return data;
  }
  if (reg_pc < 0x8000)
  {//cart
   data= (cart[reg_pc++]);
   data= (data<<8)|(cart[reg_pc++]);
   return data;
  }
  //return 0xFFFF;
  data = read16 (reg_pc);
  reg_pc += 2;
  return data;  
  
 }
#else
 static inline unsigned pc_read16 (void)
 {
	unsigned data;

	data = read16 (reg_pc);
	reg_pc += 2;

	return data;
 }
#endif 

//sign extend an 8-bit quantity into a 16-bit quantity */

//JJ static einline unsigned sign_extend (unsigned data)
#ifdef use_lib_optimice_sign_extend
 static inline unsigned short int sign_extend (unsigned char data)
#else
 static inline unsigned short int sign_extend (unsigned data)
#endif 
{//2916 veces 1 frame
	return (~(data & 0x80) + 1) | (data & 0xff); 
}

//direct addressing, upper byte of the address comes from
//the direct page register, and the lower byte comes from the
//instruction itself.
#ifdef use_lib_optimice_ea_direct
 static inline unsigned short int ea_direct (void)
#else
 static inline unsigned ea_direct (void)
#endif 
{//2804 veces 1 frame
	return (reg_dp << 8) | pc_read8 ();
}

//extended addressing, address is obtained from 2 bytes following
//the instruction.
#ifdef use_lib_optimice_ea_extended 
 #define ea_extended() pc_read16()
#else
 static inline unsigned ea_extended (void)
 {//372 veces
  return pc_read16 ();
 }
#endif  

//indexed addressing
#ifdef use_lib_optimice_ea_indexed
 static inline unsigned short int ea_indexed (unsigned *cycles)
#else
 static inline unsigned ea_indexed (unsigned *cycles)
#endif 
{//681 veces
   #ifdef use_lib_optimice_ea_indexed
    unsigned char r, op;
    unsigned short int ea;
   #else
	unsigned r, op, ea;
   #endif	

	/* post byte */

	op = pc_read8 ();

	r = (op >> 5) & 3;

	switch (op) {
	case 0x00: case 0x01: case 0x02: case 0x03:
	case 0x04: case 0x05: case 0x06: case 0x07:
	case 0x08: case 0x09: case 0x0a: case 0x0b:
	case 0x0c: case 0x0d: case 0x0e: case 0x0f:
	case 0x20: case 0x21: case 0x22: case 0x23:
	case 0x24: case 0x25: case 0x26: case 0x27:
	case 0x28: case 0x29: case 0x2a: case 0x2b:
	case 0x2c: case 0x2d: case 0x2e: case 0x2f:
	case 0x40: case 0x41: case 0x42: case 0x43:
	case 0x44: case 0x45: case 0x46: case 0x47:
	case 0x48: case 0x49: case 0x4a: case 0x4b:
	case 0x4c: case 0x4d: case 0x4e: case 0x4f:
	case 0x60: case 0x61: case 0x62: case 0x63:
	case 0x64: case 0x65: case 0x66: case 0x67:
	case 0x68: case 0x69: case 0x6a: case 0x6b:
	case 0x6c: case 0x6d: case 0x6e: case 0x6f:
		/* R, +[0, 15] */

		ea = *rptr_xyus[r] + (op & 0xf);
		(*cycles)++;
		break;
	case 0x10: case 0x11: case 0x12: case 0x13:
	case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x18: case 0x19: case 0x1a: case 0x1b:
	case 0x1c: case 0x1d: case 0x1e: case 0x1f:
	case 0x30: case 0x31: case 0x32: case 0x33:
	case 0x34: case 0x35: case 0x36: case 0x37:
	case 0x38: case 0x39: case 0x3a: case 0x3b:
	case 0x3c: case 0x3d: case 0x3e: case 0x3f:
	case 0x50: case 0x51: case 0x52: case 0x53:
	case 0x54: case 0x55: case 0x56: case 0x57:
	case 0x58: case 0x59: case 0x5a: case 0x5b:
	case 0x5c: case 0x5d: case 0x5e: case 0x5f:
	case 0x70: case 0x71: case 0x72: case 0x73:
	case 0x74: case 0x75: case 0x76: case 0x77:
	case 0x78: case 0x79: case 0x7a: case 0x7b:
	case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		/* R, +[-16, -1] */

		ea = *rptr_xyus[r] + (op & 0xf) - 0x10;
		(*cycles)++;
		break;
	case 0x80: case 0x81:
	case 0xa0: case 0xa1:
	case 0xc0: case 0xc1:
	case 0xe0: case 0xe1:
		/* ,R+ / ,R++ */

		ea = *rptr_xyus[r];
		*rptr_xyus[r] += 1 + (op & 1);
		*cycles += 2 + (op & 1);
		break;
	case 0x90: case 0x91:
	case 0xb0: case 0xb1:
	case 0xd0: case 0xd1:
	case 0xf0: case 0xf1:
		/* [,R+] ??? / [,R++] */

		ea = read16 (*rptr_xyus[r]);
		*rptr_xyus[r] += 1 + (op & 1);
		*cycles += 5 + (op & 1);
		break;
	case 0x82: case 0x83:
	case 0xa2: case 0xa3:
	case 0xc2: case 0xc3:
	case 0xe2: case 0xe3:

		/* ,-R / ,--R */

		*rptr_xyus[r] -= 1 + (op & 1);
		ea = *rptr_xyus[r];
		*cycles += 2 + (op & 1);
		break;
	case 0x92: case 0x93:
	case 0xb2: case 0xb3:
	case 0xd2: case 0xd3:
	case 0xf2: case 0xf3:
		/* [,-R] ??? / [,--R] */

		*rptr_xyus[r] -= 1 + (op & 1);
		ea = read16 (*rptr_xyus[r]);
		*cycles += 5 + (op & 1);
		break;
	case 0x84: case 0xa4:
	case 0xc4: case 0xe4:
		/* ,R */

		ea = *rptr_xyus[r];
		break;
	case 0x94: case 0xb4:
	case 0xd4: case 0xf4:
		/* [,R] */

		ea = read16 (*rptr_xyus[r]);
		*cycles += 3;
		break;
	case 0x85: case 0xa5:
	case 0xc5: case 0xe5:
		/* B,R */

		ea = *rptr_xyus[r] + sign_extend (reg_b);
		*cycles += 1;
		break;
	case 0x95: case 0xb5:
	case 0xd5: case 0xf5:
		/* [B,R] */

		ea = read16 (*rptr_xyus[r] + sign_extend (reg_b));
		*cycles += 4;
		break;
	case 0x86: case 0xa6:
	case 0xc6: case 0xe6:
		/* A,R */

		ea = *rptr_xyus[r] + sign_extend (reg_a);
		*cycles += 1;
		break;
	case 0x96: case 0xb6:
	case 0xd6: case 0xf6:
		/* [A,R] */

		ea = read16 (*rptr_xyus[r] + sign_extend (reg_a));
		*cycles += 4;
		break;
	case 0x88: case 0xa8:
	case 0xc8: case 0xe8:
		/* byte,R */

		ea = *rptr_xyus[r] + sign_extend (pc_read8 ());
		*cycles += 1;
		break;
	case 0x98: case 0xb8:
	case 0xd8: case 0xf8:
		/* [byte,R] */

		ea = read16 (*rptr_xyus[r] + sign_extend (pc_read8 ()));
		*cycles += 4;
		break;
	case 0x89: case 0xa9:
	case 0xc9: case 0xe9:
		/* word,R */

		ea = *rptr_xyus[r] + pc_read16 ();
		*cycles += 4;
		break;
	case 0x99: case 0xb9:
	case 0xd9: case 0xf9:
		/* [word,R] */

		ea = read16 (*rptr_xyus[r] + pc_read16 ());
		*cycles += 7;
		break;
	case 0x8b: case 0xab:
	case 0xcb: case 0xeb:
		/* D,R */

		ea = *rptr_xyus[r] + get_reg_d ();
		*cycles += 4;
		break;
	case 0x9b: case 0xbb:
	case 0xdb: case 0xfb:
		/* [D,R] */

		ea = read16 (*rptr_xyus[r] + get_reg_d ());
		*cycles += 7;
		break;
	case 0x8c: case 0xac:
	case 0xcc: case 0xec:
		/* byte, PC */

		r = sign_extend (pc_read8 ());
		ea = reg_pc + r;
		*cycles += 1;
		break;
	case 0x9c: case 0xbc:
	case 0xdc: case 0xfc:
		/* [byte, PC] */

		r = sign_extend (pc_read8 ());
		ea = read16 (reg_pc + r);
		*cycles += 4;
		break;
	case 0x8d: case 0xad:
	case 0xcd: case 0xed:
		/* word, PC */

		r = pc_read16 ();
		ea = reg_pc + r;
		*cycles += 5;
		break;
	case 0x9d: case 0xbd:
	case 0xdd: case 0xfd:
		/* [word, PC] */

		r = pc_read16 ();
		ea = read16 (reg_pc + r);
		*cycles += 8;
		break;
	case 0x9f:
		/* [address] */

		ea = read16 (pc_read16 ());
		*cycles += 5;
		break;
	default:
		//printf ("undefined post-byte\n");
		break;
	}

	return ea;
}

//instruction: neg
//essentially (0 - data).
#ifdef use_lib_optimice_inst_neg
 inline unsigned char inst_neg (unsigned char data)
#else
 inline unsigned inst_neg (unsigned data)
#endif 
{//47 veces 1 frame
   #ifdef use_lib_optimice_inst_neg
    unsigned char i0, i1, r;
   #else 
	unsigned i0, i1, r;
   #endif

	i0 = 0;
	i1 = ~data;
	r = i0 + i1 + 1;

	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 1));

	return r;
}

//instruction: com
#ifdef use_lib_optimice_inst_com
 inline unsigned char inst_com (unsigned char data)
#else
 inline unsigned inst_com (unsigned data)
#endif
{//3, 5 o 7 veces 1 frame
   #ifdef use_lib_optimice_inst_com
    unsigned char r;
   #else
	unsigned r;
   #endif	

	r = ~data;
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, 0);
	set_cc (FLAG_C, 1);

	return r;
}

//instruction: lsr
//cannot be faked as an add or substract. 
#ifdef use_lib_optimice_inst_lsr
 inline unsigned char inst_lsr (unsigned char data)
#else
 inline unsigned inst_lsr (unsigned data)
#endif 
{ //7 u 8 veces por frame
   #ifdef use_lib_optimice_inst_lsr
	unsigned char r;
   #else
	unsigned r;
   #endif	

	r = (data >> 1) & 0x7f;
	
	set_cc (FLAG_N, 0);
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_C, data & 1);

	return r;
}

//instruction: ror
//cannot be faked as an add or substract. 
#ifdef use_lib_optimice_inst_ror
 inline unsigned char inst_ror (unsigned char data)
#else
 inline unsigned inst_ror (unsigned data)
#endif 
{//0 o 2 por frame
   #ifdef use_lib_optimice_inst_ror
    unsigned char r, c;
   #else
	unsigned r, c;
   #endif	

	c = get_cc (FLAG_C);
	r = ((data >> 1) & 0x7f) | (c << 7);
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_C, data & 1);

	return r;
}

// instruction: asr
// cannot be faked as an add or substract. 
#ifdef use_lib_optimice_inst_asr
 inline unsigned char inst_asr (unsigned char data)
#else
 inline unsigned inst_asr (unsigned data)
#endif 
{//0 o 1 por frame
   #ifdef use_lib_optimice_inst_asr
    unsigned char r;
   #else
	unsigned r;
   #endif	

	r = ((data >> 1) & 0x7f) | (data & 0x80);
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_C, data & 1);

	return r;
}

//instruction: asl
//essentially (data + data). simple addition. 
#ifdef use_lib_optimice_inst_asl
 inline unsigned char inst_asl (unsigned char data)
#else
 inline unsigned inst_asl (unsigned data)
#endif 
{//23 por frame
   #ifdef use_lib_optimice_inst_asl
    unsigned char i0, i1, r;
   #else	
	unsigned i0, i1, r;
   #endif	

	i0 = data;
	i1 = data;
	r = i0 + i1;
	
	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 0));

	return r;
}

//instruction: rol
//essentially (data + data + carry). addition with carry. 
#ifdef use_lib_optimice_inst_rol
 inline unsigned char inst_rol (unsigned char data)
#else
 inline unsigned inst_rol (unsigned data)
#endif
{//0 a 20 frame
   #ifdef use_lib_optimice_inst_rol
    unsigned char i0, i1, c, r;
   #else
	unsigned i0, i1, c, r;
   #endif	

	i0 = data;
	i1 = data;
	c = get_cc (FLAG_C);
	r = i0 + i1 + c;
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 0));

	return r;
}

//instruction: dec
// essentially (data - 1).
#ifdef use_lib_optimice_inst_dec
 inline unsigned char inst_dec (unsigned char data)
#else
 inline unsigned inst_dec (unsigned data)
#endif 
{//438 por frame
   #ifdef use_lib_optimice_inst_dec
    unsigned char i0, i1, r;
   #else
	unsigned i0, i1, r;
   #endif	

	i0 = data;
	i1 = 0xff;
	r = i0 + i1;
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));

	return r;
}

//instruction: inc
//essentially (data + 1). 
#ifdef use_lib_optimice_inst_inc
 inline unsigned char inst_inc (unsigned char data)
#else
 inline unsigned inst_inc (unsigned data)
#endif 
{//235 por frame
   #ifdef use_lib_optimice_inst_inc
    unsigned char i0, i1, r;
   #else
	unsigned i0, i1, r;
   #endif	

	i0 = data;
	i1 = 1;
	r = i0 + i1;
	
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));

	return r;
}

//instruction: tst
#ifdef use_lib_optimice_inst_tst8
 //inline void inst_tst8 (unsigned char data)
 #ifdef use_lib_wifi
  void inst_tst8 (unsigned char data)
 #else
  void IRAM_ATTR inst_tst8 (unsigned char data)
 #endif 
#else
 inline void inst_tst8 (unsigned data)
#endif 
{//3261 por frame
	set_cc (FLAG_N, test_n (data));
	set_cc (FLAG_Z, test_z8 (data));
	set_cc (FLAG_V, 0);
}

#ifdef use_lib_optimice_inst_tst16
 //inline void inst_tst16 (unsigned short int data)
 void inst_tst16 (unsigned short int data)
#else
 inline void inst_tst16 (unsigned data)
#endif 
{//376 veces por frame
	set_cc (FLAG_N, test_n (data >> 8));
	set_cc (FLAG_Z, test_z16 (data));
	set_cc (FLAG_V, 0);
}

/* instruction: clr */

inline void inst_clr (void)
{
	set_cc (FLAG_N, 0);
	set_cc (FLAG_Z, 1);
	set_cc (FLAG_V, 0);
	set_cc (FLAG_C, 0);
}

/* instruction: suba/subb */

//inline unsigned inst_sub8 (unsigned data0, unsigned data1)
unsigned inst_sub8 (unsigned data0, unsigned data1)
{//84 veces por frame
	unsigned i0, i1, r;

	i0 = data0;
	i1 = ~data1;
	r = i0 + i1 + 1;
	
	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 1));

	return r;
}

/* instruction: sbca/sbcb/cmpa/cmpb.
 * only 8-bit version, 16-bit version not needed.
 */

//inline unsigned inst_sbc (unsigned data0, unsigned data1)
unsigned inst_sbc (unsigned data0, unsigned data1)
{//0 veces por frame
	unsigned i0, i1, c, r;

	i0 = data0;
	i1 = ~data1;
	c = 1 - get_cc (FLAG_C);
	r = i0 + i1 + c;

	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 1));

	return r;
}

//instruction: anda/andb/bita/bitb.
//only 8-bit version, 16-bit version not needed.
#ifdef use_lib_optimice_inst_and
 //inline unsigned char inst_and (unsigned char data0, unsigned char data1)
 #ifdef use_lib_wifi
  unsigned char inst_and (unsigned char data0, unsigned char data1)
 #else
  unsigned char IRAM_ATTR inst_and (unsigned char data0, unsigned char data1)
 #endif
#else
 inline unsigned inst_and (unsigned data0, unsigned data1)
#endif 
{//1623 veces por frame
   #ifdef use_lib_optimice_inst_and
    unsigned char r;
   #else
	unsigned r;
   #endif

	r = data0 & data1;

	inst_tst8 (r);

	return r;
}

/* instruction: eora/eorb.
 * only 8-bit version, 16-bit version not needed.
 */

//inline unsigned inst_eor (unsigned data0, unsigned data1)
unsigned inst_eor (unsigned data0, unsigned data1)
{//0 a 2 por frame 50 rainy
	unsigned r;

	r = data0 ^ data1;

	inst_tst8 (r);

	return r;
}

/* instruction: adca/adcb
 * only 8-bit version, 16-bit version not needed.
 */

//inline unsigned inst_adc (unsigned data0, unsigned data1)
unsigned inst_adc (unsigned data0, unsigned data1)
{//8 a 16 por frame
	unsigned i0, i1, c, r;

	i0 = data0;
	i1 = data1;
	c = get_cc (FLAG_C);
	r = i0 + i1 + c;

	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 0));

	return r;
}

/* instruction: ora/orb.
 * only 8-bit version, 16-bit version not needed.
 */

//inline unsigned inst_or (unsigned data0, unsigned data1)
unsigned inst_or (unsigned data0, unsigned data1)
{//39 por frame
	unsigned r;

	r = data0 | data1;

	inst_tst8 (r);

	return r;
}

/* instruction: adda/addb */

//inline unsigned inst_add8 (unsigned data0, unsigned data1)
unsigned inst_add8 (unsigned data0, unsigned data1)
{//18 a 20 por frame
	unsigned i0, i1, r;

	i0 = data0;
	i1 = data1;
	r = i0 + i1;

	set_cc (FLAG_H, test_c (i0 << 4, i1 << 4, r << 4, 0));
	set_cc (FLAG_N, test_n (r));
	set_cc (FLAG_Z, test_z8 (r));
	set_cc (FLAG_V, test_v (i0, i1, r));
	set_cc (FLAG_C, test_c (i0, i1, r, 0));

	return r;
}

/* instruction: addd */

//inline unsigned inst_add16 (unsigned data0, unsigned data1)
unsigned inst_add16 (unsigned data0, unsigned data1)
{//12 a 14 por frame
	unsigned i0, i1, r;

	i0 = data0;
	i1 = data1;
	r = i0 + i1;

	set_cc (FLAG_N, test_n (r >> 8));
	set_cc (FLAG_Z, test_z16 (r));
	set_cc (FLAG_V, test_v (i0 >> 8, i1 >> 8, r >> 8));
	set_cc (FLAG_C, test_c (i0 >> 8, i1 >> 8, r >> 8, 0));

	return r;
}

/* instruction: subd */

//inline unsigned inst_sub16 (unsigned data0, unsigned data1)
unsigned inst_sub16 (unsigned data0, unsigned data1)
{//33 a 43 por frame
	unsigned i0, i1, r;

	i0 = data0;
	i1 = ~data1;
	r = i0 + i1 + 1;

	set_cc (FLAG_N, test_n (r >> 8));
	set_cc (FLAG_Z, test_z16 (r));
	set_cc (FLAG_V, test_v (i0 >> 8, i1 >> 8, r >> 8));
	set_cc (FLAG_C, test_c (i0 >> 8, i1 >> 8, r >> 8, 1));

	return r;
}

/* instruction: 8-bit offset branch */

//inline void inst_bra8 (unsigned test, unsigned op, unsigned *cycles)
#ifdef use_lib_wifi
 void inst_bra8 (unsigned test, unsigned op, unsigned *cycles)
#else
 void IRAM_ATTR inst_bra8 (unsigned test, unsigned op, unsigned *cycles)
#endif 
{//1800 a 2000 por frame
	unsigned offset, mask;

	offset = pc_read8 ();

	//trying to avoid an if statement

	mask = (test ^ (op & 1)) - 1; //0xffff when taken, 0 when not taken
	reg_pc += sign_extend (offset) & mask;

	*cycles += 3;
}

/* instruction: 16-bit offset branch */

//inline void inst_bra16 (unsigned test, unsigned op, unsigned *cycles)
void inst_bra16 (unsigned test, unsigned op, unsigned *cycles)
{//85 a 99 por frame
	unsigned offset, mask;

	offset = pc_read16 ();

	/* trying to avoid an if statement */

	mask = (test ^ (op & 1)) - 1; /* 0xffff when taken, 0 when not taken */
	reg_pc += offset & mask;

	*cycles += 5 - mask;
}

//instruction: pshs/pshu
#ifdef use_lib_reg_16bits
 //inst_psh (0xff, &reg_s, reg_u, &cycles);
 void inst_psh (unsigned char op, unsigned short int *sp,
    				   unsigned short int data, unsigned int *cycles)   
#else
 inline void inst_psh (unsigned op, unsigned *sp,
					   unsigned data, unsigned *cycles)
#endif					   
{//31 a 99 por frame
	if (op & 0x80) {
		push16 (sp, reg_pc);
		*cycles += 2;
	}

	if (op & 0x40) {
		/* either s or u */
		push16 (sp, data);
		*cycles += 2;
	}

	if (op & 0x20) {
		push16 (sp, reg_y);
		*cycles += 2;
	}

	if (op & 0x10) {
		push16 (sp, reg_x);
		*cycles += 2;
	}

	if (op & 0x08) {
		push8 (sp, reg_dp);
		*cycles += 1;
	}

	if (op & 0x04) {
		push8 (sp, reg_b);
		*cycles += 1;
	}

	if (op & 0x02) {
		push8 (sp, reg_a);
		*cycles += 1;
	}

	if (op & 0x01) {
		push8 (sp, reg_cc);
		*cycles += 1;
	}
}

//instruction: puls/pulu
#ifdef use_lib_reg_16bits
 //inst_pul (pc_read8 (), &reg_s, &reg_u, &cycles);
 void inst_pul (unsigned char op, unsigned short int *sp, unsigned short int *osp,
     			       unsigned int *cycles)
#else
 inline void inst_pul (unsigned op, unsigned *sp, unsigned *osp,
					   unsigned *cycles)
#endif					   
{//75 a 99 por frame
	if (op & 0x01) {
		reg_cc = pull8 (sp);
		*cycles += 1;
	}

	if (op & 0x02) {
		reg_a = pull8 (sp);
		*cycles += 1;
	}

	if (op & 0x04) {
		reg_b = pull8 (sp);
		*cycles += 1;
	}

	if (op & 0x08) {
		reg_dp = pull8 (sp);
		*cycles += 1;
	}

	if (op & 0x10) {
		reg_x = pull16 (sp);
		*cycles += 2;
	}

	if (op & 0x20) {
		reg_y = pull16 (sp);
		*cycles += 2;
	}

	if (op & 0x40) {
		/* either s or u */
		*osp = pull16 (sp);
		*cycles += 2;
	}

	if (op & 0x80) {
		reg_pc = pull16 (sp);
		*cycles += 2;
	}
}

//inline unsigned exgtfr_read (unsigned reg)
unsigned exgtfr_read (unsigned reg)
{//25 a 46 por frame
	unsigned data;

	switch (reg) {
	case 0x0:
		data = get_reg_d ();
		break;
	case 0x1:
		data = reg_x;
		break;
	case 0x2:
		data = reg_y;
		break;
	case 0x3:
		data = reg_u;
		break;
	case 0x4:
		data = reg_s;
		break;
	case 0x5:
		data = reg_pc;
		break;
	case 0x8:
		data = 0xff00 | reg_a;
		break;
	case 0x9:
		data = 0xff00 | reg_b;
		break;
	case 0xa:
		data = 0xff00 | reg_cc;
		break;
	case 0xb:
		data = 0xff00 | reg_dp;
		break;
	default:
		data = 0xffff;
		//printf ("illegal exgtfr reg %.1x\n", reg);
		break;
	}

	return data;
}

//inline void exgtfr_write (unsigned reg, unsigned data)
void exgtfr_write (unsigned reg, unsigned data)
{//25 a 46 por frame
	switch (reg) {
	case 0x0:
		set_reg_d (data);
		break;
	case 0x1:
		reg_x = data;
		break;
	case 0x2:
		reg_y = data;
		break;
	case 0x3:
		reg_u = data;
		break;
	case 0x4:
		reg_s = data;
		break;
	case 0x5:
		reg_pc = data;
		break;
	case 0x8:
		reg_a = data;
		break;
	case 0x9:
		reg_b = data;
		break;
	case 0xa:
		reg_cc = data;
		break;
	case 0xb:
		reg_dp = data;
		break;
	default:
		//printf ("illegal exgtfr reg %.1x\n", reg);
		break;
	}
}

/* instruction: exg */

//inline void inst_exg (void)
void inst_exg (void)
{//4 a 8 por frame
	unsigned op, tmp;

	op = pc_read8 ();

	tmp = exgtfr_read (op & 0xf);
	exgtfr_write (op & 0xf, exgtfr_read (op >> 4));
	exgtfr_write (op >> 4, tmp);
}

/* instruction: tfr */

//inline void inst_tfr (void)
void inst_tfr (void)
{//20 a 38 por frame
	unsigned op;

	op = pc_read8 ();

	exgtfr_write (op & 0xf, exgtfr_read (op >> 4));
}

/* reset the 6809 */

void e6809_reset (void)
{
	reg_x = 0;
	reg_y = 0;
	reg_u = 0;
	reg_s = 0;

	reg_a = 0;
	reg_b = 0;

	reg_dp = 0;

	reg_cc = FLAG_I | FLAG_F;
	irq_status = IRQ_NORMAL;

	reg_pc = read16 (0xfffe);
}

//execute a single instruction or handle interrupts and return */
#ifdef use_lib_optimice_e6809_sstep
 //JJ unsigned int e6809_sstep (unsigned char irq_i, unsigned char irq_f)
 unsigned int e6809_sstep (unsigned char irq_i)
#else
 unsigned e6809_sstep (unsigned irq_i, unsigned irq_f)
#endif 
 {
	unsigned op;
	unsigned cycles = 0;
	unsigned ea, i0, i1, r;

    /*No se usa irq_f = 0 siempre
	if (irq_f)
	{
		if (get_cc (FLAG_F) == 0) {
			if (irq_status != IRQ_CWAI) {
				set_cc (FLAG_E, 0);
				inst_psh (0x81, &reg_s, reg_u, &cycles);
			}

			set_cc (FLAG_I, 1);
			set_cc (FLAG_F, 1);

			reg_pc = read16 (0xfff6);
			irq_status = IRQ_NORMAL;
			cycles += 7;
		} else {
			if (irq_status == IRQ_SYNC) {
				irq_status = IRQ_NORMAL;
			}
		}
	}
	*/
	
	if (irq_i) {
		if (get_cc (FLAG_I) == 0) {
			if (irq_status != IRQ_CWAI) {
				set_cc (FLAG_E, 1);
				inst_psh (0xff, &reg_s, reg_u, &cycles);
			}

			set_cc (FLAG_I, 1);

			reg_pc = read16 (0xfff8);
			irq_status = IRQ_NORMAL;
			cycles += 7;
		} else {
			if (irq_status == IRQ_SYNC) {
				irq_status = IRQ_NORMAL;
			}
		}
	}

	if (irq_status != IRQ_NORMAL) {
		return cycles + 1;
	}

	op = pc_read8 ();

	switch (op) {
	/* page 0 instructions */

	/* neg, nega, negb */
	case 0x00:
		ea = ea_direct ();
		r = inst_neg (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x40:
		reg_a = inst_neg (reg_a);
		cycles += 2;
		break;
	case 0x50:
		reg_b = inst_neg (reg_b);
		cycles += 2;
		break;
	case 0x60:
		ea = ea_indexed (&cycles);
		r = inst_neg (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x70:
		ea = ea_extended ();
		r = inst_neg (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* com, coma, comb */
	case 0x03:
		ea = ea_direct ();
		r = inst_com (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x43:
		reg_a = inst_com (reg_a);
		cycles += 2;
		break;
	case 0x53:
		reg_b = inst_com (reg_b);
		cycles += 2;
		break;
	case 0x63:
		ea = ea_indexed (&cycles);
		r = inst_com (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x73:
		ea = ea_extended ();
		r = inst_com (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* lsr, lsra, lsrb */
	case 0x04:
		ea = ea_direct ();
		r = inst_lsr (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x44:
		reg_a = inst_lsr (reg_a);
		cycles += 2;
		break;
	case 0x54:
		reg_b = inst_lsr (reg_b);
		cycles += 2;
		break;
	case 0x64:
		ea = ea_indexed (&cycles);
		r = inst_lsr (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x74:
		ea = ea_extended ();
		r = inst_lsr (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* ror, rora, rorb */
	case 0x06:
		ea = ea_direct ();
		r = inst_ror (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x46:
		reg_a = inst_ror (reg_a);
		cycles += 2;
		break;
	case 0x56:
		reg_b = inst_ror (reg_b);
		cycles += 2;
		break;
	case 0x66:
		ea = ea_indexed (&cycles);
		r = inst_ror (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x76:
		ea = ea_extended ();
		r = inst_ror (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* asr, asra, asrb */
	case 0x07:
		ea = ea_direct ();
		r = inst_asr (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x47:
		reg_a = inst_asr (reg_a);
		cycles += 2;
		break;
	case 0x57:
		reg_b = inst_asr (reg_b);
		cycles += 2;
		break;
	case 0x67:
		ea = ea_indexed (&cycles);
		r = inst_asr (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x77:
		ea = ea_extended ();
		r = inst_asr (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* asl, asla, aslb */
	case 0x08:
		ea = ea_direct ();
		r = inst_asl (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x48:
		reg_a = inst_asl (reg_a);
		cycles += 2;
		break;
	case 0x58:
		reg_b = inst_asl (reg_b);
		cycles += 2;
		break;
	case 0x68:
		ea = ea_indexed (&cycles);
		r = inst_asl (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x78:
		ea = ea_extended ();
		r = inst_asl (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* rol, rola, rolb */
	case 0x09:
		ea = ea_direct ();
		r = inst_rol (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x49:
		reg_a = inst_rol (reg_a);
		cycles += 2;
		break;
	case 0x59:
		reg_b = inst_rol (reg_b);
		cycles += 2;
		break;
	case 0x69:
		ea = ea_indexed (&cycles);
		r = inst_rol (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x79:
		ea = ea_extended ();
		r = inst_rol (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* dec, deca, decb */
	case 0x0a:
		ea = ea_direct ();
		r = inst_dec (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x4a:
		reg_a = inst_dec (reg_a);
		cycles += 2;
		break;
	case 0x5a:
		reg_b = inst_dec (reg_b);
		cycles += 2;
		break;
	case 0x6a:
		ea = ea_indexed (&cycles);
		r = inst_dec (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x7a:
		ea = ea_extended ();
		r = inst_dec (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* inc, inca, incb */
	case 0x0c:
		ea = ea_direct ();
		r = inst_inc (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x4c:
		reg_a = inst_inc (reg_a);
		cycles += 2;
		break;
	case 0x5c:
		reg_b = inst_inc (reg_b);
		cycles += 2;
		break;
	case 0x6c:
		ea = ea_indexed (&cycles);
		r = inst_inc (read8 (ea));
		write8 (ea, r);
		cycles += 6;
		break;
	case 0x7c:
		ea = ea_extended ();
		r = inst_inc (read8 (ea));
		write8 (ea, r);
		cycles += 7;
		break;
	/* tst, tsta, tstb */
	case 0x0d:
		ea = ea_direct ();
		inst_tst8 (read8 (ea));
		cycles += 6;
		break;
	case 0x4d:
		inst_tst8 (reg_a);
		cycles += 2;
		break;
	case 0x5d:
		inst_tst8 (reg_b);
		cycles += 2;
		break;
	case 0x6d:
		ea = ea_indexed (&cycles);
		inst_tst8 (read8 (ea));
		cycles += 6;
		break;
	case 0x7d:
		ea = ea_extended ();
		inst_tst8 (read8 (ea));
		cycles += 7;
		break;
	/* jmp */
	case 0x0e:
		reg_pc = ea_direct ();
		cycles += 3;
		break;
	case 0x6e:
		reg_pc = ea_indexed (&cycles);
		cycles += 3;
		break;
	case 0x7e:
		reg_pc = ea_extended ();
		cycles += 4;
		break;
	/* clr */
	case 0x0f:
		ea = ea_direct ();
		inst_clr ();
		write8 (ea, 0);
		cycles += 6;
		break;
	case 0x4f:
		inst_clr ();
		reg_a = 0;
		cycles += 2;
		break;
	case 0x5f:
		inst_clr ();
		reg_b = 0;
		cycles += 2;
		break;
	case 0x6f:
		ea = ea_indexed (&cycles);
		inst_clr ();
		write8 (ea, 0);
		cycles += 6;
		break;
	case 0x7f:
		ea = ea_extended ();
		inst_clr ();
		write8 (ea, 0);
		cycles += 7;
		break;
	/* suba */
	case 0x80:
		reg_a = inst_sub8 (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x90:
		ea = ea_direct ();
		reg_a = inst_sub8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa0:
		ea = ea_indexed (&cycles);
		reg_a = inst_sub8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb0:
		ea = ea_extended ();
		reg_a = inst_sub8 (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* subb */
	case 0xc0:
		reg_b = inst_sub8 (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd0:
		ea = ea_direct ();
		reg_b = inst_sub8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe0:
		ea = ea_indexed (&cycles);
		reg_b = inst_sub8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf0:
		ea = ea_extended ();
		reg_b = inst_sub8 (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* cmpa */
	case 0x81:
		inst_sub8 (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x91:
		ea = ea_direct ();
		inst_sub8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa1:
		ea = ea_indexed (&cycles);
		inst_sub8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb1:
		ea = ea_extended ();
		inst_sub8 (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* cmpb */
	case 0xc1:
		inst_sub8 (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd1:
		ea = ea_direct ();
		inst_sub8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe1:
		ea = ea_indexed (&cycles);
		inst_sub8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf1:
		ea = ea_extended ();
		inst_sub8 (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* sbca */
	case 0x82:
		reg_a = inst_sbc (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x92:
		ea = ea_direct ();
		reg_a = inst_sbc (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa2:
		ea = ea_indexed (&cycles);
		reg_a = inst_sbc (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb2:
		ea = ea_extended ();
		reg_a = inst_sbc (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* sbcb */
	case 0xc2:
		reg_b = inst_sbc (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd2:
		ea = ea_direct ();
		reg_b = inst_sbc (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe2:
		ea = ea_indexed (&cycles);
		reg_b = inst_sbc (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf2:
		ea = ea_extended ();
		reg_b = inst_sbc (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* anda */
	case 0x84:
		reg_a = inst_and (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x94:
		ea = ea_direct ();
		reg_a = inst_and (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa4:
		ea = ea_indexed (&cycles);
		reg_a = inst_and (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb4:
		ea = ea_extended ();
		reg_a = inst_and (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* andb */
	case 0xc4:
		reg_b = inst_and (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd4:
		ea = ea_direct ();
		reg_b = inst_and (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe4:
		ea = ea_indexed (&cycles);
		reg_b = inst_and (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf4:
		ea = ea_extended ();
		reg_b = inst_and (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* bita */
	case 0x85:
		inst_and (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x95:
		ea = ea_direct ();
		inst_and (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa5:
		ea = ea_indexed (&cycles);
		inst_and (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb5:
		ea = ea_extended ();
		inst_and (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* bitb */
	case 0xc5:
		inst_and (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd5:
		ea = ea_direct ();
		inst_and (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe5:
		ea = ea_indexed (&cycles);
		inst_and (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf5:
		ea = ea_extended ();
		inst_and (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* lda */
	case 0x86:
		reg_a = pc_read8 ();
		inst_tst8 (reg_a);
		cycles += 2;
		break;
	case 0x96:
		ea = ea_direct ();
		reg_a = read8 (ea);
		inst_tst8 (reg_a);
		cycles += 4;
		break;
	case 0xa6:
		ea = ea_indexed (&cycles);
		reg_a = read8 (ea);
		inst_tst8 (reg_a);
		cycles += 4;
		break;
	case 0xb6:
		ea = ea_extended ();
		reg_a = read8 (ea);
		inst_tst8 (reg_a);
		cycles += 5;
		break;
	/* ldb */
	case 0xc6:
		reg_b = pc_read8 ();
		inst_tst8 (reg_b);
		cycles += 2;
		break;
	case 0xd6:
		ea = ea_direct ();
		reg_b = read8 (ea);
		inst_tst8 (reg_b);
		cycles += 4;
		break;
	case 0xe6:
		ea = ea_indexed (&cycles);
		reg_b = read8 (ea);
		inst_tst8 (reg_b);
		cycles += 4;
		break;
	case 0xf6:
		ea = ea_extended ();
		reg_b = read8 (ea);
		inst_tst8 (reg_b);
		cycles += 5;
		break;
	/* sta */
	case 0x97:
		ea = ea_direct ();
		write8 (ea, reg_a);
		inst_tst8 (reg_a);
		cycles += 4;
		break;
	case 0xa7:
		ea = ea_indexed (&cycles);
		write8 (ea, reg_a);
		inst_tst8 (reg_a);
		cycles += 4;
		break;
	case 0xb7:
		ea = ea_extended ();
		write8 (ea, reg_a);
		inst_tst8 (reg_a);
		cycles += 5;
		break;
	/* stb */
	case 0xd7:
		ea = ea_direct ();
		write8 (ea, reg_b);
		inst_tst8 (reg_b);
		cycles += 4;
		break;
	case 0xe7:
		ea = ea_indexed (&cycles);
		write8 (ea, reg_b);
		inst_tst8 (reg_b);
		cycles += 4;
		break;
	case 0xf7:
		ea = ea_extended ();
		write8 (ea, reg_b);
		inst_tst8 (reg_b);
		cycles += 5;
		break;
	/* eora */
	case 0x88:
		reg_a = inst_eor (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x98:
		ea = ea_direct ();
		reg_a = inst_eor (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa8:
		ea = ea_indexed (&cycles);
		reg_a = inst_eor (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb8:
		ea = ea_extended ();
		reg_a = inst_eor (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* eorb */
	case 0xc8:
		reg_b = inst_eor (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd8:
		ea = ea_direct ();
		reg_b = inst_eor (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe8:
		ea = ea_indexed (&cycles);
		reg_b = inst_eor (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf8:
		ea = ea_extended ();
		reg_b = inst_eor (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* adca */
	case 0x89:
		reg_a = inst_adc (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x99:
		ea = ea_direct ();
		reg_a = inst_adc (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xa9:
		ea = ea_indexed (&cycles);
		reg_a = inst_adc (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xb9:
		ea = ea_extended ();
		reg_a = inst_adc (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* adcb */
	case 0xc9:
		reg_b = inst_adc (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xd9:
		ea = ea_direct ();
		reg_b = inst_adc (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xe9:
		ea = ea_indexed (&cycles);
		reg_b = inst_adc (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xf9:
		ea = ea_extended ();
		reg_b = inst_adc (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* ora */
	case 0x8a:
		reg_a = inst_or (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x9a:
		ea = ea_direct ();
		reg_a = inst_or (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xaa:
		ea = ea_indexed (&cycles);
		reg_a = inst_or (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xba:
		ea = ea_extended ();
		reg_a = inst_or (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* orb */
	case 0xca:
		reg_b = inst_or (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xda:
		ea = ea_direct ();
		reg_b = inst_or (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xea:
		ea = ea_indexed (&cycles);
		reg_b = inst_or (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xfa:
		ea = ea_extended ();
		reg_b = inst_or (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* adda */
	case 0x8b:
		reg_a = inst_add8 (reg_a, pc_read8 ());
		cycles += 2;
		break;
	case 0x9b:
		ea = ea_direct ();
		reg_a = inst_add8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xab:
		ea = ea_indexed (&cycles);
		reg_a = inst_add8 (reg_a, read8 (ea));
		cycles += 4;
		break;
	case 0xbb:
		ea = ea_extended ();
		reg_a = inst_add8 (reg_a, read8 (ea));
		cycles += 5;
		break;
	/* addb */
	case 0xcb:
		reg_b = inst_add8 (reg_b, pc_read8 ());
		cycles += 2;
		break;
	case 0xdb:
		ea = ea_direct ();
		reg_b = inst_add8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xeb:
		ea = ea_indexed (&cycles);
		reg_b = inst_add8 (reg_b, read8 (ea));
		cycles += 4;
		break;
	case 0xfb:
		ea = ea_extended ();
		reg_b = inst_add8 (reg_b, read8 (ea));
		cycles += 5;
		break;
	/* subd */
	case 0x83:
		set_reg_d (inst_sub16 (get_reg_d (), pc_read16 ()));
		cycles += 4;
		break;
	case 0x93:
		ea = ea_direct ();
		set_reg_d (inst_sub16 (get_reg_d (), read16 (ea)));
		cycles += 6;
		break;
	case 0xa3:
		ea = ea_indexed (&cycles);
		set_reg_d (inst_sub16 (get_reg_d (), read16 (ea)));
		cycles += 6;
		break;
	case 0xb3:
		ea = ea_extended ();
		set_reg_d (inst_sub16 (get_reg_d (), read16 (ea)));
		cycles += 7;
		break;
	/* cmpx */
	case 0x8c:
		inst_sub16 (reg_x, pc_read16 ());
		cycles += 4;
		break;
	case 0x9c:
		ea = ea_direct ();
		inst_sub16 (reg_x, read16 (ea));
		cycles += 6;
		break;
	case 0xac:
		ea = ea_indexed (&cycles);
		inst_sub16 (reg_x, read16 (ea));
		cycles += 6;
		break;
	case 0xbc:
		ea = ea_extended ();
		inst_sub16 (reg_x, read16 (ea));
		cycles += 7;
		break;
	/* ldx */
	case 0x8e:
		reg_x = pc_read16 ();
		inst_tst16 (reg_x);
		cycles += 3;
		break;
	case 0x9e: 
		ea = ea_direct ();
		reg_x = read16 (ea);
		inst_tst16 (reg_x);
		cycles += 5;
		break;
	case 0xae:
		ea = ea_indexed (&cycles);
		reg_x = read16 (ea);
		inst_tst16 (reg_x);
		cycles += 5;
		break;
	case 0xbe:
		ea = ea_extended ();
		reg_x = read16 (ea);
		inst_tst16 (reg_x);
		cycles += 6;
		break;
	/* ldu */
	case 0xce:
		reg_u = pc_read16 ();
		inst_tst16 (reg_u);
		cycles += 3;
		break;
	case 0xde: 
		ea = ea_direct ();
		reg_u = read16 (ea);
		inst_tst16 (reg_u);
		cycles += 5;
		break;
	case 0xee:
		ea = ea_indexed (&cycles);
		reg_u = read16 (ea);
		inst_tst16 (reg_u);
		cycles += 5;
		break;
	case 0xfe:
		ea = ea_extended ();
		reg_u = read16 (ea);
		inst_tst16 (reg_u);
		cycles += 6;
		break;
	/* stx */
	case 0x9f:
		ea = ea_direct ();
		write16 (ea, reg_x);
		inst_tst16 (reg_x);
		cycles += 5;
		break;
	case 0xaf:
		ea = ea_indexed (&cycles);
		write16 (ea, reg_x);
		inst_tst16 (reg_x);
		cycles += 5;
		break;
	case 0xbf:
		ea = ea_extended ();
		write16 (ea, reg_x);
		inst_tst16 (reg_x);
		cycles += 6;
		break;
	/* stu */
	case 0xdf:
		ea = ea_direct ();
		write16 (ea, reg_u);
		inst_tst16 (reg_u);
		cycles += 5;
		break;
	case 0xef:
		ea = ea_indexed (&cycles);
		write16 (ea, reg_u);
		inst_tst16 (reg_u);
		cycles += 5;
		break;
	case 0xff:
		ea = ea_extended ();
		write16 (ea, reg_u);
		inst_tst16 (reg_u);
		cycles += 6;
		break;
	/* addd */
	case 0xc3:
		set_reg_d (inst_add16 (get_reg_d (), pc_read16 ()));
		cycles += 4;
		break;
	case 0xd3:
		ea = ea_direct ();
		set_reg_d (inst_add16 (get_reg_d (), read16 (ea)));
		cycles += 6;
		break;
	case 0xe3:
		ea = ea_indexed (&cycles);
		set_reg_d (inst_add16 (get_reg_d (), read16 (ea)));
		cycles += 6;
		break;
	case 0xf3:
		ea = ea_extended ();
		set_reg_d (inst_add16 (get_reg_d (), read16 (ea)));
		cycles += 7;
		break;
	/* ldd */
	case 0xcc:
		set_reg_d (pc_read16 ());
		inst_tst16 (get_reg_d ());
		cycles += 3;
		break;
	case 0xdc: 
		ea = ea_direct ();
		set_reg_d (read16 (ea));
		inst_tst16 (get_reg_d ());
		cycles += 5;
		break;
	case 0xec:
		ea = ea_indexed (&cycles);
		set_reg_d (read16 (ea));
		inst_tst16 (get_reg_d ());
		cycles += 5;
		break;
	case 0xfc:
		ea = ea_extended ();
		set_reg_d (read16 (ea));
		inst_tst16 (get_reg_d ());
		cycles += 6;
		break;
	/* std */
	case 0xdd:
		ea = ea_direct ();
		write16 (ea, get_reg_d ());
		inst_tst16 (get_reg_d ());
		cycles += 5;
		break;
	case 0xed:
		ea = ea_indexed (&cycles);
		write16 (ea, get_reg_d ());
		inst_tst16 (get_reg_d ());
		cycles += 5;
		break;
	case 0xfd:
		ea = ea_extended ();
		write16 (ea, get_reg_d ());
		inst_tst16 (get_reg_d ());
		cycles += 6;
		break;
	/* nop */
	case 0x12:
		cycles += 2;
		break;
	/* mul */
	case 0x3d:
		r = (reg_a & 0xff) * (reg_b & 0xff);
		set_reg_d (r);

		set_cc (FLAG_Z, test_z16 (r));
		set_cc (FLAG_C, (r >> 7) & 1);
		
		cycles += 11;
		break;
	/* bra */
	case 0x20:
	/* brn */
	case 0x21:
		inst_bra8 (0, op, &cycles);
		break;
	/* bhi */
	case 0x22:
	/* bls */
	case 0x23:
		inst_bra8 (get_cc (FLAG_C) | get_cc (FLAG_Z), op, &cycles);
		break;
	/* bhs/bcc */
	case 0x24:
	/* blo/bcs */
	case 0x25:
		inst_bra8 (get_cc (FLAG_C), op, &cycles);
		break;
	/* bne */
	case 0x26:
	/* beq */
	case 0x27:
		inst_bra8 (get_cc (FLAG_Z), op, &cycles);
		break;
	/* bvc */
	case 0x28:
	/* bvs */
	case 0x29:
		inst_bra8 (get_cc (FLAG_V), op, &cycles);
		break;
	/* bpl */
	case 0x2a:
	/* bmi */
	case 0x2b:
		inst_bra8 (get_cc (FLAG_N), op, &cycles);
		break;
	/* bge */
	case 0x2c:
	/* blt */
	case 0x2d:
		inst_bra8 (get_cc (FLAG_N) ^ get_cc (FLAG_V), op, &cycles);
		break;
	/* bgt */
	case 0x2e:
	/* ble */
	case 0x2f:
		inst_bra8 (get_cc (FLAG_Z) |
				   (get_cc (FLAG_N) ^ get_cc (FLAG_V)), op, &cycles);
		break;
	/* lbra */
	case 0x16:
		r = pc_read16 ();
		reg_pc += r;
		cycles += 5;
		break;
	/* lbsr */
	case 0x17:
		r = pc_read16 ();
		push16 (&reg_s, reg_pc);
		reg_pc += r;
		cycles += 9;
		break;
	/* bsr */
	case 0x8d:
		r = pc_read8 ();
		push16 (&reg_s, reg_pc);
		reg_pc += sign_extend (r);
		cycles += 7;
		break;
	/* jsr */
	case 0x9d:
		ea = ea_direct ();
		push16 (&reg_s, reg_pc);
		reg_pc = ea;
		cycles += 7;
		break;
	case 0xad:
		ea = ea_indexed (&cycles);
		push16 (&reg_s, reg_pc);
		reg_pc = ea;
		cycles += 7;
		break;
	case 0xbd:
		ea = ea_extended ();
		push16 (&reg_s, reg_pc);
		reg_pc = ea;
		cycles += 8;
		break;
	/* leax */
	case 0x30:
		reg_x = ea_indexed (&cycles);
		set_cc (FLAG_Z, test_z16 (reg_x));
		cycles += 4;
		break;
	/* leay */
	case 0x31:
		reg_y = ea_indexed (&cycles);
		set_cc (FLAG_Z, test_z16 (reg_y));
		cycles += 4;
		break;
	/* leas */
	case 0x32:
		reg_s = ea_indexed (&cycles);
		cycles += 4;
		break;
	/* leau */
	case 0x33:
		reg_u = ea_indexed (&cycles);
		cycles += 4;
		break;
	/* pshs */
	case 0x34:
		inst_psh (pc_read8 (), &reg_s, reg_u, &cycles);
		cycles += 5;
		break;
	/* puls */
	case 0x35:
		inst_pul (pc_read8 (), &reg_s, &reg_u, &cycles);
		cycles += 5;
		break;
	/* pshu */
	case 0x36:
		inst_psh (pc_read8 (), &reg_u, reg_s, &cycles);
		cycles += 5;
		break;
	/* pulu */
	case 0x37:
		inst_pul (pc_read8 (), &reg_u, &reg_s, &cycles);
		cycles += 5;
		break;
	/* rts */
	case 0x39:
		reg_pc = pull16 (&reg_s);
		cycles += 5;
		break;
	/* abx */
	case 0x3a:
		reg_x += reg_b & 0xff;
		cycles += 3;
		break;
	/* orcc */
	case 0x1a:
		reg_cc |= pc_read8 ();
		cycles += 3;
		break;
	/* andcc */
	case 0x1c:
		reg_cc &= pc_read8 ();
		cycles += 3;
		break;
	/* sex */
	case 0x1d:
		set_reg_d (sign_extend (reg_b));
		set_cc (FLAG_N, test_n (reg_a));
		set_cc (FLAG_Z, test_z16 (get_reg_d ()));
		cycles += 2;
		break;
	/* exg */
	case 0x1e:
		inst_exg ();
		cycles += 8;
		break;
	/* tfr */
	case 0x1f:
		inst_tfr ();
		cycles += 6;
		break;
	/* rti */
	case 0x3b:
		if (get_cc (FLAG_E)) {
			inst_pul (0xff, &reg_s, &reg_u, &cycles);
		} else {
			inst_pul (0x81, &reg_s, &reg_u, &cycles);
		}

		cycles += 3;
		break;
	/* swi */
	case 0x3f:
		set_cc (FLAG_E, 1);
		inst_psh (0xff, &reg_s, reg_u, &cycles);
		set_cc (FLAG_I, 1);
		set_cc (FLAG_F, 1);
        reg_pc = read16 (0xfffa);
        cycles += 7;
		break;
	/* sync */
	case 0x13:
		irq_status = IRQ_SYNC;
		cycles += 2;
		break;
	/* daa */
	case 0x19:
		i0 = reg_a;
		i1 = 0;

		if ((reg_a & 0x0f) > 0x09 || get_cc (FLAG_H) == 1) {
			i1 |= 0x06;
		}

		if ((reg_a & 0xf0) > 0x80 && (reg_a & 0x0f) > 0x09) {
			i1 |= 0x60;
		}

		if ((reg_a & 0xf0) > 0x90 || get_cc (FLAG_C) == 1) {
			i1 |= 0x60;
		}

		reg_a = i0 + i1;

		set_cc (FLAG_N, test_n (reg_a));
		set_cc (FLAG_Z, test_z8 (reg_a));
		set_cc (FLAG_V, 0);
		set_cc (FLAG_C, test_c (i0, i1, reg_a, 0));
		cycles += 2;
		break;
	/* cwai */
	case 0x3c:
		reg_cc &= pc_read8 ();
		set_cc (FLAG_E, 1);
		inst_psh (0xff, &reg_s, reg_u, &cycles);
		irq_status = IRQ_CWAI;
		cycles += 4;
		break;

	/* page 1 instructions */

	case 0x10:
		op = pc_read8 ();

		switch (op) {
		/* lbra */
		case 0x20:
		/* lbrn */
		case 0x21:
			inst_bra16 (0, op, &cycles);
			break;
		/* lbhi */
		case 0x22:
		/* lbls */
		case 0x23:
			inst_bra16 (get_cc (FLAG_C) | get_cc (FLAG_Z), op, &cycles);
			break;
		/* lbhs/lbcc */
		case 0x24:
		/* lblo/lbcs */
		case 0x25:
			inst_bra16 (get_cc (FLAG_C), op, &cycles);
			break;
		/* lbne */
		case 0x26:
		/* lbeq */
		case 0x27:
			inst_bra16 (get_cc (FLAG_Z), op, &cycles);
			break;
		/* lbvc */
		case 0x28:
		/* lbvs */
		case 0x29:
			inst_bra16 (get_cc (FLAG_V), op, &cycles);
			break;
		/* lbpl */
		case 0x2a:
		/* lbmi */
		case 0x2b:
			inst_bra16 (get_cc (FLAG_N), op, &cycles);
			break;
		/* lbge */
		case 0x2c:
		/* lblt */
		case 0x2d:
			inst_bra16 (get_cc (FLAG_N) ^ get_cc (FLAG_V), op, &cycles);
			break;
		/* lbgt */
		case 0x2e:
		/* lble */
		case 0x2f:
			inst_bra16 (get_cc (FLAG_Z) |
						(get_cc (FLAG_N) ^ get_cc (FLAG_V)), op, &cycles);
			break;
		/* cmpd */
		case 0x83:
			inst_sub16 (get_reg_d (), pc_read16 ());
			cycles += 5;
			break;
		case 0x93:
			ea = ea_direct ();
			inst_sub16 (get_reg_d (), read16 (ea));
			cycles += 7;
			break;
		case 0xa3:
			ea = ea_indexed (&cycles);
			inst_sub16 (get_reg_d (), read16 (ea));
			cycles += 7;
			break;
		case 0xb3:
			ea = ea_extended ();
			inst_sub16 (get_reg_d (), read16 (ea));
			cycles += 8;
			break;
		/* cmpy */
		case 0x8c:
			inst_sub16 (reg_y, pc_read16 ());
			cycles += 5;
			break;
		case 0x9c:
			ea = ea_direct ();
			inst_sub16 (reg_y, read16 (ea));
			cycles += 7;
			break;
		case 0xac:
			ea = ea_indexed (&cycles);
			inst_sub16 (reg_y, read16 (ea));
			cycles += 7;
			break;
		case 0xbc:
			ea = ea_extended ();
			inst_sub16 (reg_y, read16 (ea));
			cycles += 8;
			break;
		/* ldy */
		case 0x8e:
			reg_y = pc_read16 ();
			inst_tst16 (reg_y);
			cycles += 4;
			break;
		case 0x9e: 
			ea = ea_direct ();
			reg_y = read16 (ea);
			inst_tst16 (reg_y);
			cycles += 6;
			break;
		case 0xae:
			ea = ea_indexed (&cycles);
			reg_y = read16 (ea);
			inst_tst16 (reg_y);
			cycles += 6;
			break;
		case 0xbe:
			ea = ea_extended ();
			reg_y = read16 (ea);
			inst_tst16 (reg_y);
			cycles += 7;
			break;
		/* sty */
		case 0x9f:
			ea = ea_direct ();
			write16 (ea, reg_y);
			inst_tst16 (reg_y);
			cycles += 6;
			break;
		case 0xaf:
			ea = ea_indexed (&cycles);
			write16 (ea, reg_y);
			inst_tst16 (reg_y);
			cycles += 6;
			break;
		case 0xbf:
			ea = ea_extended ();
			write16 (ea, reg_y);
			inst_tst16 (reg_y);
			cycles += 7;
			break;
		/* lds */
		case 0xce:
			reg_s = pc_read16 ();
			inst_tst16 (reg_s);
			cycles += 4;
			break;
		case 0xde: 
			ea = ea_direct ();
			reg_s = read16 (ea);
			inst_tst16 (reg_s);
			cycles += 6;
			break;
		case 0xee:
			ea = ea_indexed (&cycles);
			reg_s = read16 (ea);
			inst_tst16 (reg_s);
			cycles += 6;
			break;
		case 0xfe:
			ea = ea_extended ();
			reg_s = read16 (ea);
			inst_tst16 (reg_s);
			cycles += 7;
			break;
		/* sts */
		case 0xdf:
			ea = ea_direct ();
			write16 (ea, reg_s);
			inst_tst16 (reg_s);
			cycles += 6;
			break;
		case 0xef:
			ea = ea_indexed (&cycles);
			write16 (ea, reg_s);
			inst_tst16 (reg_s);
			cycles += 6;
			break;
		case 0xff:
			ea = ea_extended ();
			write16 (ea, reg_s);
			inst_tst16 (reg_s);
			cycles += 7;
			break;
		/* swi2 */
		case 0x3f:
			set_cc (FLAG_E, 1);
			inst_psh (0xff, &reg_s, reg_u, &cycles);
		    reg_pc = read16 (0xfff4);
			cycles += 8;
			break;
		default:
			//printf ("unknown page-1 op code: %.2x\n", op);
			break;
		}

		break;

	/* page 2 instructions */

	case 0x11:
		op = pc_read8 ();

		switch (op) {
		/* cmpu */
		case 0x83:
			inst_sub16 (reg_u, pc_read16 ());
			cycles += 5;
			break;
		case 0x93:
			ea = ea_direct ();
			inst_sub16 (reg_u, read16 (ea));
			cycles += 7;
			break;
		case 0xa3:
			ea = ea_indexed (&cycles);
			inst_sub16 (reg_u, read16 (ea));
			cycles += 7;
			break;
		case 0xb3:
			ea = ea_extended ();
			inst_sub16 (reg_u, read16 (ea));
			cycles += 8;
			break;
		/* cmps */
		case 0x8c:
			inst_sub16 (reg_s, pc_read16 ());
			cycles += 5;
			break;
		case 0x9c:
			ea = ea_direct ();
			inst_sub16 (reg_s, read16 (ea));
			cycles += 7;
			break;
		case 0xac:
			ea = ea_indexed (&cycles);
			inst_sub16 (reg_s, read16 (ea));
			cycles += 7;
			break;
		case 0xbc:
			ea = ea_extended ();
			inst_sub16 (reg_s, read16 (ea));
			cycles += 8;
			break;
		/* swi3 */
		case 0x3f:
			set_cc (FLAG_E, 1);
			inst_psh (0xff, &reg_s, reg_u, &cycles);
		    reg_pc = read16 (0xfff2);
			cycles += 8;
			break;
		default:
			//printf ("unknown page-2 op code: %.2x\n", op);
			break;
		}

		break;

	default:
		//printf ("unknown page-0 op code: %.2x\n", op);
		break;
	}

	return cycles;
 }

