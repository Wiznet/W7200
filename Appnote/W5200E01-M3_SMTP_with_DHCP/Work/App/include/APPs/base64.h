#ifndef	_BASE64_H_
#define	_BASE64_H_

 
/*
********************************************************************************
Define Part
********************************************************************************
*/


/*
********************************************************************************
Function Prototype Definition Part
********************************************************************************
*/
int base64_decode(char *text, unsigned char *dst, int numBytes );
int base64_encode(char *text, int numBytes, char *encodedText);


#endif	/* _BASE64_H_ */
