/* file: "primitives.c" */

/*
 * Copyright 2004-2009 by Marc Feeley and Vincent St-Amour, All Rights Reserved.
 */

#include "picobit-vm.h"

/*---------------------------------------------------------------------------*/

#ifdef WORKSTATION

char *prim_name[64] =
  {
    "prim #%number?",
    "prim #%+",
    "prim #%-",
    "prim #%*",
    "prim #%quotient",
    "prim #%remainder",
    "prim #%neg",
    "prim #%=",
    "prim #%<",
    "prim #%ior",
    "prim #%>",
    "prim #%xor",
    "prim #%pair?",
    "prim #%cons",
    "prim #%car",
    "prim #%cdr",
    "prim #%set-car!",
    "prim #%set-cdr!",
    "prim #%null?",
    "prim #%eq?",
    "prim #%not",
    "prim #%get-cont",
    "prim #%graft-to-cont",
    "prim #%return-to-cont",
    "prim #%halt",
    "prim #%symbol?",
    "prim #%string?",
    "prim #%string->list",
    "prim #%list->string",
    "prim #%make-u8vector",
    "prim #%u8vector-ref",
    "prim #%u8vector-set!",
    "prim #%print",
    "prim #%clock",
    "prim #%motor",
    "prim #%led",
    "prim #%led2-color",
    "prim #%getchar-wait",
    "prim #%putchar",
    "prim #%beep",
    "prim #%adc",
    "prim #%u8vector?",
    "prim #%sernum",
    "prim #%u8vector-length",
    "prim #%u8vector-copy!",
    "shift",
    "pop",
    "return",
    "prim #%boolean?",
    "prim #%network-init",
    "prim #%network-cleanup",
    "prim #%receive-packet-to-u8vector",
    "prim #%send-packet-from-u8vector",
    "prim #%<=",
    "prim #%>=",
    "prim 55",
    "prim 56",
    "prim 57",
    "prim 58",
    "prim 59",
    "prim 60",
    "prim 61",
    "prim 62",
    "prim 63"
  };

#endif

/*---------------------------------------------------------------------------*/

// numerical primitives

void prim_numberp (void) {
  if (arg1 >= MIN_FIXNUM_ENCODING
      && arg1 <= (MIN_FIXNUM_ENCODING + (MAX_FIXNUM - MIN_FIXNUM)))
    arg1 = OBJ_TRUE;
  else {
    if (IN_RAM(arg1))
      arg1 = encode_bool (RAM_BIGNUM(arg1));
    else if (IN_ROM(arg1))
      arg1 = encode_bool (ROM_BIGNUM(arg1));
    else
      arg1 = OBJ_FALSE;
  }
}

