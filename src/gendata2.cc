#include <stdio.h>
#include "define.h"
#include "struct.h"

#define GEN_DATA_LABEL_LENGTH  4 /* should be the same as LABEL_SIZE */
#define DATA_LABEL_LENGTH      4 /* should be the same as LABEL_SIZE */
#define ITEM_LABEL_LENGTH      4 /* should be the same as LABEL_SIZE */

#define StringLABEL      "STR!"
#define IntegerLABEL     "INT!"
#define GenDataLABEL     "GDta"
#define GenArrayLABEL    "GAry"
#define GenDataEndLABEL  "END!"

typedef unsigned char    UInt8;
typedef signed char      SInt8;
typedef unsigned short   UInt16;
typedef signed short     SInt16;
typedef unsigned long    UInt32;
typedef signed long      SInt32;

void Gen_Data_Save_STR     ( int, char**, char**, FILE* );
void Gen_Data_Save_INT     ( int, char**, int*, FILE* );
void Gen_Data_Save_GENDATA ( int, char**, Gen_Data**, FILE* );

void Gen_Data_Load_STR     ( gen_data *, FILE * );
void Gen_Data_Load_INT     ( gen_data *, FILE * );
void Gen_Data_Load_GENDATA ( gen_data *, FILE * );

class Gen_Data_Types
{
 public:
   const char*     label;
   void            ( *load_func ) ( gen_data*, FILE* );
};

Gen_Data_Types gen_data_type_list [] =
{
  { StringLABEL,  Gen_Data_Load_STR     },
  { IntegerLABEL, Gen_Data_Load_INT     },
  { GenDataLABEL, Gen_Data_Load_GENDATA },
  { "",           NULL                  }
};

void Gen_Data :: Save (FILE *fp)
{
  fwrite(section, GEN_DATA_LABEL_LENGTH, 1, fp);
  
  if(str_count > 0)
  {
    fwrite(StringLABEL, DATA_LABEL_LENGTH, 1, fp);
    Gen_Data_Save_STR (str_count, str_labels, str_data, fp);
  }
  
  if(int_count > 0)
  {
    fwrite(IntegerLABEL, DATA_LABEL_LENGTH, 1, fp);
    Gen_Data_Save_INT (int_count, int_labels, int_data, fp);
  }
  
  if(data_count > 0)
  {
    fwrite(GenDataLABEL, DATA_LABEL_LENGTH, 1, fp);
    Gen_Data_Save_GENDATA (data_count, data_labels, data_gen, fp);
  }
  
  if(array_count > 0)
  {
    UInt16 data_count16;
    UInt32 data_count32;
    
    fwrite(GenArrayLABEL, DATA_LABEL_LENGTH, 1, fp);
    data_count16 = (UInt16) array_count;
    fwrite(&data_count16, sizeof(UInt16), 1, fp);
    for(int i = 0; i < array_count; i++)
    {
      fwrite(arrays_labels[i], DATA_LABEL_LENGTH, 1, fp);
      data_count32 = (UInt32) arrays_count[i];
      fwrite(&data_count32, sizeof(UInt32), 1, fp);
      for(int j = 0; j < arrays_count[i]; j++)
        arrays_gen[i][j]->Save(fp);
    }
  }
  fwrite(GenDataEndLABEL, DATA_LABEL_LENGTH, 1, fp);
}

void Gen_Data_Save_STR (int count, char** label_array, char** data_array,
  FILE *fp)
{
  UInt16   str_length;
  UInt16 data_count16;
  
  data_count16 = (UInt16) count;
  fwrite(&data_count16, sizeof(UInt16), 1, fp);
  
  for(int i = 0; i < count; i++)
  {
    fwrite(label_array[i], ITEM_LABEL_LENGTH, 1, fp);

    str_length = strlen(data_array[i]);
    fwrite(&str_length, sizeof(UInt16), 1, fp);
    fwrite(data_array[i], sizeof(char), str_length, fp);
  }
}

void Gen_Data_Save_INT (int count, char** label_array, int* data_array,
  FILE *fp)
{
  UInt16 data_count16;
  
  data_count16 = (UInt16) count;
  fwrite(&data_count16, sizeof(UInt16), 1, fp);
  
  for(int i = 0; i < count; i++)
  {
    fwrite(label_array[i], ITEM_LABEL_LENGTH, 1, fp);
    fwrite(&data_array[i], sizeof(int), 1, fp);
  }
}


void Gen_Data_Save_GENDATA (int count, char** label_array,
  Gen_Data** data_array, FILE *fp)
{
  UInt32 data_count32;
  
  data_count32 = (UInt32) count;
  fwrite(&data_count32, sizeof(UInt32), 1, fp);
    
  for(int i = 0; i < count; i++)
  {
    fwrite(label_array[i], ITEM_LABEL_LENGTH, 1, fp);
    data_array[i]->Save(fp);
  }
}

