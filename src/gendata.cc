#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"

/* 
 *     Gen_Data Routines - Original work ... Smith - 29-4-2000
 */


/*
 *     generic Construction/Destruction routines
 */

#define gdEmptyLabel "EMTY"

void Gen_Data :: Initialize ( )
{
  section = new char[LABEL_SIZE+1];
  strcpy (section, gdEmptyLabel);

  str_count = 0;
  str_labels = NULL;
  str_data = NULL;
  
  int_count = 0;
  int_labels = NULL;
  int_data = NULL;

  data_count = 0;
  data_labels = NULL;
  data_gen = NULL;

  array_count = 0;
  arrays_count = NULL;
  arrays_labels = NULL;
  arrays_gen = NULL;  
}

Gen_Data :: Gen_Data ( ) 
{
  Initialize ( );
}

Gen_Data :: Gen_Data (FILE* fp)
{
  Initialize ( );
  Load(fp);
}

#define gdAreaLabel "AREA"
#define gdAreaName  "ANme"
#define gdCreator   "Crea"
#define gdHelp      "Help"
#define gdLevel     "Levl"
#define gdResetTime "RsTm"
#define gdStatus    "Stat"
#define gdRooms     "Roms"

Gen_Data :: Gen_Data (area_data* in)
{
  Initialize ( );
  
  Set_Section (gdAreaLabel);
  
  Set_Count (gdStr, 3);
  Add_String (gdAreaName, in->name);
  Add_String (gdCreator, in->creator);
  Add_String (gdHelp, in->help);
  
  Set_Count (gdInt, 3);
  Add_Number (gdLevel, in->level);
  Add_Number (gdResetTime, in->reset_time);
  Add_Number (gdStatus, in->status);

  Set_Count (gdArray, 1);
  Add_Array (gdRooms, NULL);
  int count_temp = 0;
  room_data* room = NULL;
  for( room = in->room_first; room != NULL; room = room->next ) 
    count_temp++;
  Set_Array_Count (0,count_temp);
  room = NULL;
  for( room = in->room_first; room != NULL; room = room->next ) 
    Add_Element (0, new Gen_Data (room));  
}

#define gdRoomLabel   "ROOM"
#define gdRoomName    "RNme"
#define gdDescription "Desc"
#define gdComments    "Comt"
#define gdVNum        "VNum"
#define gdRoomFlags   "RFlg"
#define gdSectorType  "ScTy"
#define gdSize        "Size"
#define gdExits       "Exts"
#define gdExtras      "Etrs"
#define gdActions     "Acts"
#define gdResets      "Rsts"


Gen_Data :: Gen_Data (room_data* in)
{
  Initialize ( );
  
  Set_Section (gdRoomLabel);
  
  Set_Count (gdStr, 3);
  Add_String (gdRoomName, in->name);
  Add_String (gdDescription, in->description);
  Add_String (gdComments, in->comments);

  Set_Count (gdInt, 4);
  Add_Number (gdVNum, in->vnum);
  Add_Number (gdRoomFlags, in->room_flags);
  Add_Number (gdSectorType, in->sector_type);
  Add_Number (gdSize, in->size);

  Set_Count (gdArray, 4);
  Add_Array (gdExits, NULL);
  Set_Array_Count (0,in->exits.size);
  for( int i = 0; i < in->exits.size; i++ ) 
    Add_Element (0, new Gen_Data (in->exits[i]));  

  Add_Array (gdExtras, NULL);
  Set_Array_Count (1,in->extra_descr.size);
  for( int i = 0; i < in->extra_descr.size; i++ ) 
    Add_Element (1, new Gen_Data (in->extra_descr[i]));  
  
  Add_Array (gdActions, NULL);
  int count_temp = 0;
  action_data* action = NULL;
  for( action = in->action; action != NULL; action = action->next ) 
    count_temp++;
  Set_Array_Count (2,count_temp);
  action = NULL;
  for( action = in->action; action != NULL; action = action->next ) 
    Add_Element (2, new Gen_Data (action));  

  Add_Array (gdResets, NULL);
  count_temp = 0;
  reset_data* reset = NULL;
  for( reset = in->reset; reset != NULL; reset = reset->next ) 
    count_temp++;
  Set_Array_Count (3,count_temp);
  reset = NULL;
  for( reset = in->reset; reset != NULL; reset = reset->next ) 
    Add_Element (3, new Gen_Data (reset));  
  
}
#define gdExitLabel    "EXIT"
#define gdExitName     "ENme"
#define gdExitKeywords "EtKW"
#define gdDirection    "Dire"
#define gdExitInfo     "EInf"
#define gdKey          "EKey"
#define gdTarget       "Targ"
#define gdStrength     "Stre"
#define gdLight        "Lght"
#define gdExitSize     "ESze"

