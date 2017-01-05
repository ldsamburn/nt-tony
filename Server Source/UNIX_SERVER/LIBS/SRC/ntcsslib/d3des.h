/* d3des.h -
 *
 *	Headers and defines for d3des.c
 *	Graven Imagery, 1992.
 *
 * Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge
 *	(GEnie : OUTER; CIS : [71755,204])
 *
 * At the end of this file, there are some examples for testing,
 * as well as some e-mail correspondance from the author clarifying
 * the use of these functions.
*/

#define D2_DES		/* include double-length support */

#define D3_DES      	/* include triple-length support */


#ifdef D3_DES
#ifndef D2_DES
#define D2_DES		/* D2_DES is needed for D3_DES */
#endif
#endif
	
#define EN0	0	/* MODE == encrypt */
#define DE1	1	/* MODE == decrypt */

/* A useful alias on 68000-ish machines, but NOT USED HERE. */
typedef union {
	unsigned long blok[2];
	unsigned short word[4];
	unsigned char byte[8];
	} M68K;


extern void deskey(  unsigned char *, short  );
/*		      hexkey[8]     MODE
 * Sets the internal key register according to the hexadecimal
 * key contained in the 8 bytes of hexkey, according to the DES,
 * for encryption or decryption according to MODE.
 */


extern void usekey(  unsigned long *  );
/*		    cookedkey[32]
 * Loads the internal key register with the data in cookedkey.
 */

extern void cpkey(  unsigned long *  );
/*		   cookedkey[32]
 * Copies the contents of the internal key register into the storage
 * located at &cookedkey[0].
 */

extern void des(  unsigned char *, unsigned char *  );
/*		    from[8]	      to[8]
 * Encrypts/Decrypts (according to the key currently loaded in the
 * internal key register) one block of eight bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

#ifdef D2_DES
#define desDkey(a,b)	des2key((a),(b))

extern void des2key(  unsigned char *, short  );
/*		      hexkey[16]     MODE
 * Sets the internal key registerS according to the hexadecimal
 * keyS contained in the 16 bytes of hexkey, according to the DES,
 * for DOUBLE encryption or decryption according to MODE.
 * NOTE: this clobbers all three key registers!
 */

extern void Ddes(  unsigned char *, unsigned char *  );
/*		    from[8]	      to[8]
 * Encrypts/Decrypts (according to the keyS currently loaded in the
 * internal key registerS) one block of eight bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

extern void D2des(  unsigned char *, unsigned char *  );
/*		    from[16]	      to[16]
 * Encrypts/Decrypts (according to the keyS currently loaded in the
 * internal key registerS) one block of SIXTEEN bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

extern void makekey(  char *, unsigned char *  );
/*		*password,	single-length key[8]
 * With a double-length default key, this routine hashes a NULL-terminated
 * string into an eight-byte random-looking key, suitable for use with the
 * deskey() routine.
 */

#define makeDkey(a,b)	make2key((a),(b))

extern void make2key(  char *, unsigned char *  );
/*		*password,	double-length key[16]
 * With a double-length default key, this routine hashes a NULL-terminated
 * string into a sixteen-byte random-looking key, suitable for use with the
 * des2key() routine.
 */

#ifndef D3_DES	/* D2_DES only */
#define useDkey(a)	use2key((a))
#define cpDkey(a)	cp2key((a))

extern void use2key(  unsigned long *  );
/*		    cookedkey[64]
 * Loads the internal key registerS with the data in cookedkey.
 * NOTE: this clobbers all three key registers!
 */

extern void cp2key(  unsigned long *  );
/*		   cookedkey[64]
 * Copies the contents of the internal key registerS into the storage
 * located at &cookedkey[0].
 */

#else	/* D3_DES too */
#define useDkey(a)	use3key((a))
#define cpDkey(a)	cp3key((a))

extern void des3key(  unsigned char *, short );
/*		      hexkey[24]     MODE
 * Sets the internal key registerS according to the hexadecimal
 * keyS contained in the 24 bytes of hexkey, according to the DES,
 * for DOUBLE encryption or decryption according to MODE.
 */

extern void use3key(  unsigned long * );
/*		    cookedkey[96]
 * Loads the 3 internal key registerS with the data in cookedkey.
 */

extern void cp3key(  unsigned long *  );
/*		   cookedkey[96]
 * Copies the contents of the 3 internal key registerS into the storage
 * located at &cookedkey[0].
 */

extern void make3key(  const char *, unsigned char *  );
/*		*password,	triple-length key[24]
 * With a triple-length default key, this routine hashes a NULL-terminated
 * string into a twenty-four-byte random-looking key, suitable for use with
 * the des3key() routine.
 */

#endif	/* D3_DES */
#endif	/* D2_DES */

/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
 ********************************************************************/
