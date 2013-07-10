#include "sys/types.h"
#include <ctype.h>
#include "stdlib.h"
#include "stdio.h"
#include "define.h"
#include "struct.h"

void save_pextra( pfile_data *pfile )
{
  FILE*      fp;
  int   version  = 2;
  
  if( ( fp = open_file( EXTRA_DIR, pfile->name, "w" ) ) == NULL )
    return;
  
  fprintf( fp, "%d\n", version );
  
  fprintf( fp, "%d\n", pfile->bounty );
  fprintf( fp, "%s~\n", pfile->icq );
  fprintf( fp, "%d\n", pfile->pkcount );
  fprintf( fp, "%d\n", pfile->pickupage );
  fprintf( fp, "%d\n", pfile->max_trust );
  
  for( int i = 0; i < MAX_EXPCAT; i++ ) {
     fprintf( fp, "%d\n", pfile->levelt[ i ] );
     fprintf( fp, "%d\n", pfile->expt  [ i ] );
     }

  fclose( fp );
}
 

void load_pextra( pfile_data* pfile )
{
  FILE*      fp;
  int   version;
  
  if( ( fp = open_file( EXTRA_DIR, pfile->name, "r", FALSE ) ) == NULL )
    return;
  
  version = fread_number( fp );
  
  if( version < 1 )
  {
    bug( "Load_Pextra: Incorrect version number." );
    fclose( fp );
    return;
  }
  
  pfile->bounty    = fread_number( fp );
  pfile->icq       = fread_string( fp, MEM_PFILE );
  pfile->pkcount   = fread_number( fp );
  pfile->pickupage = fread_number( fp );
  pfile->max_trust = fread_number( fp );

  for( int i = 0; i < MAX_EXPCAT; i++ ) {
     pfile->levelt[ i ]   = fread_number( fp );
     pfile->expt  [ i ]   = fread_number( fp );
     if( pfile->levelt[ i ] == 0 )
       pfile->levelt[ i ] = 1;

     if( i == EXP_MISC )
       if( pfile->levelt[ i ] == 0 ) {
         pfile->levelt[ i ] = pfile->level;
         pfile->expt  [ i ] = pfile->exp;
         }
     }

  fclose( fp ); 
}