Gen_Data :: Gen_Data (exit_data* in)
{
  Initialize ();
  
  Set_Section (gdExitLabel);

  Set_Count (gdStr, 2);
  Add_String (gdExitName, in->name);
  Add_String (gdExitKeywords, in->keywords);

  Set_Count (gdInt, 7);
  Add_Number (gdDirection, in->direction);
  Add_Number (gdExitInfo, in->exit_info);
  Add_Number (gdKey, in->key);
  Add_Number (gdTarget, in->to_room->vnum);
  Add_Number (gdStrength, in->strength);
  Add_Number (gdLight, in->light);
  Add_Number (gdExitSize, in->size);

}

#define gdExtraLabel    "ETRA"
#define gdExtraKeywords "EaKW"
#define gdText          "Text"

Gen_Data :: Gen_Data (extra_data* in)
{
  Initialize ();

  Set_Section (gdExtraLabel);

  Set_Count (gdStr, 2);
  Add_String (gdExtraKeywords, in->keyword);
  Add_String (gdText, in->text);

}

#define gdActionLabel  "ACTN"
#define gdCommand      "Comd"
#define gdTarget       "Targ"
#define gdCode         "Code"
#define gdTrigger      "Trig"
#define gdValue        "Valu"
#define gdFlags        "Flgs"
#define gdActionData   "Data"

Gen_Data :: Gen_Data (action_data* in)
{
  Initialize ();

  Set_Section (gdActionLabel);

  Set_Count (gdStr, 3);
  Add_String (gdCommand, in->command);
  Add_String (gdTarget, in->target);
  Add_String (gdCode, in->code);
  
  Set_Count (gdInt, 3);
  Add_Number (gdTrigger, in->trigger);
  Add_Number (gdValue, in->value);
  Add_Number (gdFlags, in->flags);
  
  Set_Count (gdArray, 1);
  Add_Array (gdActionData, NULL);
  Set_Array_Count (0,in->data.size);
  for( int i = 0; i < in->data.size; i++ ) 
    Add_Element (0, new Gen_Data (in->data[i]));  
  

}

#define gdResetLabel  "REST"
#define gdResetTarget "RtTg"
#define gdResetFlags  "RsFg"
#define gdChances     "Chnc"
#define gdResetValue  "Valu"
#define gdLiquid      "Liqu"

Gen_Data :: Gen_Data (reset_data* in)
{
  Initialize ();

  Set_Section (gdResetLabel);

  Set_Count (gdInt, 5);
  Add_Number (gdResetTarget, in->vnum);
  Add_Number (gdResetFlags, in->flags);
  Add_Number (gdChances, in->chances);
  Add_Number (gdResetValue, in->value);
  Add_Number (gdLiquid, in->liquid);

}

Gen_Data :: ~Gen_Data ( ) /* do i need NULL tests ??? */
{
  if (section != NULL) 
    delete[] section;
  
  for (; str_count > 0; str_count--) {
    if (str_labels[str_count-1] != NULL) 
      delete[] str_labels[str_count-1];
    if (str_data[str_count-1] != NULL)
      delete[] str_data[str_count-1];
  } 
  if (str_labels != NULL)
    delete[] str_labels;
  if (str_data != NULL)
    delete[] str_data;

  for (; int_count > 0; int_count--) 
    if (int_labels[int_count-1] != NULL) 
      delete[] int_labels[int_count-1]; 
  if (int_labels != NULL) 
    delete[] int_labels;
  if (int_data != NULL)
    delete[] int_data;

  for (; data_count > 0; data_count--) {
    if (data_labels[data_count-1] != NULL) 
      delete[] data_labels[data_count-1];
    if (data_gen[data_count-1] != NULL)
      delete data_gen[data_count-1];
  } 
  if (data_labels != NULL)
    delete[] data_labels;
  if (data_gen != NULL)
    delete[] data_gen;

  for (; array_count > 0; array_count--) {
    if (arrays_labels[array_count-1] != NULL)
      delete[] arrays_labels[array_count-1];
    for (; arrays_count[array_count-1] > 0; arrays_count[array_count-1]--) 
      if (arrays_gen[array_count-1][arrays_count[array_count-1]-1] != NULL)
        delete arrays_gen[array_count-1][arrays_count[array_count-1]-1];
    if (arrays_gen[array_count-1] != NULL)
      delete[] arrays_gen[array_count-1];
  }
  if (arrays_count != NULL)
    delete[] arrays_count;
  if (arrays_labels != NULL)
    delete[] arrays_labels;
  if (arrays_gen != NULL);
    delete[] arrays_gen;
}

