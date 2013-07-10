#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


weather_data  weather;

const char* month_name [ 10 ] = {
  "Month of the Boar", "Month of the Merchant", "Month of the King",
"Month of the Wolf", "Month of Merrymaking",  "Month of the Squanderer",
"Month of the Scion", "Month of the Undead",  "Month of the Red Moon",
"Month of DawnStar" };

const int days_in_month [ 10 ] = {
  25, 25, 25, 25, 25, 25, 25, 25, 25, 25  };


void time_update( void )
{
  char         tmp  [ ONE_LINE * 7];
  link_data*  link;
  
  tmp[0] = '\0';
 
  if( ++weather.minute > 59 ) {
    weather.hour++;
    weather.minute = 0;

    switch ( weather.hour ) {
    case  5:
      strcat( tmp, "Streaks of light begin to show over the horizon.\n\r");

      break;

    case  6:
      strcat( tmp, "The sun peaks in the east, proclaiming a new day in Faldyor.\n\r");

      break;

    case 12:
      strcat( tmp, "The sun shines brightly from the center of the sky.\n\r");
    
      break;

    case 19:
      strcat( tmp, "The sun disappears over the Cairn Mountains.\n\r" );
      break;

    case 20:
      strcat( tmp, "The night looms darkly over Faldyor.\n\r" );
      break;

    case 24:
      weather.hour = 0;
      weather.day++;
      break;
      }
    }

  if( weather.day  > days_in_month[weather.month] ) {
    weather.day = 1;
    weather.month++;
    }

  if( weather.month > 12 ) {
    weather.month = 1;
    weather.year++;
    }

  weather.sunlight = sunlight( 60*weather.hour+weather.minute );

  if( *tmp != '\0' ) {
    for( link = link_list; link != NULL; link = link->next ) {
      if( link->connected == CON_PLAYING && IS_OUTSIDE( link->character )
        && IS_AWAKE( link->character )
        && is_set( &link->character->pcdata->message, MSG_WEATHER ) )
        send( tmp, link->character );
      }
    }
}


const char* sky_state( )
{
  if( weather.hour < 5 || weather.hour > 20 ) 
    return "Night";

  if( weather.hour < 6  )  return "Early Morning";
  if( weather.hour < 8  )  return "Mid-Morning";
  if( weather.hour < 14 )  return "Near-Noon";
  if( weather.hour < 17 )  return "Afternoon";
   
  return "Evening";
}
    

