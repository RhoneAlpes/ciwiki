/*
 *      wikiform.c
 *      
 *      Copyright 2010 jpr <inphilly@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
/*********************************************************************/
/* Here we modify the page (raw text) in order to insert an entry or
 * to delete an entry in the page. 
 */

#include "ci.h"

/* need to be improved: Not strict enough! */
char *wiki_add_entry(char *filename, char *entry, char *form_nb) 
{
  struct stat st;
  FILE*       fp;
  char*       str;
  char*       result;
  char*       str_ptr;
  char*       str_ptr0;
  char*       str_ptr1;
  char        tag[255];
  int         len,lg;
  int         form_nb_int = atoi(form_nb);

  /* Get the file size. */
  if (stat(filename, &st)) 
    return NULL;
  /* file exist? */
  if (!(fp = fopen(filename, "rb"))) 
    return NULL;
  time_t now;
  (void) time(&now);
  /* Data to insert= checkbox + entry */
  st.st_size+= asprintf(&result,"\n{{checkbox=%li;0}} %s\n",(long)now,entry);
  /* allocate mem for file + data */
  str = (char *)malloc(sizeof(char)*(st.st_size + 1));
  /* load the file */
  len = fread(str, 1, st.st_size, fp);
  if (len >= 0) str[len] = '\0';
  fclose(fp);
  /* search location of 'data' tag, data will inserted under it 
   * 2 possibilities  {{data#}} or {{data}} 
   * search is weak.
   * */
  int data_cnt = 0;
  int data_num = 0;
  str_ptr1=str;
  /* search the opening of tag */
  while ( (str_ptr0=strstr(str_ptr1,"{{")) )
  {
    str_ptr0+=2;
    if ( *(str_ptr0-3) != '!'  && (str_ptr1=strstr(str_ptr0,"}}")) )
    {
      lg = str_ptr1-str_ptr0;
      if ( lg > 255 ) lg=255;
      str_ptr1+=2;
      strncpy(tag,str_ptr0,lg);
      tag[lg]='\0';
      /* search tag "data" */
      if ( (str_ptr=strstr(tag,"data")) )
      {
        str_ptr+=4;
        /* if digit attached at data */
        if ( isdigit( *str_ptr ) )
          data_num = atoi(str_ptr);
        else
        {
          data_num = 0;
          data_cnt++;
        }
      
        if ( form_nb_int == data_num || form_nb_int == data_cnt )
        {
          /* insert the checkbox and the entry */
          char *movethis = strdup(str_ptr1);
          *(str_ptr1)='\0';
          strcat(str,result);
          strcat(str,movethis);
          return str; //return the new page
        }    
      }
    }
    else
      str_ptr1 = str_ptr0;
  }
  return str; //tag not found, return page unchanged
}

/* need to be improved */
char *wiki_delete_entry(char *filename, char *list) 
{
  struct stat st;
  FILE*       fp;
  char*       str;
  char        *str_ptr1,*str_ptr2,*str_ptr3,*str_ptr4;
  int         len,lg;
  char        index[16];
  int         i,value;
  char        *key;

  /* Get the file size. */
  if (stat(filename, &st)) 
    return NULL;
  /* file exist? */
  if (!(fp = fopen(filename, "rb"))) 
    return NULL;
  /* allocate mem for file */
  str = (char *)malloc(sizeof(char)*(st.st_size + 1));
  /* load the file */
  len = fread(str, 1, st.st_size, fp);
  if (len >= 0) str[len] = '\0';
  fclose(fp);
  
  str_ptr1=list;
  
  /* foreach checkbox checked */
  while ( (str_ptr2=strchr(str_ptr1,'=')) )
  {
    lg=str_ptr2-str_ptr1;
    if ( lg < 16 )
      strncpy(index, str_ptr1, lg);
    else
      lg=0;
      
    index[lg]='\0';
    str_ptr1 = str_ptr2+1; //point the value
    
    if( (str_ptr2=strchr(str_ptr1,';')) )
    {
      if ( *(str_ptr1) == '1' )
        value = 1;
      else
        value = 0;
          
      str_ptr1 = str_ptr2+1; //point the next index
    }
    else
      break; //!end of the loop if we are here, there is a bug
    
    if ( value == 1 )
    {
      /* search location of checkbox and delete the whole line 
       * delete extra \n and \r too.
       * !!! stict search of {{checkbox=xxxxx}} */
      lg=asprintf(&key,"{{checkbox=%s;",index);
      if ( (str_ptr3=strstr(str,key)) ) //search the beginning
        if ( (str_ptr4=strpbrk(str_ptr3+lg,"\n\r")) ) //search the end
        {
          /* if more than 2 \n or \r , extra eol will be deleted */
          i=0;
          while ( *(str_ptr4+i) == '\n' || *(str_ptr4+i) == '\r' )
            i++;
          if (i > 1)
            str_ptr4+= i;
          /* move right to left */
          strcpy(str_ptr3,str_ptr4);
        }
    }
  }
    
  return str;
}


/* need to be improved */
char *wiki_set_checkbox(char *filename, char *list) 
{
  struct stat st;
  FILE*       fp;
  char*       str;
  char        *str_ptr1,*str_ptr2,*str_ptr3;
  int         len,lg;
  char        index[16];
  int         value;
  char        *key;

  /* Get the file size. */
  if (stat(filename, &st)) 
    return NULL;
  /* file exist? */
  if (!(fp = fopen(filename, "rb"))) 
    return NULL;
  /* allocate mem for file */
  str = (char *)malloc(sizeof(char)*(st.st_size + 1));
  /* load the file */
  len = fread(str, 1, st.st_size, fp);
  if (len >= 0) str[len] = '\0';
  fclose(fp);
  
  str_ptr1=list;
  
  /* foreach checkbox checked */
  while ( (str_ptr2=strchr(str_ptr1,'=')) )
  {
    lg=str_ptr2-str_ptr1;
    if ( lg < 16 )
      strncpy(index, str_ptr1, lg);
    else
      lg=0;
    index[lg]='\0';
    
    str_ptr1 = str_ptr2+1; //point the value
    
    if( (str_ptr2=strchr(str_ptr1,';')) )
    {
      if ( *(str_ptr1) == '1' )
        value = 1;
      else
        value = 0;

      str_ptr1 = str_ptr2+1; //point the next index
    }
    else
      break; //!end of the loop, there is a bug
    
    /* search location of checkbox and delete the whole line 
     * stict search but {{checkbox=xxxxx;0}} is automatically created */
    lg=asprintf(&key,"{{checkbox=%s;",index);
    if ( (str_ptr3=strstr(str,key)) )
    {
      if ( value == 1 )
        *(str_ptr3+lg) = '1';
      else
        *(str_ptr3+lg) = '0';
    }
  }
    
  return str;
}