/*
 *   DS Size manipulation/reading routines
 */


void Gen_Data :: Set_Count (int count_type, int counter)
{
  if (count_type == gdStr) {
    char** temp_labels = str_labels;
    char** temp_data = str_data;
    str_labels = new char*[counter];
    str_data = new char*[counter];
    int i=0;
    for (; i < str_count && i < counter; i++) {
      str_labels[i] = temp_labels[i];
      str_data[i] = temp_data[i];
    }
    for (; i < str_count; i++) {
      delete[] temp_labels[i];
      delete[] temp_data[i];
    }
    for (; i < counter; i++) {
      str_labels[i] = NULL;
      str_data[i] = NULL;
    }
    delete[] temp_labels;
    delete[] temp_data;
    str_count = counter;
    return;
  }

  if (count_type == gdInt) {
    char** temp_labels = int_labels;
    int* temp_data = int_data;
    int_labels = new char*[counter];
    int_data = new int[counter];
    int i=0;
    for (; i < int_count && i < counter; i++) {
      int_labels[i] = temp_labels[i];
      int_data[i] = temp_data[i];
    }
    for (; i < int_count; i++)
      delete[] temp_labels[i];
    for (; i < counter; i++) {
      int_labels[i] = NULL;
      int_data[i] = 0;
    }
    delete[] temp_labels;
    delete[] temp_data;
    int_count = counter;
    return;
  }

  if (count_type == gdData) {
    char** temp_labels = data_labels;
    Gen_Data** temp_data = data_gen;
    data_labels = new char*[counter];
    data_gen = new Gen_Data*[counter];
    int i=0;
    for (; i < data_count && i<counter; i++) {
      data_labels[i] = temp_labels[i];
      data_gen[i] = temp_data[i];
    }
    for (; i < data_count; i++) {
      delete[] temp_labels[i];
      delete temp_data[i];
    }
    for (; i < counter; i++) {
      data_labels[i] = NULL;
      data_gen[i] = NULL;
    }
    delete[] temp_labels;
    delete[] temp_data;
    data_count = counter;
    return;
  }

  if (count_type == gdArray) {
    char** temp_labels = arrays_labels;
    Gen_Data*** temp_data = arrays_gen;
    int* temp_count = arrays_count;
    arrays_labels = new char*[counter];
    arrays_gen = new Gen_Data**[counter];
    arrays_count = new int[counter];
    int i=0;
    for (; i < array_count && i < counter; i++) {
      arrays_labels[i] = temp_labels[i];
      arrays_gen[i] = temp_data[i];
      arrays_count[i] = temp_count[i];
    }
    for (; i < array_count; i++) {
      delete[] temp_labels[i];
      for(;temp_count[i]>0;temp_count[i]--) 
        delete temp_data[i][temp_count[i]-1];
      delete[] temp_data[i];
    }
    for (; i < counter; i++) {
      arrays_labels[i] = NULL;
      arrays_gen[i] = NULL;
      arrays_count[i] = 0;
    }
    delete[] temp_labels;
    delete[] temp_data;
    delete[] temp_count;
    array_count = counter;
    return;
  }
  return;
}

int Gen_Data :: Get_Count (int count_type)
{
  if (count_type == gdStr)
    return str_count;
  if (count_type == gdInt)
    return int_count;
  if (count_type == gdData)
    return data_count;
  if (count_type == gdArray)
    return array_count;
  return 0;
}

void Gen_Data :: Set_Array_Count (int array_num, int counter)
{
  if (array_num >= 0 && array_num < array_count) {
    Gen_Data** temp_data = arrays_gen[array_num];
    arrays_gen[array_num] = new Gen_Data*[counter];
    int i=0;
    for (; i < arrays_count[array_num] && i < counter; i++) 
      arrays_gen[array_num][i] = temp_data[i];
    for (; i < arrays_count[array_num]; i++)
      delete temp_data[i];
    for (; i < counter; i++)
      arrays_gen[array_num][i] = NULL;
    arrays_count[array_num] = counter; 
  }
  return;
}

int Gen_Data :: Get_Array_Count (int array_num)
{
  if (array_num >= 0 && array_num < array_count)
    return arrays_count[array_num];
  return 0;
}

/*
 *    Data input routines
 */

void Gen_Data :: Set_Section (const char* name)
{
  strcpy (section, name);
  return;
}