void Gen_Data :: Load (FILE *fp)
{
  char       label [ LABEL_SIZE+1 ];
  long       datalength;
  
  label[LABEL_SIZE] = '\0';

  if( fread( label, GEN_DATA_LABEL_LENGTH, 1, fp ) < 1 )
    panic( "Gen_Data Load: missing section label" );
   
  Set_Section( label );
  
  for( ; ; )
  {
    if( fread( label, GEN_DATA_LABEL_LENGTH, 1, fp ) < 1 )
      panic( "Gen_Data Load: no data label" );
            
      if( !strcmp( label, GenDataEndLABEL ) )
        break;
      
      if( !strcmp( label, GenArrayLABEL ) )
      {
        UInt16 data_count16;
        UInt32 data_count32;
        
        if( fread(&data_count16, sizeof(UInt16), 1, fp) < 1 )
          panic( "Gen_Data Load: can't get array count" );
        
        Set_Count( gdArray, data_count16 );
        for(int i = 0; i < data_count16; i++)
        {
          if( fread( label, DATA_LABEL_LENGTH, 1, fp) < 1 )
            panic( "Gen_Data Load: can't get array label" );
          
          Add_Array( label, NULL );
          
          if( fread( &data_count32, sizeof(UInt32), 1, fp) < 1 )
            panic( "Gen_Data Load: can't get array length" );
          
          Set_Array_Count( i, data_count32 );
          for(int j = 0; j < data_count32; j++)
            arrays_gen[i][j] = new Gen_Data( fp );
        }
      }
      else
          for( int i = 0; ; i++ )
          {
            if( gen_data_type_list[i].label == '\0' )
              panic( "Gen_Data Load: unrecognised data label" );
            
            if( !strcmp( label, gen_data_type_list[i].label ) )
            {
              ( gen_data_type_list[i].load_func ) ( this, fp );
              break;
            }
          }
  }
}

void Gen_Data_Load_STR ( gen_data *gd, FILE *fp )
{
  char                     label [ LABEL_SIZE+1 ];
  char                    buffer [ MAX_STRING_LENGTH * 3 ];
  UInt16            data_count16;
  unsigned short   string_length;
  
  if( fread(&data_count16, sizeof(UInt16), 1, fp) < 1 )
    panic( "Gen_Data Load: STR: can't get string count" );
  
  gd->Set_Count( gdStr, data_count16 );
  
  label[LABEL_SIZE] = '\0';
  
  for(int i = 0; i < data_count16; i++)
  {
    if( fread( label, ITEM_LABEL_LENGTH, 1, fp ) < 1 )
      panic( "Gen_Data Load: STR: can't get string label" );
    
    if( fread( &string_length, sizeof(unsigned short), 1, fp) < 1 )
      panic( "Gen_Data Load: STR: can't get string length" );
    
    if( fread( buffer, sizeof(char), string_length, fp ) < string_length )
      panic( "Gen_Data Load: STR: can't get string" );
    
    buffer[string_length] = '\0';

    gd->Add_String( label, buffer );
  }
}

void Gen_Data_Load_INT ( gen_data *gd, FILE *fp )
{
  char           label [ LABEL_SIZE+1 ];
  UInt16  data_count16;
  int         int_data;
  
  if( fread(&data_count16, sizeof(UInt16), 1, fp) < 1 )
    panic( "Gen_Data Load: INT: can't get int count" );
  
  gd->Set_Count( gdInt, data_count16 );

  label[LABEL_SIZE] = '\0';
  
  for(int i = 0; i < data_count16; i++)
  {
    if( fread( label, ITEM_LABEL_LENGTH, 1, fp ) < 1 )
      panic( "Gen_Data Load: INT: can't get int label" );

    if( fread( &int_data, sizeof(int), 1, fp) < 1 )
      panic( "Gen_Data Load: INT: can't get int" );
    
    gd->Add_Number( label, int_data );
  }
}


void Gen_Data_Load_GENDATA ( gen_data *gd, FILE *fp )
{
  char             label [ LABEL_SIZE+1 ];
  UInt32    data_count32;

  if( fread(&data_count32, sizeof(UInt16), 1, fp) < 1 )
    panic( "Gen_Data Load: DATA: can't get data count" );
  
  gd->Set_Count( gdData, data_count32 );

  label[LABEL_SIZE] = '\0';

  for(int i = 0; i < data_count32; i++)
  {
    if( fread( label, ITEM_LABEL_LENGTH, 1, fp ) < 1 )
      panic( "Gen_Data Load: DATA: can't get data label" );
    
    gd->Add_Gen_Data( label, new Gen_Data( fp ) );
  }
}


