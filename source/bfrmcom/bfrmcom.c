#include <stdio.h>

main()
{
  char ch ;
  int i ;

  ch = (char) fgetc( stdin ) ;

  while ( !feof( stdin ) )
  {
    if ( ch == '+' || ch == '-' ||
         ch == '[' || ch == ']' ||
         ch == '.' || ch == ',' ||
         ch == '<' || ch == '>' ||
         ch == '!' )
    {
       if ( i == 70 )
       {
          printf( "\n" ) ;
          i = 0 ;
       }
       i++ ;
       printf( "%c", ch ) ;
    }
    ch = (char) fgetc( stdin ) ;
  }
  printf( "\n" ) ;
}