void Gen_Data :: Add_String (const char* label, const char* data)
{
  for (int i = 0; i < str_count; i++) 
    if (str_labels[i] == NULL) {
      str_labels[i] = new char[LABEL_SIZE+1];
      str_data[i] = new char[strlen(data)+1];
      strcpy (str_labels[i], label);
      strcpy (str_data[i], data);
      return;
    }
  /* might want to add auto expansion here */
  return; 
}

void Gen_Data :: Add_Number (const char* label, int data)
{
  for (int i = 0; i < int_count; i++) 
    if (int_labels[i] == NULL) {
      int_labels[i] = new char[LABEL_SIZE+1];
      int_data[i] = data;
      strcpy (int_labels[i], label);
      return;
    }
  /* might want to add auto expansion here */
  return; 

}

void Gen_Data :: Add_Gen_Data (const char* label, Gen_Data* data)
{
  for (int i = 0; i < data_count; i++) 
    if (data_labels[i] == NULL) {
      data_labels[i] = new char[LABEL_SIZE+1];
      data_gen[i] = data;
      strcpy (data_labels[i], label);
      return;
    }
  /* might want to add auto expansion here */
  return; 
}

void Gen_Data :: Add_Array (const char* label, Gen_Data** data)
{
  for (int i = 0; i < array_count; i++) 
    if (arrays_labels[i] == NULL) {
      arrays_labels[i] = new char[LABEL_SIZE+1];
      arrays_gen[i] = data;
      strcpy (arrays_labels[i], label);
      return;
    }
  /* might want to add auto expansion here */
  return; 
}

void Gen_Data :: Add_Element (int array_num, Gen_Data* data)
{
  if (array_num >= 0 && array_num < array_count) {
    for (int i = 0; i < arrays_count[array_num]; i++) {
      if (arrays_gen[array_num][i] == NULL) {
        arrays_gen[array_num][i] = data;
        return;
      }
    }
    /* might want to add auto expansion here */
    return;
  }
  /* probably dont want to add auto expansion here */
  return; 
}

/*
 *   Data retrieval routines
 */

char* Gen_Data :: Get_Section ( )
{
  return section;
}

char* Gen_Data :: Extract_String( const char* label, int index_guess )
{
  int            i;
  char*  returnStr;
  
  if( strcmp( label, str_labels[index_guess] ) )
  {
    for( i = 0; i < str_count; i++ )
      if( i != index_guess && !strcmp( str_labels[i], label ) )
      {
        index_guess = i;
        break;
      }
    if( i == str_count )
      return NULL;
  }
  
  returnStr = str_data[index_guess];
  str_data[index_guess] = NULL;
  *str_labels[index_guess] = '\\';
  
  return returnStr;
}

bool Gen_Data :: Get_Int( int& num, const char* label, int index_guess )
{
  if( !strcmp( label, int_labels[index_guess] ) )
  {
    num = int_data[index_guess];
    return TRUE;
  }

  for( int i = 0; i < int_count; i++ )
    if( i != index_guess && !strcmp ( int_labels[i], label ) )
    {
      num = int_data[i];
      return TRUE;
    }

  return FALSE;
}

char* Gen_Data :: Get_String (const char* label, int index)
{
  if (!strcmp(Get_String_Label (index), label) )
     return Get_String (index);
  else
    return Get_String (label);
}



char* Gen_Data :: Get_String (const char* label)
{
  for (int i = 0; i < str_count; i++)
    if (!strcmp (str_labels[i], label) ) 
      return str_data[i];
  return NULL;
}

int Gen_Data :: Get_String_Index (const char* label)
{
  for (int i = 0; i < str_count; i++)
    if (!strcmp (str_labels[i], label) ) 
      return i;
  return -1;
}

char* Gen_Data :: Get_String (int index)
{
  if (index >= 0 && index < str_count) 
    return str_data[index];
  return NULL;
}

char* Gen_Data :: Get_String_Label (int index)
{
  if (index >= 0 && index < str_count) 
    return str_labels[index];
  return NULL;
}

int Gen_Data :: Get_Number (const char* label, int index)
{
  if (!strcmp(Get_Number_Label (index), label) )
    return Get_Number (index);
  else
    return Get_Number (label);
}

int Gen_Data :: Get_Number (const char* label)
{
  for (int i = 0; i < int_count; i++)
    if (!strcmp (int_labels[i], label) ) 
      return int_data[i];
  return 0;
}

int Gen_Data :: Get_Number_Index (const char* label)
{
  for (int i = 0; i < int_count; i++)
    if (!strcmp (int_labels[i], label) ) 
      return i;
  return -1;
}

int Gen_Data :: Get_Number (int index)
{
  if (index >= 0 && index < int_count) 
    return int_data[index];
  return 0;
}

