#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"

bool can_hide      ( char_data*, bool );

// badpos added by Sphenotus Sept 6, 2000
thing_data* badpos( thing_data* object, char_data* thief, thing_data* victim )
{
  
    char_data*  rch = (char_data*) victim;
    obj_data*   obj = (obj_data*) object;

    if( obj == rch->Wearing( WEAR_BODY ) ||
        obj == rch->Wearing( WEAR_HEAD ) ||
        obj == rch->Wearing( WEAR_LEGS ) ||
        obj == rch->Wearing( WEAR_FEET ) ||
        obj == rch->Wearing( WEAR_ARMS ))
        return NULL; 
    return obj; 
}

// steal code added by Sphenotus Sept 6, 2000
thing_data* steal( thing_data* obj, char_data* receiver, thing_data* giver )
{
    obj = obj->From( obj->selected );
    set_owner( (obj_data*) obj, receiver, (char_data*) giver );
    obj->To( receiver );

    return obj;
}

void do_gouge( char_data* ch, char* argument )
{
  char_data* victim;
  int  delay;

  if( is_confused_pet( ch ) )
    return;
  
  if( is_mob( ch ) )
    return;

  if( ch->get_skill( SKILL_EYE_GOUGE )  < 1 ) {
    send( ch, "You do not know how to gouge eyes.\n\r" );
    return;
    }

  if( ( victim = get_victim( ch, argument, "Gouge whose eyes?\n\r" ) ) ==
    NULL )
    return;

  if( victim == ch ) {
    send( ch, "Gouging your own eyes is not very productive.\n\r" );
    return;
    }

  if( victim->species != NULL
    && !is_set( &victim->species->act_flags, ACT_HAS_EYES ) ) {
    send( ch,  "This creature has no eyes and cannot be blinded.\n\r" );
    return;
    }

  if( victim->fighting != ch && ch->fighting != victim ) {
    if( !can_kill( ch, victim ) )
      return;
    check_killer( ch, victim );
    }

  delay = gouge_attack( ch, victim );

  ch->fighting = victim ;
  react_attack( ch, victim );

  add_queue( &ch->active, delay );

  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_EYE_GOUGE );

  return;
}


int gouge_attack( char_data* ch, char_data* victim )
{
  int roll = number_range( 0, 20 )
    + ch->get_skill( SKILL_EYE_GOUGE )/2
    + (ch->shdata->dexterity - victim->shdata->dexterity)/2
    + (ch->shdata->level - victim->shdata->level)/4;

  if( roll < 6 ) {
    fsend( ch, "You attempt to gouge %s but are unsuccessful.", victim );
    fsend( victim, "%s attempts to gouge you but is unsuccessful.", ch );
    fsend( *ch->array,
      "%s attempts to gouge %s but is unsuccessful.", ch, victim );
    return 32;
    }

  if( roll > 20 ) {
    fsend( ch, "You gouge %s in the eye!!", victim );
    fsend( victim, "%s gouges you in the eye!!", ch );
    fsend( *ch->array, "%s gouges %s in the eye!!", ch, victim );

    int duration = 1+number_range( 0,
      (int) ( ch->get_skill( SKILL_EYE_GOUGE )/4.0 ) );
    spell_affect( ch, victim, ch->get_skill( SKILL_EYE_GOUGE ),
      duration, SPELL_BLIND, AFF_BLIND );

    disrupt_spell( victim ); 
    set_delay( victim, 32 );
    return 20;
  }

  send( ch, "You attempt to gouge %s but fail.\n\r", victim );
  send( victim, "%s attempts to gouge you but fails.\n\r", ch );
  send( *ch->array, "%s attempts to gouge %s but fails.\n\r", ch, victim );
  return 20;
}

void do_disguise( char_data*, char* )
{
  return;
}


