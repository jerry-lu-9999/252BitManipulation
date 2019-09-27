#include <stdio.h>
#include <stdint.h>

/*
   Multibyte encodings.

   In ASCII, a standard for encoding mostly English text data, each
   character (e.g. A to Z) occupies one byte. However, using one byte
   per character does not work for languages which may have a large
   number of characters.

   Such languages use a multibyte encoding where one character can
   occupy more than one byte. Many such multibyte encodings exist. One
   such encoding for a hypothetical language is described below:

   Assume that there are 1114112 characters of this language, so we
   can use 21 bits (so characters range from 0 to 1114111).

   Then,

   For values from 0 to 127 (which take 7 bits), the encoding is:

   0xxx xxxx - where the xxx xxxx bit indicate the character.

   e.g. 65 (0100 0001) => 0100 0001

   Thus, values from 0 to 127 consume only one byte.


   For values from 128 to 2047 (which take 11 bits), use two bytes:

   110x xxxx 10xx xxxx

   e.g. 128 (000 1000 0000) => 1100 0010 1000 0000
                                      ^^   ^^ ^^^^

   For values from 2048 to 65535 (which take 16 bits), use three bytes

   1110 xxxx  10xx xxxx  10xx xxxx


   For values from 65535 to 1114111 (which take 21 bits), use four bytes

   1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx

   This is a variable-length multibyte encoding, where each character
   can occupy 1 to 4 bytes.  In such encodings, a one-to-one relation
   between a character and a byte does not exist. Functions that
   operate on multibyte strings have to be rewritten.

   In this assignment, you will write functions to encode, decode, and
   find the length of strings.
*/



/*
   Input mbstr is an array containing bytes, its length (in bytes) is nbytes.

   Compute and return the number of characters of the multibyte string.
 */
size_t mbstr_length(uint8_t mbstr[], size_t nbytes) {

  /*
	 Note that in the encoding above, tarts wa character shen the byte
	 contains a 0 bit in the 8th position (counting from 1), or if the
	 byte contains two or more high bits that are 1 followed by a 0.

	 Bytes that have one high bit that is one followed by a zero
	 (i.e. 10xx xxxx) are actually "inside" a multibyte character.
  */


  /* One strategy is to look at each byte, identify if it is the start of
	 a character, and increase the length if so. */
  int length = 0;
  int i = 0;
  while(i < nbytes){
     if((mbstr[i]>>7 == 0x00)){
        length++;
        i++;
     } else if((mbstr[i]>>5) == 0x06){
                if((mbstr[i+1]>>6) == 0x02){
                   length++;
                   i = i + 2;
                } else{
                   i = i + 2;
                }
     } else if((mbstr[i]>>4)==0x0e){
                if(((mbstr[i+1]>>6) == 0x02) && (mbstr[i+2]>>6) == 0x02){
                   length++;
                   i = i + 3;
                } else{
                  i = i + 3;
                }
     } else if((mbstr[i]>>3) == 0x1e){
                if (((mbstr[i+1]>>6) == 0x02) && (mbstr[i+2]>>6) == 0x02 && (mbstr[i+3]>>6) == 0x02){
                   length++;
                   i = i + 4;
                } else{
                   i = i + 4;
                }
     }
  }
  return length;

 /* change this to return the actual length*/
}



/*

  Encode characters from 0 to 1114111 (decimal) in mbstr. Assume mbstr
  is an array at least 4 bytes long.

  Return the number of bytes in the encoded value (1 to 4).

  Return 0 if c is outside the valid range of characters.

 */

int encode(uint32_t c, uint8_t mbstr[]) {
    if(c < 0 || c > 1114111){
      return 0;
    } else if(c >= 0 && c <= 127){
      mbstr[0] = c | 0x00;
      return 1;
    } else if(c >= 128 && c <= 2047){
      mbstr[0] = c>>6 | 0xc0;
      mbstr[1] = (c&0x3f) | 0x80;
      return 2;
    } else if(c >= 2048 && c <= 65535){
      mbstr[0] = c>>12 | 0xe0;
      mbstr[1] = (c&0xc0)>>6 | 0x80 | (0x0f00&c)>>6;
      mbstr[2] = (c&0x3f) | 0x80;
      return 3;
    } else if(c >= 65536 && c <= 1114111){
      mbstr[0] = (c&0x1c0000)>>18| 0xf0;
      mbstr[1] = (c&0x0f000)>>12| ((c&0x30000)>>12)|0x80;
      mbstr[2] = (c&0xc0)>>6 | 0x80 | (0x0f00&c)>>6;
      mbstr[3] = (c&0x3f) | 0x80;
      return 4;
    }
    return 0;
}