char* Gen_Data :: Get_Number_Label (int index)
{
  if (index >= 0 && index < int_count) 
    return int_labels[index];
  return NULL;
}

Gen_Data* Gen_Data :: Get_Gen_Data (const char* label, int index)
{
  if (!strcmp(Get_Gen_Data_Label (index), label) )
    return Get_Gen_Data (index);
  else
    return Get_Gen_Data (label);
}

Gen_Data* Gen_Data :: Get_Gen_Data (const char* label)
{
  for (int i = 0; i < data_count; i++)
    if (!strcmp (data_labels[i], label) ) 
      return data_gen[i];
  return NULL;
}

int Gen_Data :: Get_Gen_Data_Index (const char* label)
{
  for (int i = 0; i < data_count; i++)
    if (!strcmp (data_labels[i], label) ) 
      return i;
  return -1;
}

Gen_Data* Gen_Data :: Get_Gen_Data (int index)
{
  if (index >= 0 && index < data_count) 
    return data_gen[index];
  return NULL;
}

char* Gen_Data :: Get_Gen_Data_Label (int index)
{
  if (index >= 0 && index < data_count) 
    return data_labels[index];
  return NULL;
}

Gen_Data** Gen_Data :: Get_Array (const char* label, int index)
{
  if (!strcmp(Get_Array_Label (index), label) )
    return Get_Array (index);
  else
    return Get_Array (label);
}

Gen_Data** Gen_Data :: Get_Array (const char* label)
{
  for (int i = 0; i < array_count; i++)
    if (!strcmp (arrays_labels[i], label) ) 
      return arrays_gen[i];
  return NULL;
}

int Gen_Data :: Get_Array_Index (const char* label)
{
  for (int i = 0; i < array_count; i++)
    if (!strcmp (arrays_labels[i], label) ) 
      return i;
  return -1;
}

int Gen_Data :: Get_Array_Index (const char* label, int index)
{
  if (!strcmp (Get_Array_Label (index), label))
    return index;
  else
    return Get_Array_Index (label);
}

Gen_Data** Gen_Data :: Get_Array (int index)
{
  if (index >= 0 && index < array_count) 
    return arrays_gen[index];
  return NULL;
}

char* Gen_Data :: Get_Array_Label (int index)
{
  if (index >= 0 && index < array_count) 
    return arrays_labels[index];
  return NULL;
}

Gen_Data* Gen_Data :: Get_Element (const char* label, int array_num, int index)
{
  if (!strcmp (Get_Array_Label (array_num), label) )
    return Get_Element (array_num, index);
  else
    return Get_Element (label, index);
}

Gen_Data* Gen_Data :: Get_Element (const char* label, int index)
{
  for (int i = 0; i < array_count; i++)
  {
    if (!strcmp (arrays_labels[i], label) )
      if (index >= 0 && index < arrays_count[i])
        return arrays_gen[i][index];
      else
        return NULL;
  }
  return NULL;
}

Gen_Data* Gen_Data :: Get_Element (int array_num, int index)
{
  if (array_num >= 0 && array_num < array_count)
    if (index >= 0 && index < arrays_count[array_num])
      return arrays_gen[array_num][index];
  return NULL;
}

void Gen_Data :: Point_Data ( construct_list* list )
{
  char* tmp_str;
  int   tmp_int;
  
  for( int i = 0; list[i].datatype != -1; i++ )
  {
    switch( list[i].datatype )
    {
      case StrType:
        tmp_str = Extract_String( list[i].label, list[i].index );
        if( tmp_str != NULL )
        {
          *((char**) list[i].pointer) = tmp_str;
          record_new( strlen( tmp_str )+1, -list[i].mem_type );
        }
        break;
      
      case IntType:
        if( Get_Int( tmp_int, list[i].label, list[i].index ) )
        {
          *((int *) list[i].pointer) = tmp_int;
          if( list[i].mem_type != -1 )
            record_new( sizeof(int), -list[i].mem_type );
        }
        break;
      
      case GddType:
        break;
    }
  
  }
}

bool Gen_Data :: ConvertTo ( extra_data*& extra )
{  
  if( strcmp ( section, gdExtraLabel ) )
    return FALSE;
  
  extra = new extra_data;

  construct_list list [] = {
    { StrType, gdExtraKeywords, 0, &extra->keyword, MEM_EXTRA },
    { StrType, gdText,          1, &extra->text,    MEM_EXTRA },
    { -1,      NULL,            0, NULL,            0         }
  };
  
  Point_Data( list );

  return TRUE;
}