void do_garrote( char_data* ch, char* argument )
{
  char_data* victim;
  int         level;
  obj_data* garrote;
  int             i;


  for( i = 0; i < ch->contents; i++ ) {
    garrote = (obj_data*) ch->contents[i];
    if( garrote->pIndexData->item_type == ITEM_GARROTE )
      break;
    }
  
  if( i == ch->contents.size ) {
    send( ch, "You don't have a garrote.\n\r" );
    return;
    }
  
  level = ch->get_skill( SKILL_GARROTE );
  
  if( level == 0 ) {
    send( "Garrote is not part of your repertoire\n\r", ch );
    return;
    }

  if( ( victim = get_victim( ch, argument, "Garrote whom?\n\r" ) ) == NULL )
    return;

  if ( victim == ch ) {
    send( ch, "Garroting yourself takes more skill then you can muster.\n\r" );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;
  
  set_bit( victim->affected_by, AFF_CHOKING );
  send( victim, "A garrote begins to tighten around your neck.\n\r" );
  send( *victim->array, "The garrote tightens around %s neck.\n\r", victim ); 
  
  ch->fighting = victim ;
  react_attack( ch, victim );
  add_queue( &ch->active, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  set_delay( victim, 32 );
  ch->improve_skill( SKILL_GARROTE );
 
  attack( ch, victim, "garrote", garrote, -1, 0 );
  
  return;
}


/* 
 *   BACKSTAB FUNCTIONS
 */


void do_backstab( char_data* ch, char* argument )
{
  char_data*  victim;
  obj_data*      obj;
  int          skill  = ch->get_skill( SKILL_BACKSTAB );

  if( ch->mount != NULL ) {
    send( ch, "Backstabbing while mounted is beyond your skill.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Backstab whom?\n\r" );
    return;
    }

  if( opponent( ch ) != NULL ) {
    send( ch, "You are unable to backstab while fighting someone.\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "backstab", ch->array ) )
    == NULL ) 
    return;

  if( victim == ch ) {
    send( "How can you sneak up on yourself?\n\r", ch );
    return;
    }

  if( skill == 0 ) {
    send( "Backstabbing is not part of your repertoire.\n\r", ch );
    return;
    }

  if( ( obj = ch->Wearing( WEAR_HELD_R ) ) == NULL ) {
    send( "You need to be wielding a weapon to backstab.\n\r", ch );
    return;
    }

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_BACKSTAB ) ) {
    send( ch, "It isn't possible to use %s to backstab.\n\r", obj );
    return;
    }

  if( ch->Seen( victim ) && includes( victim->aggressive, ch ) ) {
    send( ch, "%s is too wary of you for backstab to succeed.\n\r", victim );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim ;
  react_attack( ch, victim );
  add_queue( &ch->active, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  if( !attack( ch, victim, "backstab", obj, -1, 0 ) && ch->Is_Valid( ) )
    send( ch, "Your attempted backstab misses the mark.\n\r" );

  return;
}


/*
 *   STEAL ROUTINES
 */


void do_steal( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  obj_data*      obj;

  if( is_confused_pet( ch ) )
    return;
 
  if( is_mob( ch ) )
    return;
 
  if( !two_argument( argument, "from", arg ) ) {
    send( ch, "Syntax: steal <object> [from] <character>\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "steal", ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( "That's pointless.\n\r", ch );
    return;
    }

  if( !can_kill( ch, victim ) ) {
    if( ch->Is_Valid( ) )
      send( "You can't steal from them.\n\r", ch );
    return; 
    } 

  if( !ch->check_skill( SKILL_STEAL )
    || number_range( 3, 35 ) < victim->Intelligence( ) ) {
    leave_shadows( ch );
    ch->fighting = victim ;
    react_attack( ch, victim );
    add_queue( &ch->active, number_range(5,20) );
    set_delay( ch, 32 );
 
    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    send( victim, "%s tried to steal from you.\n\r", ch );
    send( *ch->array, "%s tried to steal from %s.\n\r", ch, victim );
    modify_reputation( ch, victim, REP_STOLE_FROM );
    if( victim->pShop != NULL ) {
      sprintf( buf, "Guards! %s is a thief.", ch->Name( victim ) );
      buf[8] = toupper( buf[8] );
      do_yell( victim, buf );
      summon_help( victim, ch );
      }
    return;
  }

  if ( ( obj = one_object( ch, arg, empty_string,
    &victim->contents ) ) == NULL ) {
    send( "You can't find it.\n\r", ch );
    return;
  }
    
  if ( !obj->droppable() ) {
    send( ch, "You can't pry it away.\n\r" );
    return;
  }

  if ( ch->contents.size >= ch->can_carry_n() ) {
    send( "You have your hands full.\n\r", ch );
    return;
  }

  if ( obj->Weight() > ch->Capacity() ) {
    send( "You can't carry that much weight.\n\r", ch );
    return;
  }

  obj = (obj_data*) obj->From( 1 );
  obj->To( ch );
  send( ch, "You succeeded in stealing %s.\n\r", obj );
  consolidate( obj );
  ch->improve_skill( SKILL_STEAL );

  return;
}


void do_heist( char_data* ch, char* argument )
{

    char           buf  [ MAX_INPUT_LENGTH ];
    char           arg  [ MAX_INPUT_LENGTH ];
    char_data*  victim;
    thing_array*   array;

    if( is_confused_pet( ch ) )
        return;
 
    if( is_mob( ch ) )
        return;
 
    if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
        send( ch, "You can not heist with parry on.\n\r" );
        return;
    }

    if( !is_set( &ch->status, STAT_HIDING ) && !is_set( &ch->status, STAT_CAMOUFLAGED ) ) {
        send( ch, "You can not heist while hidden.\n\r" );
        return;
    }

    argument = one_argument( argument, arg );

    for( ; ; ) {
        argument = one_argument( argument, buf );
        if( buf[ 0 ] == '\0' || !strcasecmp( buf, "from" ) )
            break;
        sprintf( arg+strlen( arg ), " %s", buf );
    }

    if( arg[0] == '\0' || argument[0] == '\0' ) {
        send( "Syntax: heist <object> from <character>\n\r", ch );
        return;
    }

    if( ( victim = one_character( ch, argument, "heist", ch->array ) ) == NULL ) 
        return;

    if( victim == ch ) {
        send( "That's pointless.\n\r", ch );
        return;
    }

    if( victim->pcdata != NULL ) {
        send( "You can't heist from players.\n\r", ch );
        return;
    }  

    if( !can_kill( ch, victim ) ) {
        send(ch, "You can't heist from them.\n\r" );
        return; 
    } 

    if( ch->shdata->skill[ SKILL_HEIST] == UNLEARNT) {
        send(ch, "You are untrained in the art of heisting.\n\r");
        return;
    }
   
    remove_bit( ch->pcdata->pfile->flags, PLR_PARRY );

    if(ch->Dexterity() / 4 + ch->shdata->skill [ SKILL_HEIST ] + 
        ( ( ch->shdata->level-victim->shdata->level )/4) < number_range( 1, 20 ) ) {
        leave_shadows( ch );
        if(!player(victim)) {
            ch->fighting = victim;
            react_attack( ch, victim );
            add_queue( &ch->active, 20 );
            remove_bit( &ch->status, STAT_LEAPING );
            remove_bit( &ch->status, STAT_WIMPY );
        } 
        send( victim, "%s tried to steal from you.\n\r", ch );
        send( *ch->array, "%s tried to steal from %s.\n\r", ch, victim );
        modify_reputation( ch, victim, REP_STOLE_FROM );
        if( victim->pShop != NULL ) {
            sprintf( buf, "Guards! %s is a thief.", ch->Name( victim ) );
            do_yell( victim, buf );
            summon_help( victim, ch );
        }
        return;
    }

    if( ( array = several_things( ch, arg, "heist", &victim->wearing ) ) == NULL )
        return;

    if(array->size > 1) {
        send(ch, "You may only heist one item at a time.");
        return;
    } 
    thing_array   subset  [ 5 ];
    thing_func*     func  [ 5 ]  = { cursed, heavy, many, badpos, steal };

    sort_objects( ch, *array, victim, 5, subset, func );

    page_priv( ch, NULL, empty_string );
    page_priv( ch, &subset[0], "can't let go of", victim );
    page_priv( ch, &subset[1], "can't lift" );
    page_priv( ch, &subset[2], "can't handle" );
    page_priv( ch, &subset[3], "can't heist");
    page_priv( ch, &subset[4], "steal", victim, "from" );

    ch->improve_skill( SKILL_HEIST );

    return;
}

 
/*
 *   SNEAK FUNCTIONS (Redone by Sphenotus)
 */


void do_sneak( char_data* ch, char* argument )
{
    if( not_player( ch ) )
        return;

    if( ch->shdata->skill[ SKILL_SNEAK ] == 0 ) {
        send( ch, "Sneaking is not something you are adept at.\n\r" );
        return;
    }

    if( is_set( ch->pcdata->pfile->flags, PLR_SNEAK ) ) {
        remove_bit( ch->pcdata->pfile->flags, PLR_SNEAK );
        send( ch, "You stop sneaking.\n\r" );
        return;
    }
   
    if( ch->mount != NULL ) {
        send( ch, "But your mount can't sneak!\n\r");
        return;
    }

    if( ch->fighting != NULL ) {
        send( ch, "It would be a good idea to concentrate on the battle.\n\r" );
        return;
    }

    if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) || is_set( ch->affected_by, AFF_FAERIE_FIRE ) ) {
        send( ch, "Your fiery glow rather spoils that.\n\r" );
        return;
    }

    if( water_logged( ch->in_room ) ) {
        send( ch, "It's hard to sneak in water.\n\r" );
        return;
    }
    
    set_bit( ch->pcdata->pfile->flags, PLR_SNEAK );

    send( ch, "You start sneaking.\n\r" );
    
}

/*
 *   HIDE ROUTINES
 */


bool can_hide( char_data* ch, bool msg )
{
  room_data*  room  = ch->in_room;

  if( ch->mount != NULL ) {
    if( msg ) 
      send( ch, "Hiding while mounted is impossible.\n\r" );
    return FALSE;
    }

  if( ch->fighting != NULL ) {
    if( msg )
      send( ch, "Hiding while fighting someone is difficult.\n\r" );
    return FALSE;
    }

  if ( !ch->Can_See() ) {
    if( msg )
      send( ch, "Hiding while blind is extremely difficult.\n\r" );
    return FALSE;
    }

  if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) 
    || is_set( ch->affected_by, AFF_ION_SHIELD )
    || is_set( ch->affected_by, AFF_FAERIE_FIRE ) ) {
    if( msg ) 
      send( ch, "Your fiery glow rather spoils that.\n\r" );
    return FALSE;
    }

  if( water_logged( room ) ) {
    if( msg )
      send( ch, "Hiding is unsoluble.\n\r" );
    return FALSE;
    }
    
  return TRUE;
}