void prim_add (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = add (arg1, arg2);
#else
  decode_2_int_args ();
  arg1 = encode_int (a1 + a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_sub (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = sub (arg1, arg2);
#else
  decode_2_int_args ();
  arg1 = encode_int (a1 - a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_mul (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  a1 = negp (arg1);
  a2 = negp (arg2); // -1 if negative
  arg1 = mulnonneg (a1 ? neg(arg1) : arg1,
		    a2 ? neg(arg2) : arg2);
  if (a1 + a2 == 1) // only one of the 2 was negative
    arg1 = neg(arg1);
#else
  decode_2_int_args ();
  arg1 = encode_int (a1 * a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_div (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  if (obj_eq(arg2, ZERO))
    ERROR("quotient", "divide by 0");
  a1 = negp (arg1);
  a2 = negp (arg2); // -1 if negative
  arg1 = divnonneg (a1 ? neg(arg1) : arg1,
		    a2 ? neg(arg2) : arg2);
  if (a1 + a2 == 1) // only one of the 2 was negative
    arg1 = neg(arg1);
#else
  decode_2_int_args ();
  if (a2 == 0)
    ERROR("quotient", "divide by 0");
  arg1 = encode_int (a1 / a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_rem (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  if (obj_eq(arg2, ZERO))
    ERROR("remainder", "divide by 0");
  if (negp(arg1) || negp(arg2))
    ERROR("remainder", "only positive numbers are supported");
  // TODO fix this to handle negatives
  // TODO logic quite similar to mul and div (likely, once we fix), abstract ?
  arg3 = divnonneg (arg1, arg2);
  arg4 = mulnonneg (arg2, arg3);
  arg1 = sub(arg1, arg4 );
  arg3 = OBJ_FALSE;
  arg4 = OBJ_FALSE;
#else
  decode_2_int_args ();
  if (a2 == 0)
    ERROR("remainder", "divide by 0");
  arg1 = encode_int (a1 % a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_neg (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = neg (arg1);
#else
  a1 = decode_int (arg1);
  arg1 = encode_int (- a1);
#endif
}

void prim_eq (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = encode_bool(cmp (arg1, arg2) == 0);
#else
  decode_2_int_args ();
  arg1 = encode_bool(a1 == a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_lt (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = encode_bool(cmp (arg1, arg2) < 0);
#else
  decode_2_int_args ();
  arg1 = encode_bool(a1 < a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_gt (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = encode_bool(cmp (arg1, arg2) > 0);
#else
  decode_2_int_args ();
  arg1 = encode_bool(a1 > a2);
#endif
  arg2 = OBJ_FALSE;
}

prim_leq (void) { // TODO these 2 are useful, but they add to the code size, is it worth it ?
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = encode_bool(cmp (arg1, arg2) <= 0);
#else
  decode_2_int_args ();
  arg1 = encode_bool(a1 <= a2);
#endif
  arg2 = OBJ_FALSE;
  
}

void prim_geq (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = encode_bool(cmp (arg1, arg2) >= 0);
#else
  decode_2_int_args ();
  arg1 = encode_bool(a1 >= a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_ior (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = bitwise_ior(arg1, arg2);
#else
  decode_2_int_args (); // TODO is the function call overhead worth it ?
  arg1 = encode_int (a1 | a2);
#endif
  arg2 = OBJ_FALSE;
}

void prim_xor (void) {
#ifdef INFINITE_PRECISION_BIGNUMS
  arg1 = bitwise_xor(arg1, arg2);
#else
  decode_2_int_args (); // TODO is the function call overhead worth it ?
  arg1 = encode_int (a1 ^ a2);
#endif
  arg2 = OBJ_FALSE;
}

// TODO primitives de shift ?

/*---------------------------------------------------------------------------*/

// list primitives

void prim_pairp (void) {
  if (IN_RAM(arg1))
    arg1 = encode_bool (RAM_PAIR(arg1));
  else if (IN_ROM(arg1))
    arg1 = encode_bool (ROM_PAIR(arg1));
  else
    arg1 = OBJ_FALSE;
}

obj cons (obj car, obj cdr) {
  return alloc_ram_cell_init (COMPOSITE_FIELD0 | (car >> 8),
			      car & 0xff,
			      PAIR_FIELD2 | (cdr >> 8),
			      cdr & 0xff);
}

void prim_cons (void) {
  arg1 = cons (arg1, arg2);
  arg2 = OBJ_FALSE;
}

void prim_car (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_PAIR(arg1))
      TYPE_ERROR("car.0", "pair");
    arg1 = ram_get_car (arg1);
  }
  else if (IN_ROM(arg1)) {
    if (!ROM_PAIR(arg1))
      TYPE_ERROR("car.1", "pair");
    arg1 = rom_get_car (arg1);
  }
  else
    TYPE_ERROR("car.2", "pair");
}

void prim_cdr (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_PAIR(arg1))
      TYPE_ERROR("cdr.0", "pair");
    arg1 = ram_get_cdr (arg1);
  }
  else if (IN_ROM(arg1)) {
    if (!ROM_PAIR(arg1))
      TYPE_ERROR("cdr.1", "pair");
    arg1 = rom_get_cdr (arg1);
  }
  else
    TYPE_ERROR("cdr.2", "pair");
}

void prim_set_car (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_PAIR(arg1))
      TYPE_ERROR("set-car!.0", "pair");
    
    ram_set_car (arg1, arg2);
    arg1 = OBJ_FALSE;
    arg2 = OBJ_FALSE;
  }
  else
    TYPE_ERROR("set-car!.1", "pair");
}

void prim_set_cdr (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_PAIR(arg1))
      TYPE_ERROR("set-cdr!.0", "pair");
    
    ram_set_cdr (arg1, arg2);
    arg1 = OBJ_FALSE;
    arg2 = OBJ_FALSE;
  }
  else
    TYPE_ERROR("set-cdr!.1", "pair");
}

void prim_nullp (void) {
  arg1 = encode_bool (arg1 == OBJ_NULL);
}

/*---------------------------------------------------------------------------*/

// vector primitives

void prim_u8vectorp (void) {
  if (IN_RAM(arg1))
    arg1 = encode_bool (RAM_VECTOR(arg1));
  else if (IN_ROM(arg1))
    arg1 = encode_bool (ROM_VECTOR(arg1));
  else
    arg1 = OBJ_FALSE;
}

void prim_make_u8vector (void) {
  decode_2_int_args (); // arg1 is length, arg2 is contents
  // TODO adapt for the new bignums
  if (a2 > 255)
    ERROR("make-u8vector", "byte vectors can only contain bytes");
  
  arg3 = alloc_vec_cell (a1);
  arg1 = alloc_ram_cell_init (COMPOSITE_FIELD0 | (a1 >> 8),
			      a1 & 0xff,
			      VECTOR_FIELD2 | (arg3 >> 8),
			      arg3 & 0xff);

  a1 = (a1 + 3) / 4; // actual length, in words
  while (a1--) {
    ram_set_field0 (arg3, a2);
    ram_set_field1 (arg3, a2);
    ram_set_field2 (arg3, a2);
    ram_set_field3 (arg3, a2);
    arg3++;
  }
}

void prim_u8vector_ref (void) {
  a2 = decode_int (arg2);
  // TODO adapt for the new bignums
  if (IN_RAM(arg1)) {
    if (!RAM_VECTOR(arg1))
      TYPE_ERROR("u8vector-ref.0", "vector");
    if ((ram_get_car (arg1) <= a2) || (a2 < 0))
      ERROR("u8vector-ref.0", "vector index invalid");
    arg1 = ram_get_cdr (arg1);
  }
  else if (IN_ROM(arg1)) {
    if (!ROM_VECTOR(arg1))
      TYPE_ERROR("u8vector-ref.1", "vector");
    if ((rom_get_car (arg1) <= a2) || (a2 < 0))
      ERROR("u8vector-ref.1", "vector index invalid");
    arg1 = rom_get_cdr (arg1);
  }
  else
    TYPE_ERROR("u8vector-ref.2", "vector");

  if (IN_VEC(arg1)) {
    arg1 += (a2 / 4);
    a2 %= 4;
    
    arg1 = encode_int (ram_get_fieldn (arg1, a2));
  }
  else { // rom vector, stored as a list
    while (a2--)
      arg1 = rom_get_cdr (arg1);
    
    // the contents are already encoded as fixnums
    arg1 = rom_get_car (arg1);
  }

  arg2 = OBJ_FALSE;
  arg3 = OBJ_FALSE;
  arg4 = OBJ_FALSE;
}

void prim_u8vector_set (void) { // TODO a lot in common with ref, abstract that
  a2 = decode_int (arg2); // TODO adapt for bignums
  a3 = decode_int (arg3);

  if (a3 > 255)
    ERROR("u8vector-set!", "byte vectors can only contain bytes");
  
  if (IN_RAM(arg1)) {
    if (!RAM_VECTOR(arg1))
      TYPE_ERROR("u8vector-set!.0", "vector");
    if ((ram_get_car (arg1) <= a2) || (a2 < 0))
      ERROR("u8vector-set!", "vector index invalid");
    arg1 = ram_get_cdr (arg1);
  }
  else
    TYPE_ERROR("u8vector-set!.1", "vector");
  
  arg1 += (a2 / 4);
  a2 %= 4;
  
  ram_set_fieldn (arg1, a2, a3);
  
  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
  arg3 = OBJ_FALSE;
}

void prim_u8vector_length (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_VECTOR(arg1))
      TYPE_ERROR("u8vector-length.0", "vector");
    arg1 = encode_int (ram_get_car (arg1));
  }
  else if (IN_ROM(arg1)) {
    if (!ROM_VECTOR(arg1))
      TYPE_ERROR("u8vector-length.1", "vector");
    arg1 = encode_int (rom_get_car (arg1));
  }
  else
    TYPE_ERROR("u8vector-length.2", "vector");
}

void prim_u8vector_copy (void) {
  // arg1 is source, arg2 is source-start, arg3 is target, arg4 is target-start
  // arg5 is number of bytes to copy
  
  a1 = decode_int (arg2);
  a2 = decode_int (arg4);
  a3 = decode_int (arg5);
  
  // case 1 : ram to ram
  if (IN_RAM(arg1) && IN_RAM(arg3)) {
    if (!RAM_VECTOR(arg1) || !RAM_VECTOR(arg3))
      TYPE_ERROR("u8vector-copy!.0", "vector");
    if ((ram_get_car (arg1) < (a1 + a3)) || (a1 < 0) ||
	(ram_get_car (arg3) < (a2 + a3)) || (a2 < 0))
      ERROR("u8vector-copy!.0", "vector index invalid");
    
    // position to the start
    arg1 = ram_get_cdr (arg1);
    arg1 += (a1 / 4);
    a1 %= 4;
    arg3 = ram_get_cdr (arg3);
    arg3 += (a2 / 4);
    a2 %= 4;
    
    // copy
    while (a3--) {
      ram_set_fieldn (arg3, a2, ram_get_fieldn (arg1, a1));
      
      a1++;
      arg1 += (a1 / 4);
      a1 %= 4; // TODO merge with the previous similar block ?
      a2++;
      arg3 += (a2 / 4);
      a2 %= 4;
    }
  }
  // case 2 : rom to ram
  else if (IN_ROM(arg1) && IN_RAM(arg3)) {
    if (!ROM_VECTOR(arg1) || !RAM_VECTOR(arg3))
      TYPE_ERROR("u8vector-copy!.1", "vector");
    if ((rom_get_car (arg1) < (a1 + a3)) || (a1 < 0) ||
	(ram_get_car (arg3) < (a2 + a3)) || (a2 < 0))
      ERROR("u8vector-copy!.1", "vector index invalid");
    
    arg1 = rom_get_cdr (arg1);
    while (a1--)
      arg1 = rom_get_cdr (arg1);
    
    arg3 = ram_get_cdr (arg3);
    arg3 += (a2 / 4);
    a2 %= 4;
    
    while (a3--) {
      ram_set_fieldn (arg3, a2, decode_int (rom_get_car (arg1)));
      
      arg1 = rom_get_cdr (arg1);
      a2++;
      arg3 += (a2 / 4);
      a2 %= 4; // TODO very similar to the other case
    }
  }
  else
    TYPE_ERROR("u8vector-copy!.2", "vector");
  
  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
  arg3 = OBJ_FALSE;
  arg4 = OBJ_FALSE;
  arg5 = OBJ_FALSE;
}

/*---------------------------------------------------------------------------*/

// miscellaneous primitives

void prim_eqp (void) {
  arg1 = encode_bool (arg1 == arg2);
  arg2 = OBJ_FALSE;
}

void prim_not (void) {
  arg1 = encode_bool (arg1 == OBJ_FALSE);
}

void prim_symbolp (void) {
  if (IN_RAM(arg1))
    arg1 = encode_bool (RAM_SYMBOL(arg1));
  else if (IN_ROM(arg1))
    arg1 = encode_bool (ROM_SYMBOL(arg1));
  else
    arg1 = OBJ_FALSE;
}

void prim_stringp (void) {
  if (IN_RAM(arg1))
    arg1 = encode_bool (RAM_STRING(arg1));
  else if (IN_ROM(arg1))
    arg1 = encode_bool (ROM_STRING(arg1));
  else
    arg1 = OBJ_FALSE;
}

void prim_string2list (void) {
  if (IN_RAM(arg1)) {
    if (!RAM_STRING(arg1))
      TYPE_ERROR("string->list.0", "string");
    
    arg1 = ram_get_car (arg1);
  }
  else if (IN_ROM(arg1)) {
    if (!ROM_STRING(arg1))
      TYPE_ERROR("string->list.1", "string");
      
    arg1 = rom_get_car (arg1);
    }
  else
    TYPE_ERROR("string->list.2", "string");
}

void prim_list2string (void) {
  arg1 = alloc_ram_cell_init (COMPOSITE_FIELD0 | ((arg1 & 0x1f00) >> 8),
			      arg1 & 0xff,
			      STRING_FIELD2,
			      0);
}

void prim_booleanp (void) {
  arg1 = encode_bool (arg1 < 2);
}

/*---------------------------------------------------------------------------*/

// robot-specific primitives

#ifdef WORKSTATION

void show (obj o) {
#if 0
  printf ("[%d]", o);
#endif

  if (o == OBJ_FALSE)
    printf ("#f");
  else if (o == OBJ_TRUE)
    printf ("#t");
  else if (o == OBJ_NULL)
    printf ("()");
  else if (o <= (MIN_FIXNUM_ENCODING + (MAX_FIXNUM - MIN_FIXNUM)))
    printf ("%d", DECODE_FIXNUM(o));
  else {
    uint8 in_ram;
    
    if (IN_RAM(o))
      in_ram = 1;
    else
      in_ram = 0;
    
    if ((in_ram && RAM_BIGNUM(o)) || (!in_ram && ROM_BIGNUM(o))) // TODO fix for new bignums
      printf ("%d", decode_int (o));
    else if ((in_ram && RAM_COMPOSITE(o)) || (!in_ram && ROM_COMPOSITE(o))) {
      obj car;
      obj cdr;
      
      if ((in_ram && RAM_PAIR(o)) || (!in_ram && ROM_PAIR(o))) {	      
	if (in_ram) {
	  car = ram_get_car (o);
	  cdr = ram_get_cdr (o);
	}
	else {
	  car = rom_get_car (o);
	  cdr = rom_get_cdr (o);
	}
	
	printf ("(");
	
      loop:
	
	show (car);
	
	if (cdr == OBJ_NULL)
	  printf (")");
	else if ((IN_RAM(cdr) && RAM_PAIR(cdr))
		 || (IN_ROM(cdr) && ROM_PAIR(cdr))) {
	  if (IN_RAM(cdr)) {
	    car = ram_get_car (cdr);
	    cdr = ram_get_cdr (cdr);
	  }
	  else {
	    car = rom_get_car (cdr);
	    cdr = rom_get_cdr (cdr);
	  }
	  
	  printf (" ");
	  goto loop;
	}
	else {
	  printf (" . ");
	  show (cdr);
	  printf (")");
	}
      }
      else if ((in_ram && RAM_SYMBOL(o)) || (!in_ram && ROM_SYMBOL(o)))
	printf ("#<symbol>");
      else if ((in_ram && RAM_STRING(o)) || (!in_ram && ROM_STRING(o)))
	printf ("#<string>");
      else if ((in_ram && RAM_VECTOR(o)) || (!in_ram && ROM_VECTOR(o)))
	printf ("#<vector %d>", o);
      else {
	printf ("(");
	car = ram_get_car (o);
	cdr = ram_get_cdr (o);
	// ugly hack, takes advantage of the fact that pairs and
	// continuations have the same layout
	goto loop;
      }
    }
    else { // closure
      obj env;
      rom_addr pc;
      
      if (IN_RAM(o))
	env = ram_get_cdr (o);
      else
	env = rom_get_cdr (o);
      
      if (IN_RAM(o))
	pc = ram_get_entry (o);
      else
	pc = rom_get_entry (o);
      
      printf ("{0x%04x ", pc);
      show (env);
      printf ("}");
    }
  }
  
  fflush (stdout);
}

void print (obj o) {
  show (o);
  printf ("\n");
  fflush (stdout);
}

#endif

void prim_print (void) {
#ifdef WORKSTATION
  print (arg1);
#endif

  arg1 = OBJ_FALSE;
}

int32 read_clock (void) {
  int32 now = 0;

#ifdef PICOBOARD2
  now = from_now( 0 );
#endif

#ifdef WORKSTATION
#ifdef _WIN32
  static int32 start = 0;
  struct timeb tb;
  ftime (&tb);
  now = tb.time * 1000 + tb.millitm;
  if (start == 0)
    start = now;
  now -= start;
#else
  static int32 start = 0;
  struct timeval tv;
  if (gettimeofday (&tv, NULL) == 0) {
    now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    if (start == 0)
      start = now;
    now -= start;
  }
#endif
#endif

  return now;
}

void prim_clock (void) {
  arg1 = encode_int (read_clock ());
}

void prim_motor (void) {
  decode_2_int_args ();

  if (a1 < 1 || a1 > 2 || a2 < -100 || a2 > 100)
    ERROR("motor", "argument out of range");

#ifdef PICOBOARD2
  MOTOR_set( a1, a2 );
#endif

#ifdef WORKSTATION
  printf ("motor %d -> power=%d\n", a1, a2);
  fflush (stdout);
#endif
  
  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
}


void prim_led (void) {
  decode_2_int_args ();
  a3 = decode_int (arg3);
  
  if (a1 < 1 || a1 > 3 || a2 < 0 || a3 < 0)
    ERROR("led", "argument out of range");

#ifdef PICOBOARD2
  LED_set( a1, a2, a3 );
#endif

#ifdef WORKSTATION
  printf ("led %d -> duty=%d period=%d\n", a1, a2, a3 );
  fflush (stdout);
#endif
  
  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
  arg3 = OBJ_FALSE;
}


void prim_led2_color (void) {
  a1 = decode_int (arg1);

  if (a1 < 0 || a1 > 1)
    ERROR("led2-colors", "argument out of range");

#ifdef PICOBOARD2
  LED2_color_set( a1 );
#endif

#ifdef WORKSTATION
  printf ("led2-color -> %s\n", (a1==0)?"green":"red");
  fflush (stdout);
#endif

  arg1 = OBJ_FALSE;
}


void prim_getchar_wait (void) {
  decode_2_int_args();
  a1 = read_clock () + a1;

  if (a1 < 0 || a2 < 1 || a2 > 3)
    ERROR("getchar-wait", "argument out of range");

#ifdef PICOBOARD2
  arg1 = OBJ_FALSE;
  {
    serial_port_set ports;
    ports = serial_rx_wait_with_timeout( a2, a1 );
    if (ports != 0)
      arg1 = encode_int (serial_rx_read( ports ));
  }
#endif

#ifdef WORKSTATION
#ifdef _WIN32
  arg1 = OBJ_FALSE;
  do {
    if (_kbhit ())  {
      arg1 = encode_int (_getch ());
      break;
    }
  } while (read_clock () < a1);
#else
  arg1 = encode_int (getchar ());
#endif
#endif
}


void prim_putchar (void) {
  decode_2_int_args ();

  if (a1 < 0 || a1 > 255 || a2 < 1 || a2 > 3)
    ERROR("putchar", "argument out of range");

#ifdef PICOBOARD2
  serial_tx_write( a2, a1 );
#endif

#ifdef WORKSTATION
  putchar (a1);
  fflush (stdout);
#endif

  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
}


void prim_beep (void) {
  decode_2_int_args ();

  if (a1 < 1 || a1 > 255 || a2 < 0)
    ERROR("beep", "argument out of range");
  
#ifdef PICOBOARD2
  beep( a1, from_now( a2 ) );
#endif

#ifdef WORKSTATION
  printf ("beep -> freq-div=%d duration=%d\n", a1, a2 );
  fflush (stdout);
#endif

  arg1 = OBJ_FALSE;
  arg2 = OBJ_FALSE;
}


void prim_adc (void) {
  short x;

  a1 = decode_int (arg1);

  if (a1 < 1 || a1 > 3)
    ERROR("adc", "argument out of range");

#ifdef PICOBOARD2
  x = adc( a1 );
#endif

#ifdef WORKSTATION
  x = read_clock () & 255;
  if (x > 127) x = 256 - x;
  x += 200;
#endif

  arg1 = encode_int (x);
}

void prim_sernum (void) {
  short x;

#ifdef PICOBOARD2
  x = serial_num ();
#endif

#ifdef WORKSTATION
  x = 0;
#endif

  arg1 = encode_int (x);
}

/*---------------------------------------------------------------------------*/

// networking primitives

void prim_network_init (void) { // TODO maybe put in the initialization of the vm
#ifdef WORKSTATION
  handle = pcap_open_live(INTERFACE, MAX_PACKET_SIZE, PROMISC, TO_MSEC, errbuf);
  if (handle == NULL)
    ERROR("network-init", "interface not responding");
#endif
}

void prim_network_cleanup (void) { // TODO maybe put in halt ?
#ifdef WORKSTATION
  pcap_close(handle);
#endif
}

void prim_receive_packet_to_u8vector (void) {
  // arg1 is the vector in which to put the received packet
  if (!RAM_VECTOR(arg1))
    TYPE_ERROR("receive-packet-to-u8vector", "vector");

#ifdef WORKSTATION
  // receive the packet in the buffer
  struct pcap_pkthdr header;
  const u_char *packet;

  packet = pcap_next(handle, &header);

  if (packet == NULL)
    header.len = 0;

  if (ram_get_car (arg1) < header.len)
    ERROR("receive-packet-to-u8vector", "packet longer than vector");
    
  if (header.len > 0) { // we have received a packet, write it in the vector
    arg2 = rom_get_cdr (arg1);
    arg1 = header.len; // we return the length of the received packet
    a1 = 0;
    
    while (a1 < arg1) {
      ram_set_fieldn (arg2, a1 % 4, (char)packet[a1]);
      a1++;
      arg2 += (a1 % 4) ? 0 : 1;
    }
    
    arg2 = OBJ_FALSE;
  }
  else // no packet to be read
    arg1 = OBJ_FALSE;
#endif
}

void prim_send_packet_from_u8vector (void) {
  // arg1 is the vector which contains the packet to be sent
  // arg2 is the length of the packet
  // TODO only works with ram vectors for now
  if (!RAM_VECTOR(arg1))
    TYPE_ERROR("send-packet-from-vector!", "vector");

  a2 = decode_int (arg2); // TODO fix for bignums
  a1 = 0; 
  
  // TODO test if the length of the packet is longer than the length of the vector
  if (ram_get_car (arg1) < a2)
    ERROR("send-packet-from-u8vector", "packet cannot be longer than vector");

  arg1 = ram_get_cdr (arg1);

#ifdef WORKSTATION
  // copy the packet to the output buffer
  while (a1 < a2)
    buf[a1] = ram_get_fieldn (arg1, a1 % 4);
  // TODO maybe I could just give pcap the pointer to the memory

  if (pcap_sendpacket(handle, buf, a2) < 0) // TODO an error has occurred, can we reuse the interface ?
    arg1 = OBJ_FALSE;
  else
    arg1 = OBJ_TRUE;
#endif

  arg2 = OBJ_FALSE;
}