bool Gen_Data :: ConvertTo ( action_data*& action )
{
  int         count;
  int         index;
  extra_data* extra;
  gen_data*   gdata;
  
  if( strcmp ( section, gdActionLabel ) )
    return FALSE;
  
  action = new action_data;
  
  construct_list list [] = {
    { StrType, gdCommand,    0, &action->command, MEM_ACTION },
    { StrType, gdTarget,     1, &action->target,  MEM_ACTION },
    { StrType, gdCode,       2, &action->code,    MEM_ACTION },
    { IntType, gdTrigger,    0, &action->trigger, -1         },
    { IntType, gdValue,      1, &action->value,   -1         },
    { IntType, gdFlags,      2, &action->flags,   -1         },
    { -1,      NULL,         0, NULL,             0          }
  };
  
  Point_Data( list );
  
  if( ( index = Get_Array_Index( gdActionData, 0 ) ) != -1 )
  {
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
    {
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( extra ) )
        action->data += extra;
    }
  }
  
  return TRUE;
}

bool Gen_Data :: ConvertTo ( reset_data*& reset )
{
  if( strcmp( section, gdResetLabel ) )
    return FALSE;
  
  reset = new reset_data;
  
  construct_list list [] = {
    { IntType, gdResetTarget, 0, &reset->vnum,    -1         },
    { IntType, gdResetFlags,  1, &reset->flags,   -1         },
    { IntType, gdChances,     2, &reset->chances, -1         },
    { IntType, gdResetValue,  3, &reset->value,   -1         },
    { IntType, gdLiquid,      4, &reset->liquid,  -1         },
    { -1,      NULL,          0, NULL,             0         }
  };
  
  Point_Data( list );
  
  return TRUE;  
}

bool Gen_Data :: ConvertTo ( exit_data*& exit )
{
  if( strcmp( section, gdExitLabel ) )
    return FALSE;
  
  exit = new exit_data;
  
  construct_list list [] = {
    { StrType, gdExitName,     0, &exit->name,      MEM_EXIT },
    { StrType, gdExitKeywords, 1, &exit->keywords,  MEM_EXIT },
    { IntType, gdDirection,    0, &exit->direction, -1         },
    { IntType, gdExitInfo,     1, &exit->exit_info, -1         },
    { IntType, gdKey,          2, &exit->key,       -1         },
    { IntType, gdTarget,       3, &exit->to_room,   -1         },
    { IntType, gdStrength,     4, &exit->strength,  -1         },
    { IntType, gdLight,        5, &exit->light,     -1         },
    { IntType, gdExitSize,     6, &exit->size,      -1         },
    { -1,      NULL,           0, NULL,              0         }
  };
  
  Point_Data( list );
  
  return TRUE;
}

extern exit_data* add_exit( room_data*, int );

bool Gen_Data :: ConvertTo ( room_data*& room )
{
  int           count;
  int           index;
  exit_data*     exit;
  extra_data*   extra;
  action_data* action;
  reset_data*   reset;
  gen_data*     gdata;
  
  if( strcmp( section, gdRoomLabel ) )
    return FALSE;
  
  room = new room_data;
  
  construct_list list [] = {
    { StrType, gdRoomName,     0, &room->name,        MEM_ROOM },
    { StrType, gdDescription,  1, &room->description, MEM_ROOM },
    { StrType, gdComments,     2, &room->comments,    MEM_ROOM },
    { IntType, gdVNum,         0, &room->vnum,        -1       },
    { IntType, gdRoomFlags,    1, &room->room_flags,  -1       },
    { IntType, gdSectorType,   2, &room->sector_type, -1       },
    { IntType, gdSize,         3, &room->size,        -1       },
    { -1,      NULL,           0, NULL,                0       }
  };
  
  Point_Data( list );
  
  if( ( index = Get_Array_Index( gdExits, 0 ) ) != -1 )
  {
    exit_data* tmp_exit;
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( tmp_exit ) )
      {
        exit = add_exit( room, tmp_exit->direction );
        memcpy( exit, tmp_exit, sizeof( exit_data ) );
        record_delete( sizeof( exit_data ), MEM_EXIT );
      }
  }
  
  if( ( index = Get_Array_Index( gdExtras, 1 ) ) != -1 )
  {
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( extra ) )
        room->extra_descr += extra;
  }
  
  if( ( index = Get_Array_Index( gdActions, 2 ) ) != -1 )
  {
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( action ) )
      {
        action->room = room;
        append( room->action, action );
      }
  }
  
  if( ( index = Get_Array_Index( gdResets, 3 ) ) != -1 )
  {
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( reset ) )
        append( room->reset, reset );
  }
    
  return TRUE;
}