void do_hide( char_data* ch, char* )
{
    char_data*               rch;
    char*            send_string;
    int           terrain_chance;

    if( not_player( ch ) )
        return;

    if( ch->shdata->skill[ SKILL_HIDE ] == 0 ) {
        send( ch, "You attempt to conceal yourself but fail.\n\r" );
        return;
    }

    if( leave_shadows( ch ) )
        return;

    if( ch->mount != NULL ) {
        send( ch, "You cannot hide while mounted.\n\r" );
        return;
    }
    
    if( ch->fighting != NULL ) {
        send( ch, "You are in battle!\n\r");
        return;
    }

    if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) || is_set( ch->affected_by, AFF_FAERIE_FIRE ) ) {
        send( ch, "Your fiery glow rather spoils that.\n\r" );
        return;
    }

    switch( ch->in_room->sector_type ) {
        case SECT_SWAMP :
            terrain_chance = 10;
            send_string = "You crouch low, concealing yourself in the swampy fog.";
            break;
        case SECT_FOREST :
            terrain_chance = 10;
            send_string = "You conceal yourself in the trees.";
            break;
        case SECT_JUNGLE :
            terrain_chance = 10;
            send_string = "You hide in the lush foliage.";
            break;
        case SECT_CAVES :
            terrain_chance = 9;
            send_string = "You step into the shadows, concealing yourself.";
            break;
        case SECT_ARCTIC :
            terrain_chance = 9;
            send_string = "Ignoring the cold, you crouch low in the snow and conceal yourself.";
            break;
        case SECT_FIELD :
            terrain_chance = 8;
            send_string = "You hunker down in the grass.";
            break;
        case SECT_CITY :
            terrain_chance = 8;
            send_string = "You hide among the shadows of the city.";
            break;
        case SECT_HILLS :
            terrain_chance = 7;
            send_string = "You hunker down in the grass.";
            break;
        case SECT_MOUNTAIN :
            terrain_chance = 7;
            send_string = "You hide among the rocks and trees.";
            break;
        case SECT_ICE :
            terrain_chance = 4;
            send_string = "You crouch low, hugging the icy terrain.";
            break;
        case SECT_DESERT :
            terrain_chance = 3;
            send_string = "You manage to conceal yourself in the sandy terrain.";
            break;
        case SECT_INSIDE :
            terrain_chance = 1;
            send_string = "You hide in the shadows.";
            break;
        case SECT_ROAD :
            terrain_chance = 1;
            send_string = "You crouch down on the side of the road.";
            break;
        case SECT_SHALLOWS :
            terrain_chance = 1;
            send_string = "You conceal yourself in the marsh.";
            break;
        case SECT_BEACH :
            terrain_chance = 1;
            send_string = "You hunker down in the sand.";
            break;
        case SECT_UNKEPT_ROAD :
            terrain_chance = 3;
            send_string = "You hunker down in some bushes.";  
            break;
        case SECT_TRAIL :
            terrain_chance = 3;
            send_string = "You conceal yourself at the trail's edge.";  
            break;
        case SECT_PATH : 
            terrain_chance = 3;
            send_string = "You crouch down at the edge of the path.";  
            break;
        case SECT_STREAM :            
        case SECT_UNDERWATER :            
        case SECT_RIVER :            
        case SECT_WATER_SURFACE :            
        case SECT_AIR :
            send( ch, "You cannot hide here.\n\r" );
            return;
        default :
            code_bug( "HIDE : Invalid Terrain." );
            return;
        
    }
    if( number_range(1, 20) > ch->shdata->skill[ SKILL_HIDE ] + terrain_chance ) {
        send( ch, "You attempt to conceal yourself, but fail.\n\r" );
        return;
    }
        
    for( int i = 0; i < *ch->array; i++ ) 
        if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) )
            ch->seen_by += rch;

    send( ch, "%s\n\r", send_string );
     
    set_bit( &ch->status, STAT_HIDING );

    ch->improve_skill( SKILL_HIDE );
    
    if ( ch->pcdata->clss == CLSS_THIEF )
        if ( ch->shdata->level > 59 && terrain_chance > number_range(1,10) ) {
            send( ch, "You manage to camouflage yourself into the surroundings.\n\r");
            set_bit( &ch->status, STAT_CAMOUFLAGED );
        }    
         
}

