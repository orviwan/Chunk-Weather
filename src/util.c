#include "util.h"
#define INT_DIGITS 3		/* enough for 64 bit integer */

//int to char
char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

//char to int
int my_atoi(char *p) {
 int k = 0;
 while (*p) {
 k = (k<<3)+(k<<1)+(*p)-'0';
 p++;
 }
 return k;
}



static const char *daySuffixLookup[] = { "th","st","nd","rd","th",
                           "th","th","th","th","th" };
//This is broken! 21nd
const char *daySuffix(int n)
{
    if(n % 100 >= 11 && n % 100 <= 13)
        return "th";

    return daySuffixLookup[n % 10];
}