bool Gen_Data :: ConvertTo ( area_data*& area )
{
  int           count;
  int           index;
  room_data*     room;
  gen_data*     gdata;
  
  if( strcmp( section, gdAreaLabel ) )
    return FALSE;
  
  area = new area_data;
  
  construct_list list [] = {
    { StrType, gdAreaName,     0, &area->name,        MEM_AREA },
    { StrType, gdCreator,      1, &area->creator,     MEM_AREA },
    { StrType, gdHelp,         2, &area->help,        MEM_AREA },
    { IntType, gdLevel,        0, &area->level,       -1       },
    { IntType, gdResetTime,    1, &area->reset_time,  -1       },
    { IntType, gdStatus,       2, &area->status,      -1       },
    { -1,      NULL,           0, NULL,                0       }
  };
  
  Point_Data( list );
  
  if( ( index = Get_Array_Index( gdRooms, 0 ) ) != -1 )
  {
    exit_data* tmp_exit;
    count = Get_Array_Count( index );
    
    for( int i = 0; i < count; i++ )
      if( ( gdata = Get_Element( index, i ) ) != NULL
        && gdata->ConvertTo( room ) )
      {
        room->area = area;
        if( area->room_first == NULL ) {
          area->room_first = room;
          room->next = NULL;
        }
        else
          if( area->room_first->vnum > room->vnum ) {
            room->next = area->room_first;
            area->room_first = room;
          }
          else
          {
            room_data* room2;
            for( room2 = area->room_first; room2->next != NULL
              && room2->next->vnum < room->vnum; room2 = room2->next ) ;
            room->next = room2->next;
            room2->next = room;
          }
      }
  }
  return TRUE;
}