bool leave_shadows( char_data* ch )
{
    if( !is_set( &ch->status, STAT_HIDING ) && !is_set( &ch->status, STAT_CAMOUFLAGED ) )
        return FALSE;
  
    remove_bit( &ch->status, STAT_CAMOUFLAGED );
    remove_bit( &ch->status, STAT_HIDING );
    
    clear( ch->seen_by );

    send( ch, "You stop hiding.\n\r" );
    send_seen( ch, "%s steps from the shadows.\n\r", ch );
    return TRUE;
}



/* 
 *   DIP ROUTINE
 */


void do_dip( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  obj_data*   container;
  obj_data*         obj;
  int             value;
  affect_data    affect;
  int             spell;

  if( !two_argument( argument, "into", arg ) ) {
    send( ch, "Syntax: Dip <object> [into] <object>\n\r" );
    return;
    }

  if( ( obj = one_object( ch, arg, "dip", &ch->contents ) ) == NULL ) 
    return;

  if( ( container = one_object( ch, argument, "dip into",
    &ch->contents, ch->array ) ) == NULL ) 
    return;

  if( container->pIndexData->item_type != ITEM_DRINK_CON
    && container->pIndexData->item_type != ITEM_FOUNTAIN ) {
    send( ch, "%s isn't something you can dip things into.\n\r", container );
    return;
    }

  if( container == obj ) {
    send( ch, "You can't dip %s into itself.\n\r", obj );
    return;
    }

  value               = container->value[1];
  container->value[1] = -2;

  if( value == 0 ) {
    send( ch, "%s is empty.\n\r", container );
    container->value[1] = value;
    return;
    }

  obj = sunder( obj );

  if( ( obj->dampness += max( 0, liquid_table[container->value[2]].thirst-
    liquid_table[container->value[2]].alcohol )*5 ) > 100 )
    obj->dampness = 100;
  
  if( ( obj->burnmass += max( 0, liquid_table[container->value[2]].alcohol-
    liquid_table[container->value[2]].thirst )*5 ) > 1000 )
    obj->burnmass = 1000;  
  
  consolidate( obj );

  if( obj->burning && stop_burning( obj ) ) {
    fsend( ch, "You extinguish %s by quickly dipping it into %s.",
      obj, container );
    fsend( *ch->array,
      "%s extinguishes %s by quickly dipping it into %s.",
      ch, obj, container );
    
    container->value[1] = value;
    return;
    } 

  send( ch, "You dip %s into %s.\n\r", obj, container );
  send_seen( ch, "%s dips %s into %s.\n\r", ch, obj, container );
  container->value[1] = ( value == -1 ? -1 : max( 0, value-5 ) );

  if( ( spell = liquid_table[container->value[2]].spell ) == -1 ) 
    return;

  if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    bug( "Do_dip: Liquid with non-spell skill." );
    return;
    }

  if( is_set( &spell_table[spell-SPELL_FIRST].usable_flag, STYPE_DIP ) )
    ( *spell_table[spell-SPELL_FIRST].function )( ch, NULL, obj, 10, -3,
      STYPE_DIP ); 

  return;
}  