/* Begin e-mail correspondance:

X-Sender: outer@mail.interlog.com
Date: Tue, 3 Jun 1997 20:14:52 -0400
To: byers@cs.odu.edu
From: Richard Outerbridge <outer@interlog.com>
Subject: Re: Your triple DES algorithm set

-----BEGIN PGP SIGNED MESSAGE-----

1997/06/03 19:47:07 EDST
Denton -

>	I obtained you code for implementing triple DES encryption (v5.09)
>on a related web site recently.   In the absence of a FAQ or readme, I got
>confused as to the order in which your functions are called.  I assume
>that you have to call make3key() to create a suitable triple key, then
>call des3key() with that key to set the internal key registers.  Then I
>assume you call d3des() with the data to be encrypted and the buffer
>to put the result in.

>This is the order I called the procedures in (with the exception of
>make3key() ) when I was trying to validate that everything worked
>according to your validation set for "Triple-length key, double-length
>plaintext."
>
>Is this the correct order?

Yes.  You've guessed it right.  I gather something is wrong?

Try this: first, get single DES working.  Then triple DES is easy.
DES has the property that a key of all zeroes is self-inverse.  That
is, encrypting twice with the same key will give you back plaintext.
This works for a key of all ones as well.  Let me know what you find.

Also, be aware that if the source you came by came from the >1st< edition
of Applied Cryptography as printed it contains a number of typos.  If in
doubt, send me your source, and I'll be happy to debug it for you.

>Also, I am assuming that you don't have to call cp3key(), and then
>use3key() after each successive iteration of d3des() to en/de-crypt using
>the same key.  I noticed that some of your other functions "clobber" the
>key registers but didn't read anything to this regard bout the triple DES
>functions.

Right again.

The software 'emulates' a chip which maintains a single active key at
any one point in time.  The cpXkey() and useXkey() functions pierce this
polite fiction by allowing software implementations to maintain a stack
of active keys without having to recalculate the effective key schedules
each time.  The clobbering warnings advise against side-effects.  They
also permit you to use non-standard key schedules of your own devising
by (with some puzzling out of the internal representation) setting the
768 bits of each single-DES directly.

Incidentally - the 'double-block' modes have been broken: they do not
deliver the 128-bit block, 112-bit keyspace they seem to.  With two keys
the effective brute-force keyspace can be reduced to 2**57; with three,
to 2**112.  In other words they don't achieve much more than single-block
DES does.

Note the new email address.  Thanks for your questions.

outer

-----BEGIN PGP SIGNATURE-----
Version: 2.6.2, by FileCrypt 1.0

iQCVAwUBM5SzdtNcQg4O6q8hAQEZ0QP8DHE7Kz9kaPkKzd/7HpsgfW0UpYVwgerW
31mFPi5TdS1E4SZCEFJU2mxb6wyyyjCrOTVT3dYwKVa0Lp/KvdzkNCDU/nsn44JD
c2vFskOkiWmEysbIn1xmYwoBJifqnNJgQKQGdSrj0AIOHjMdqU4lthsqA0JIx/aQ
7R05qnqUmog=
=awwY
-----END PGP SIGNATURE-----

<outer@interlog.com> :
"Just an eccentric soul with a curiosity for the bizarre."

X-Sender: outer@mail.interlog.com
Date: Thu, 19 Jun 1997 17:55:40 -0400
To: byers@cs.odu.edu
From: Richard Outerbridge <outer@interlog.com>
Subject: des

1997/06/19 17:44:30 EDST
Denton - finally back on the air!!!!

>Also, there is a test vector for triple length key and single length
>plaintext.  This implies that des() should be called for encryption since
>it is the only function that takes a single block of data,

Not true!  What about Ddes()?  Try this and see if it works!

The cryptic function naming explains all this.  The numbers refer to
the size of the arguments.  Thus Ddes() = 1 block, D2des() = 2 blocks.
des2key() = 2 keys, des3key() = 3 keys.  It's not entirely consistent.
Both Ddes() and D2des() use three iterations of DES, as opposed to
des() which only uses one, and only one key.

outer

*****/

/**** Triple-length key, single-length plaintext */
/* This is the same as double-length key, single-length plaintext
 * except the 1st and third keys are not the same... as originally
 * coded it should have been the same as single-length key,
 * single-length plaintext - the second and third keys would not
 * have been used at all.  It wouldn't have decrypted though.

  des3key(&Key, EN0);

  des(&Plain,&Cipher);
  Ddes(&Plain,&Cipher);
   
  printf(" de 0b 7c 06 ae 5e 0e d5\n");
  for (a=0;a<8;a++)
    printf(" %x", Cipher[a]);
  printf("\n");

****/

/**** for triple-key, double-plaintext */
/* Actually, this does triple-key, triple-plaintext.
 * Don't use this.  As the comments suggest ("Amateur theatrics")
 * it's not of much use for anything but password-hashing.

  des3key(&Key, EN0);
  D3des(&Plain,&Cipher);
  printf(" ad 0d 1b 30 ac 17 cf 07 0e d1 1c 63 81 e4 4d e5\n");
  for (a=0;a<16;a++)
    printf(" %x", Cipher[a]);
  printf("\n");
*****/

/* Instead, try this: */
/**** for triple-key, double-plaintext

  des3key(&Key, EN0);
  D2des(&Plain,&Cipher);
  printf(" ad 0d 1b 30 ac 17 cf 07 0e d1 1c 63 81 e4 4d e5\n");
  for (a=0;a<16;a++)
    printf(" %x", Cipher[a]);
  printf("\n");

<outer@interlog.com> :
"Just an eccentric soul with a curiosity for the bizarre."
***/

/* Refer to the d3des.c code for the testing routines outlined above. */