/*  14/5/2000 -- OLD CODE */
/*
void* Gen_Data :: Convert ( )
{
  char* temp_string;
  int temp_count;
  int temp_count2;

  if (!strcmp (section, gdActionLabel) ) {
  
    action_data* new_action = new action_data;

    if ((temp_string = Get_String (gdCommand, 0)) != NULL) {
      new_action->command = alloc_string (temp_string, MEM_ACTION);
    }
    if ((temp_string = Get_String (gdTarget, 1)) != NULL) {
      new_action->target = alloc_string (temp_string, MEM_ACTION);
    }
    if ((temp_string = Get_String (gdCode, 2)) != NULL) {
      new_action->code = alloc_string (temp_string, MEM_ACTION);
    }
    
    new_action->trigger = Get_Number (gdTrigger, 0);
    new_action->value = Get_Number (gdValue, 1);
    new_action->flags = Get_Number (gdFlags, 2);
    
    temp_count = Get_Array_Index (gdActionData, 0);
    temp_count2 = Get_Array_Count (temp_count);
    for (int i = 0; i < temp_count2; i++) 
      new_action->data += (extra_data*) Get_Element (temp_count, i)->Convert ();

    return new_action;
  
  }
  
  if (!strcmp (section, gdResetLabel) ) {
    
    reset_data* new_reset = new reset_data;
    
    new_reset->vnum = Get_Number (gdResetTarget, 0);
    new_reset->flags = Get_Number (gdResetFlags, 1);
    new_reset->chances = Get_Number (gdChances, 2);
    new_reset->value = Get_Number (gdResetValue, 3);
    new_reset->liquid = Get_Number (gdLiquid, 4);
  
    return new_reset;
  
  }
  
  if (!strcmp (section, gdExitLabel) ) {
  
    exit_data* new_exit = new exit_data;
    
    if ((temp_string = Get_String (gdExitName, 0)) != NULL) {
      new_exit->name = alloc_string (temp_string, MEM_EXIT);
    }
    if ((temp_string = Get_String (gdExitKeywords, 1)) != NULL) {
      new_exit->keywords = alloc_string (temp_string, MEM_EXIT);
    }

    new_exit->direction = Get_Number (gdDirection, 0);
    new_exit->exit_info = Get_Number (gdExitInfo, 1);
    new_exit->key = Get_Number (gdKey, 2);
    new_exit->to_room = (room_data*) Get_Number (gdTarget, 3);
    new_exit->strength = Get_Number (gdStrength, 4);
    new_exit->light = Get_Number (gdLight, 5);
    new_exit->size = Get_Number (gdExitSize, 6);
    
    return new_exit;
   
  }
  
  if (!strcmp (section, gdExtraLabel) ) {
  
    extra_data* new_extra = new extra_data;
 
    if ((temp_string = Get_String (gdExtraKeywords, 0)) != NULL) {
      new_extra->keyword = alloc_string (temp_string, MEM_EXTRA);
    }
    if ((temp_string = Get_String (gdText, 1)) != NULL) {
      new_extra->text = alloc_string (temp_string, MEM_EXTRA);
    }
    
    return new_extra;
  
  }
  
  if (!strcmp (section, gdRoomLabel) ) {
  
    room_data* new_room = new room_data;
    
    if ((temp_string = Get_String (gdRoomName, 0)) != NULL) {
      new_room->name = alloc_string (temp_string, MEM_ROOM);
    }
    if ((temp_string = Get_String (gdDescription, 1)) != NULL) {
      new_room->description = alloc_string (temp_string, MEM_ROOM);
    }
    if ((temp_string = Get_String (gdComments, 2)) != NULL) {
      new_room->comments = alloc_string (temp_string, MEM_ROOM);
    }

    new_room->vnum = Get_Number (gdVNum, 0);
    new_room->room_flags = Get_Number (gdRoomFlags, 1);
    new_room->sector_type = Get_Number (gdSectorType, 2);
    new_room->size = Get_Number (gdSize, 3);
    
    temp_count = Get_Array_Index (gdExits, 0);
    temp_count2 = Get_Array_Count (temp_count);
    exit_data* temp_exit;
    for (int i = 0; i < temp_count2; i++) {
      temp_exit = (exit_data*) Get_Element (temp_count, i)->Convert ();
      for( int j = 0; ; j++ ) 
        if( j >= new_room->exits.size || temp_exit->direction < new_room->exits[j]->direction ) {
          insert( new_room->exits.list, new_room->exits.size, temp_exit, j );
          break;
        }
    }
    
    temp_count = Get_Array_Index (gdExtras, 1);
    temp_count2 = Get_Array_Count (temp_count);
    for (int i = 0; i < temp_count2; i++) 
      new_room->extra_descr += (extra_data*) Get_Element (temp_count, i)->Convert ();
    
    temp_count = Get_Array_Index (gdActions, 2);
    temp_count2 = Get_Array_Count (temp_count);
    new_room->action = NULL;
    action_data* temp_action;
    for (int i = 0; i < temp_count2; i++) {
      temp_action = (action_data*) Get_Element (temp_count, i)->Convert ();
      temp_action->room = new_room;
      append (new_room->action, temp_action);
    }

    temp_count = Get_Array_Index (gdResets, 3);
    temp_count2 = Get_Array_Count (temp_count);
    new_room->reset = NULL;
    reset_data* temp_reset;
    for (int i = 0; i < temp_count2; i++) {
      temp_reset = (reset_data*) Get_Element (temp_count, i)->Convert ();
      append (new_room->reset, temp_reset);
    }
    
    return new_room;
  
  }
  
  if (!strcmp (section, gdAreaLabel) ) {
  
    area_data* new_area = new area_data;
    
    if ((temp_string = Get_String (gdAreaName, 0)) != NULL) {
      new_area->name = alloc_string (temp_string, MEM_AREA);
    }
    else {
      panic ( "Convert failure!!!!" );
    }
    if ((temp_string = Get_String (gdCreator, 1)) != NULL) {
      new_area->creator = alloc_string (temp_string, MEM_AREA);
    }
    if ((temp_string = Get_String (gdHelp, 2)) != NULL) {
      new_area->help = alloc_string (temp_string, MEM_AREA);
    }
    
    new_area->level = Get_Number (gdLevel, 0);
    new_area->reset_time = Get_Number (gdResetTime, 1);
    new_area->status = Get_Number (gdStatus, 2);

    temp_count = Get_Array_Index (gdRooms, 0);
    temp_count2 = Get_Array_Count (temp_count);
    room_data* temp_room;
    room_data* temp_room2;
    for (int i = 0; i < temp_count2; i++) {
      temp_room = (room_data*) Get_Element (temp_count, i)->Convert ();
      temp_room->area = new_area;
      if( ( temp_room2 = temp_room->area->room_first ) == NULL ) {
        temp_room->area->room_first = temp_room;
        temp_room->next = NULL;
      }
      else {
        if( temp_room2->vnum > temp_room->vnum ) {
          temp_room->next = temp_room2;
          temp_room->area->room_first = temp_room;
        }
        else {
        // This code looks Really dodgy ... but hmm ... err ... i copied it from room.cc
          for( ; temp_room2->next != NULL && temp_room2->next->vnum < temp_room->vnum; temp_room2 = temp_room2->next);
          temp_room->next = temp_room2->next;
          temp_room2->next = temp_room;
        }
      }
    }
    
    return new_area;
  }
  
  return NULL;
}
*/