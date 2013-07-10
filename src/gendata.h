/*
 * Gen_Data Type - does it all! Smith -- I hope ... 29-4-2000
*/
#define LABEL_SIZE             4 /* can't change this because of a mistake */

#define gdStr                  0
#define gdInt                  1
#define gdData                 2
#define gdArray                3
typedef class Gen_Data gen_data;

/* DataTypes used in Construct_List */
#define  StrType    0
#define  IntType    1
#define  GddType    2

typedef class Construct_List
{
 public:
  char        datatype;
  const char*    label;
  int            index;
  void*        pointer;
  int         mem_type;
} construct_list;

class Gen_Data
{
 private:
/* Section Label */
  char* section;
/* String Data with Labels */
  int str_count;
  char** str_labels;
  char** str_data;
/* Int Data with Labels */
  int int_count;
  char** int_labels;
  int* int_data;
/* Gen Data with Labels */
  int data_count;
  char** data_labels;
  Gen_Data** data_gen;
/* Gen Data Arrays with Labels */
  int array_count;
  int* arrays_count;
  char** arrays_labels;
  Gen_Data*** arrays_gen;
  
  void Initialize ( );
  
/* Counts may not be nescery could be implemented with null termination?*/
 public:
  Gen_Data ( ); /* Blank Check Style */
  /*
  Gen_Data (Gen_Data* original);  Copy Constrution Is this wanted?
  */
  
  /* File -> Gen_Data reader */
  
  Gen_Data ( FILE* );
  
  /* MUD usable -> Gen_Data converters */
  
  Gen_Data     ( area_data* );
  Gen_Data     ( room_data* ); 
  Gen_Data     ( exit_data* );
  Gen_Data     ( extra_data* );
  Gen_Data     ( action_data* );
  Gen_Data     ( reset_data* );
  
  /* Gen_Data -> Mud usable converters */
  
  bool    ConvertTo      ( area_data*& );
  bool    ConvertTo      ( room_data*& );
  bool    ConvertTo      ( exit_data*& );
  bool    ConvertTo      ( extra_data*& );
  bool    ConvertTo      ( action_data*& );
  bool    ConvertTo      ( reset_data*& );
    
  virtual ~Gen_Data ( );  /* Destructor ... ENSURE counts are correct */

  void Load (FILE *fp);  /* Simple Loading of structure */
  void Save (FILE *fp);  /* Simple Saving of structure */
  
  /* Same as Get_String except NULLs the original pointer */
  char*   Extract_String           ( const char*, int );
  bool    Get_Int                  ( int&, const char*, int );
  
  /* Fills the variables listed in construct_list* */
  void    Point_Data               ( construct_list* );
  
  /* Set Counters - Will clear Associated Data ... or will it*/
  void Set_Count (int count_type, int counter);
  /* Simple Get_Count */
  int Get_Count (int count_type);

  /* Set Array Counters - Will clear Associated Data ... or will it*/
  void Set_Array_Count (int array_num, int counter);
  /* Simple Get_Array_Count */
  int Get_Array_Count (int array_num);

  /* Stucture name setting */
  void Set_Section (const char* name);  

  /* Add Routines ... should they be consolidated under one name? 
     can c tell difference between Gen_Data* and Gen_Data** in pass?
  */

  /* Add a String with Label */
  void Add_String (const char* label, const char* data);
  /* Add an Int with Label */
  void Add_Number (const char* label, int data);
  /* Add a Gen_Data with Label */
  void Add_Gen_Data (const char* label, Gen_Data* data);
  /* Add an Array with Label appropriate Counter MUST be correct*/
  void Add_Array (const char* label, Gen_Data** data = NULL);
  /* Add an Array Element */
  void Add_Element (int array_num, Gen_Data* data);

  /* Get structure name */
  char* Get_Section ( );

  /* Get String by Label */
  char* Get_String (const char* label);
  /* Get Int by Label */
  int Get_Number (const char* label);
  /* Get Gen_Data by Label */
  Gen_Data* Get_Gen_Data (const char* label);
  /* Get Array by Label*/
  Gen_Data** Get_Array (const char* label);

  /* Get String by Index */
  char* Get_String (int index);
  /* Get Int by Index */
  int Get_Number (int index);
  /* Get Gen_Data by Index */
  Gen_Data* Get_Gen_Data (int index);
  /* Get Array by Index*/
  Gen_Data** Get_Array (int index);

  /* Get String label by Index */
  char* Get_String_Label (int index);
  /* Get Int label by Index */
  char* Get_Number_Label (int index);
  /* Get Gen_Data label by Index */
  char* Get_Gen_Data_Label (int index);
  /* Get Array label by Index */
  char* Get_Array_Label (int index);

  /* Get String Index by Label */
  int Get_String_Index (const char* label);
  /* Get Int Index by Label */
  int Get_Number_Index (const char* label);
  /* Get Gen_Data Index by Label */
  int Get_Gen_Data_Index (const char* label);
  /* Get Array Index by Label*/
  int Get_Array_Index (const char* label);
  
  /* Get Array Index by Label, with pre_check */
  int Get_Array_Index (const char* label, int index);

  /* Get String by Index/label */
  char* Get_String (const char* label, int index);
  /* Get Int by Index/label */
  int Get_Number (const char* label, int index);
  /* Get Gen_Data by Index/label */
  Gen_Data* Get_Gen_Data (const char* label, int index);
  /* Get Array by Index/label*/
  Gen_Data** Get_Array (const char* label, int index);


  /* Get Element by Label/Index & Index */
  Gen_Data* Get_Element (const char* label, int array_num, int index);
  /* Get Element by Label & Index */
  Gen_Data* Get_Element (const char* label, int index);
  /* Get Element by dual Index */
  Gen_Data* Get_Element (int array_num, int index);

  /* Consider adding item deletion */
};