/*

  Decode the character in mbstr, which is an array of 4 bytes
  (always).

  Return a 32-bit unsigned integer such that:

  bits 0--21 are the decoded character (0--1114111)

  bits 22--24 are the number of bytes decoded (0--4)

  Set bits 0--24 to zero if you encounter an incorrectly encoded
  character.

  Examples:

  #1:

  mbstr = {0x41, 0x42, 0x43, 0x44}

  return value should be (binary)
      0000 0000 0100 0000 0000 0000 0100 0001

      (i.e. 0x400041)

  (the only value decoded was 0x41 which occupies one byte)

  #2:

  mbstr = {0xc2, 0x80, 0x41, 0x42}

  return value should be (binary):
      0000 0000 1000 0000 0000 0000 1000 0000

      (i.e. 0x80080)

  (the value decoded was 0x80 which occupies two bytes)


  #3:

  mbstr = {0xf8, ?, ?, ?}  // ? indicates values are unimportant

  Note: 0xf8 is 1111 1000 which is not a valid encoding.

  Return 0


  #4:

  mbstr = {0xc1, 0xc0, 0x41, 0x42}

  Note: 0xc0 (1100 0000) cannot follow 0xc1 (1100 0001), so this is an
  invalid encoding.

  Return 0.

  #5:

  mbstr = {0xc0, 0x41, 0x41, 0x42}

  Note: 0xc0 (1100 0000) cannot start a character, so this is an also
  an invalid encoding.

*/

uint32_t decode(uint8_t mbstr[]) {
  uint32_t ans = 0x00000000;
  if((mbstr[0]>>7) == 0x00){
     ans = ans | 0x00400000 | mbstr[0];
  } 
  else if(mbstr[0] == 0xc0 ){
     ans = 0x0;
  } 
  else if(((mbstr[0]>>5) == 0x06) && (mbstr[1]>> 6 == 0x02)){
     ans = ans | (mbstr[1]&0x3f)| (mbstr[0]&0x1f)<<6;
     if(ans <= 0x7f){
        ans = 0x0;
     }else{
        ans = ans | (mbstr[1]&0x3f)| (mbstr[0]&0x1f)<<6 | 0x00800000;
     }
  } 
  else if(((mbstr[0]>>4) == 0x0e) && (mbstr[1]>> 6 == 0x02) && (mbstr[2]>> 6 == 0x02)){
     ans = ans | (mbstr[0]&0xf)<<12 | (mbstr[1]&0x3f)<<6 | (mbstr[2]&0x3f);
     if (ans <= 0x7ff){
        ans = 0x0;
     }else{
        ans = ans | (mbstr[0]&0xf)<<12 | (mbstr[1]&0x3f)<<6 | (mbstr[2]&0x3f) | 0x00c00000;
     }
  } 
  else if (((mbstr[0]>>3) == 0x1e) && (mbstr[1]>> 6 == 0x02) && (mbstr[2]>> 6 == 0x02)&&(mbstr[3]>> 6 == 0x02)){
     ans = ans | (mbstr[3]&0x3f) | (mbstr[2]&0x3f)<<6 | (mbstr[1]&0x3f)<<12 | (mbstr[0]&0x07)<<18;
     if (ans <= 0xffff){
        ans = 0x0;
     } else if(ans > 1114111){
        ans = 0x0;
     } else{
        ans = ans | 0x01000000 |(mbstr[3]&0x3f) | (mbstr[2]&0x3f)<<6 | (mbstr[1]&0x3f)<<12 | (mbstr[0]&0x07)<<18;
     }
  } 

  return ans;
}


/* (EXTRA CREDIT) Convert the variable-length encoded mbstr to a fixed
   length string where each character occupies exactly 32-bits.

   You can assume the output array output_str contains at least nbytes
   elements.

   Return the actual number of characters decoded.

   
 */
size_t mbstr_to_fixed(uint8_t mbstr[], size_t nbytes, uint32_t output_str[])
{
  int length = mbstr_length(mbstr, nbytes);
  int i = 0;
  int j = 0; // j is the index for output_str;
  while(i < nbytes){
     if((mbstr[i]>>7 == 0x00)){
        length++;
        i++;
        output_str[j] = 0x00000000 | mbstr[i];
        j++;
     } else if((mbstr[i]>>5) == 0x06){
                if((mbstr[i+1]>>6) == 0x02){
                   length++;
                   i = i + 2;
                   output_str[j] = 0x00000000 | mbstr[i+1] | mbstr[i]<<8;
                   j++;
                } else{
                   i = i + 2;
                }
     } else if((mbstr[i]>>4)==0x0e){
                if(((mbstr[i+1]>>6) == 0x02) && (mbstr[i+2]>>6) == 0x02){
                   length++;
                   i = i + 3;
                   output_str[j] = 0x00000000 | mbstr[i+2] | mbstr[i+1]<<8 | mbstr[i]<<16;
                   j++;
                } else{
                  i = i + 3;
                }
     } else if((mbstr[i]>>3) == 0x1e){
                if (((mbstr[i+1]>>6) == 0x02) && (mbstr[i+2]>>6) == 0x02 && (mbstr[i+3]>>6) == 0x02){
                   length++;
                   i = i + 4;
                   output_str[j] = 0x00000000 | mbstr[i+3] | mbstr[i+2]<<8 | mbstr[i+1]<<16 | mbstr[i]<<24;
                   j++;
                } else{
                   i = i + 4;
                }
     }
  }
  return j-1;
}
