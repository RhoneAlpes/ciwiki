/*
 *      wikiashtml.c
 *      
 *      Copyright 2010-2015 Jean-Pierre Redonnet <inphilly@gmail.com>
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
/************************************************************************/
/*  Here we translate the page in html and 'print' it. 
 * The main function for this job is wiki_print_data_as_html()
 * HttpResponse *res contains the vars passed to the server.
 * char *raw_page_data contains the text to translate.
 * int autorized is TRUE if logged.
 * char *page is the page name.
 */
 
#include "ci.h"

/* local variable */
int pic_width=0, pic_height=0, pic_border=0, pic_position=0;
int expand_collapse_num=0;
int target_wwwlink=0;

static char *
get_line_from_string(char **lines, int *line_len)
{
  int   i;
  char *z = *lines;

  if( z[0] == '\0' ) return NULL;

  for (i=0; z[i]; i++)
  {
      if (z[i] == '\n')
      {
        if (i > 0 && z[i-1]=='\r')
        { z[i-1] = '\0'; }
        else
        { z[i] = '\0'; }
      i++;
      break;
     }
  }

  /* advance lines on */
  *lines      = &z[i];
  *line_len -= i;

  return z;
}

static char*
check_for_link(char *line, int *skip_chars)
{
  char *start =  line;
  char *p     =  line;
  char *q;
  char *url   =  NULL;
  char *title =  NULL;
  char *result = NULL;
  int   found = 0;
  int  w,h;
  int  wwwlink;
  char border_pic_str[32]="",width_pic_str[32]="",height_pic_str[32]="";
  char *position_pic_str, *new_tab_str;
  int lgasprintf; //not used, return lg of str could be useful to check err

  if (*p == '[') 
  /* [link] or [link title] or [image link] */
    {
      /* XXX TODO XXX 
       * Allow links like [the Main page] ( covert to the_main_page )
       */
      url = start+1; *p = '\0'; p++;
      while (  *p != ']' && *p != '\0' && !isspace(*p) ) p++;

    if (isspace(*p)) //there is a title or image
    {
      *p = '\0';
      title = ++p; 
      while (  *p != ']' && *p != '\0' ) //search closing braket
        p++;
    }
      *p = '\0';
      p++;
    }
    else if ( !strncasecmp(p, "file=", 5) || !strncasecmp(p, "html=", 5))
    /* file to download or html page to download*/
    {
      int flag_html=0;
      if ( !strncasecmp(p, "html=", 5)) flag_html=1; 
      q=p;
      start=p+5;    
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      if ( flag_html ) {
        //it works but user will not able to edit the wiki page where redirect is used. Not a good idea!
        //redirect. is used in HOMEREDIRECT
        if ( !strncasecmp(url, "redirect.", 9))
          lgasprintf = asprintf(&result,"<script type='text/javascript'>window.location='/html/%s';</script>",url+9);
        else  
          lgasprintf = asprintf(&result,"<a href='/html/%s'>%s</a>",url,url); 
      }
      else
        lgasprintf = asprintf(&result,"<a href='/files/%s'>%s</a>",url,url); 
        
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }      
    else if ( !strncasecmp(p, "youtube=", 8) )
    /* youtube video embedded */
    {
      q=p;
      start=p+8;    
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object width=\"480\" height=\"385\"><param name=\"movie\" value=\"%s\">"
      "</param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" "
      "value=\"always\"></param><embed src=\"%s\" type=\"application/x-shockwave-flash\" "
      "allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"480\" height=\"385\">"
      "</embed></object>",
      url,url);       
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }   
    else if ( !strncasecmp(p, "dailymotion=", 12) )
    /* dailymotion video embedded */
    {
      q=p;
      start=p+12;
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object width=\"480\" height=\"275\"><param name=\"movie\" value=\"%s\">"
      "</param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowScriptAccess\" "
      "value=\"always\"></param><embed src=\"%s\" width=\"480\" height=\"275\" allowfullscreen=\"true\" "
      "allowscriptaccess=\"always\"></embed></object>",
      url,url); 
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }                  
    else if ( !strncasecmp(p, "vimeo=", 6) )
    /* vimeo video embedded */
    {
      q=p;  
      start=p+6;
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object width=\"400\" height=\"225\"><param name=\"allowfullscreen\" value=\"true\" />"
      "<param name=\"allowscriptaccess\" value=\"always\" /><param name=\"movie\" "
      "value=\"%s&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1\" />"
      "<embed src=\"%s&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1\" "
      "type=\"application/x-shockwave-flash\" allowfullscreen=\"true\" allowscriptaccess=\"always\" width=\"400\" height=\"225\">"
      "</embed></object>",
      url,url);       
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }
    else if ( !strncasecmp(p, "veoh=", 5) )
    /* veoh video embedded */
    {
      q=p;  
      start=p+5;
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object width=\"410\" height=\"341\" id=\"veohFlashPlayer\" name=\"veohFlashPlayer\">"
      "<param name=\"movie\" value=\"%s&player=videodetailsembedded&videoAutoPlay=0&id=anonymous\">"
      "</param><param name=\"allowFullScreen\" value=\"true\">"
      "</param><param name=\"allowscriptaccess\" value=\"always\">"
      "</param><embed src=\"%s&player=videodetailsembedded&videoAutoPlay=0&id=anonymous\" "
      "type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" "
      "width=\"410\" height=\"341\" id=\"veohFlashPlayerEmbed\" name=\"veohFlashPlayerEmbed\">"
      "</embed></object>",
      url, url);          
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }
    else if ( !strncasecmp(p, "flash=", 6) )
    /* generic flash video embedded */
    {
      q=p;  
      start=p+6;
      if ( pic_width > 100) w=pic_width;
        else w=320;
      if ( pic_height > 100) h=pic_height;
        else h=240;
        
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object type=\"application/x-shockwave-flash\" data=\"%s\" width=\"%i\" height=\"%i\">"
      "<param name=\"movie\" value=\"%s\"></object>",
      url, w, h, url);  
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }
    else if ( !strncasecmp(p, "swf=", 4) )
    /* swf flash animation embedded */
    {
      q=p;  
      start=p+4;
      if ( pic_width > 100) w=pic_width;
        else w=320;
      if ( pic_height > 100) h=pic_height;
        else h=240;
        
      while ( *p != '\0' && !isspace(*p) && *p != '|' ) p++;
      
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      start=q;
      *start = '\0';
      lgasprintf = asprintf(&result, 
      "<object type=\"application/x-shockwave-flash\" data=\"%s\" width=\"%i\" height=\"%i\">",
        url, w, h); 
      *skip_chars = p - start;
      free(url);
      url = NULL;
      return result;
    }
    else if (!strncasecmp(p, "http://", 7)
       || !strncasecmp(p, "https://", 8)
       || !strncasecmp(p, "mailto://", 9)
       || !strncasecmp(p, "file://", 7))
  /* external link */
    {
      while ( *p != '\0' && !isspace(*p) ) p++;
      found = 1;
    }
  else if (isupper(*p))         /* Camel-case */
    {
      int num_upper_char = 1;
      p++;
      while ( *p != '\0' && isalnum(*p) )
      {
        if (isupper(*p))
        { found = 1; num_upper_char++; }
        p++;
      }

      if (num_upper_char == (p-start)) /* Dont make ALLCAPS links */
        return NULL;
    }

  if (found)  /* cant really set http/camel links in place */
  {
      url = malloc(sizeof(char) * ((p - start) + 2) );
      memset(url, 0, sizeof(char) * ((p - start) + 2));
      strncpy(url, start, p - start);
      *start = '\0';
  }

  if (url != NULL)
  {
      int len = strlen(url);

      *skip_chars = p - start;

      /* url is an image (look at the file extension) ? */
      if ( !strncasecmp(url+len-4, ".gif", 4) || !strncasecmp(url+len-4, ".png", 4) 
      || !strncasecmp(url+len-4, ".jpg", 4) || !strncasecmp(url+len-5, ".jpeg", 5) )
      {   
          if (pic_width) 
            sprintf(width_pic_str," width=\"%i\"",pic_width);
          else
            width_pic_str[0]='\0';
        
          if (pic_height) 
            sprintf(height_pic_str," height=\"%i\"",pic_height);
          else
            height_pic_str[0]='\0';
        
          if (pic_border) 
            sprintf(border_pic_str," border=\"%i\"",pic_border);    
          else
            border_pic_str[0]='\0';
          
          if (pic_position==1)  
            position_pic_str=" style='float:left'";
          else if (pic_position==2)  
            position_pic_str=" style='float:right'";
          else if (pic_position==0)
            position_pic_str="";
            
          if (target_wwwlink)
            new_tab_str=" target='_blank'";
          else
            new_tab_str="";
            
          /* Need to add path ? */
          if ( (strncasecmp(url,"http",4) != 0)  && (strstr(url,"images/")==0) != 0 )  
            {
                /* print the link to the image , add the path to the image folder */
                if (title) // case: [image link]
                  lgasprintf = asprintf(&result, "<a href=\"%s\"%s><img src=\"%s/%s\"%s%s%s%s></a>",
                       title, new_tab_str, PICSFOLDER, url, border_pic_str, width_pic_str, height_pic_str, position_pic_str);
                else // case: http://link_to_image
                  lgasprintf = asprintf(&result, "<img src=\"%s/%s\"%s%s%s%s>", 
                      PICSFOLDER, url, border_pic_str, width_pic_str, height_pic_str, position_pic_str);
            }
            else
            {
                /* print the link to the image */
                if (title) // case: [image link]
                  lgasprintf = asprintf(&result, "<a href=\"%s\"%s><img src=\"%s\"%s%s%s%s></a>",
                      title, new_tab_str, url, border_pic_str, width_pic_str, height_pic_str, position_pic_str);
                else // case: http://link_to_image
                  lgasprintf = asprintf(&result, "<img src=\"%s\"%s%s%s%s>", 
                      url, border_pic_str, width_pic_str, height_pic_str, position_pic_str);
            }
      }
      else // url or title does'nt link to an image
      {
          char *extra_attr = "";

          wwwlink=0;
          if (!strncasecmp(url, "http://", 7)
              || !strncasecmp(url, "https://", 8))
          {
            extra_attr = " title='WWW link' ";
            wwwlink=1;
          }

          if (target_wwwlink && wwwlink)
            new_tab_str=" target='_blank'";
          else
            new_tab_str="";

          if (title)
            lgasprintf = asprintf(&result,"<a %s href='%s'%s>%s</a>", extra_attr, url, new_tab_str, title);
          else
            lgasprintf = asprintf(&result, "<a %s href='%s'%s>%s</a>", extra_attr, url, new_tab_str, url);
      }
      
      if (found)
      {
        free(url);
        url = NULL;
      }
      
      return result;
  }

  return NULL;
}

static int
is_wiki_format_char_or_space(char c)
{
  if (isspace(c)) return 1;
  if (strchr("/*_-", c)) return 1; 
  return 0;
}


/* picture size & position */
void
wiki_parse_between_braces(char *s)
{
    char *str_ptr;
    
    if ( (str_ptr=strstr(s,"width=")) != NULL ) 
    {
      str_ptr+=6;
      pic_width=atoi(str_ptr);
    }
    //else pic_width=0;
    
    if ( (str_ptr=strstr(s,"height=")) != NULL ) 
    {
      str_ptr+=7;
      pic_height=atoi(str_ptr);
    }
    //else pic_height=0;
    
    if ( (str_ptr=strstr(s,"border=")) != NULL ) 
    {
      str_ptr+=7;
      pic_border=atoi(str_ptr);
    }
    //else pic_border=0;
    
    if ( (str_ptr=strstr(s,"left")) != NULL ) 
      pic_position=1;
    else if ( (str_ptr=strstr(s,"right")) != NULL ) 
      pic_position=2;
    else
      pic_position=0;
      
    if ( strstr(s,"default") != NULL )
    {
      pic_width=0;
      pic_height=0;
      pic_border=0;
      pic_position=0;
    }
    
    if ( strstr(s,"wwwlink=new_tab") != NULL )
      target_wwwlink=1;
    else if ( strstr(s,"wwwlink=current_tab") != NULL )
      target_wwwlink=0;

}

void
search_header(char **sectionlist,char *raw_page_data)
/* create a list of header, will be used by {{toc}} */
{
  int header = 0;
  int i = 0;
  char *p = raw_page_data;

  int lg = 1000;
  *sectionlist = malloc(lg); //we can need to realloc more
   
  while ( *p != '\0' )
  {
    /* header is '=' at the beginning of the line */
    if ( *p == '=' && ( p == raw_page_data || *(p-1) == '\n' || *(p-1) == '\r' ) )
    {
      /* get chars until eol */
      while (*p != '\0' && *p != '\n' && *p != '\r' )
      {
        if (i > lg-3)
        {
          lg+= 1000;
          *sectionlist = realloc(*sectionlist, lg);
        }
          
        (*sectionlist)[i++] = *p;
        p++;
      }
      (*sectionlist)[i++] = '\n';
      header++;
    }
    else p++;
  }
  (*sectionlist)[i] = '\0';
  return;
}


int
wiki_print_data_as_html(
HttpResponse *res, char *raw_page_data, int autorized, char *page)
{
  #define ULIST 0
  #define OLIST 1
  #define NUM_LIST_TYPES 2
  #define LABEL_SIZE 80

  char *p = raw_page_data;      /* accumulates non marked up text */
  char *q = NULL, *link = NULL; /* temporary scratch stuff */
  char *line = NULL;
  int   line_len;
  int   i, j, k, skip_chars;
  char  color_str[64];
  char label[LABEL_SIZE];
  char *str_ptr;
  char  *sectionlist;
  int section      = 0;
  /* flags, mainly for open tag states */
  int color_on     = 0;
  int bgcolor_on   = 0;
  int code_on = 0;
  int highlight_on = 0;
  int bold_on      = 0;
  int italic_on    = 0;
  int underline_on = 0;
  int strikethrough_on = 0;
  int open_para    = 0;
  int pre_on       = 0;
  int table_on     = 0;
  int form_on      = 0;
  int form_cnt     = 0;
  int num          = 0;
  int state        = 0;
  
  char color_k,color_prev='\0';
  char bgcolor_k,bgcolor_prev='\0';  
  int private; //flag 1 if access denied
 
  struct { char ident; int  depth; char *tag; } listtypes[] = {
    { '*', 0, "ul" },
    { '#', 0, "ol" }
  };

  char date[80]=""; 
  time_t now;
  struct tm *timeinfo;
  (void) time(&now);
  timeinfo = localtime(&now);

  search_header( &sectionlist, raw_page_data);

  q = p;  /* p accumulates non marked up text, q is just a pointer
       * to the end of the current line - used by below func. 
       */
      
  private=0;
  while ( (line = get_line_from_string(&q, &line_len)) && !private)
  {
    int   header_level = 0; 
    int   blockquote_flag = 0;
    char *line_start   = line;
    int   skip_to_content = 0;
    /*
     *  process any initial wiki chars at line beginning 
     */

    if (pre_on && !isspace(*line) && *line != '\0')
    {
      /* close any preformatting if already on*/
      http_response_printf(res, "</pre>\n") ;
      pre_on = 0;
    }

    /* Handle ordered & unordered list, code is a bit mental.. */
    for (i=0; i<NUM_LIST_TYPES; i++)
    {

      /* extra checks avoid bolding */
      if ( *line == listtypes[i].ident
           && ( *(line+1) == listtypes[i].ident || isspace(*(line+1)) ) ) 
      {                       
        int item_depth = 0;

        if (listtypes[!i].depth)
        {
          for (j=0; j<listtypes[!i].depth; j++)
            http_response_printf(res, "</%s>\n", listtypes[!i].tag);
          listtypes[!i].depth = 0;
        }

        while ( *line == listtypes[i].ident ) { line++; item_depth++; }
    
        if (item_depth < listtypes[i].depth)
        {
          for (j = 0; j < (listtypes[i].depth - item_depth); j++)
            http_response_printf(res, "</%s>\n", listtypes[i].tag);
        }
        else
        {
          for (j = 0; j < (item_depth - listtypes[i].depth); j++)
            http_response_printf(res, "<%s>\n", listtypes[i].tag);
        }
        
        http_response_printf(res, "<li>");
        listtypes[i].depth = item_depth;
        skip_to_content = 1;
      }
      else if (listtypes[i].depth && !listtypes[!i].depth) 
      {
       /* close current list */
        for (j=0; j<listtypes[i].depth; j++)
          http_response_printf(res, "</%s>\n", listtypes[i].tag);
          
        listtypes[i].depth = 0;
      }
    }

    if (skip_to_content)
      goto line_content; /* skip parsing any more initial chars */

    /* Tables */

    if (*line == '|')
    {
      if (table_on==0)
        http_response_printf(res, "<table class='wikitable' cellspacing='0' cellpadding='4'>\n");
      
      line++;

      http_response_printf(res, "<tr><td>");
      table_on = 1;
      goto line_content;
    }
    else
    {
      if(table_on)
      {
        http_response_printf(res, "</table>\n");
        table_on = 0;
      }
    }

    /* pre formated  */

    if ( (isspace(*line) || *line == '\0'))
    {
      int n_spaces = 0;

      while ( isspace(*line) ) { line++; n_spaces++; }

      if (*line == '\0')  /* empty line - para */
      {
        if (pre_on)
        {
          http_response_printf(res, "\n") ;
          continue;
        }
        else if (open_para)
        {
          http_response_printf(res, "\n</p><p>\n") ;
        }
        else
        {
          http_response_printf(res, "\n<p>\n") ;
          open_para = 1;
        }
      }
      else /* starts with space so Pre formatted, see above for close */
      {
        if (!pre_on)
        http_response_printf(res, "<pre>\n") ;
        pre_on = 1;
        line = line - ( n_spaces - 1 ); /* rewind so extra spaces
                                                 they matter to pre */
        http_response_printf(res, "%s\n", line);
        continue;
      }
    }
    else if ( *line == '=' ) //header
    {
      section++;
      while (*line == '=')
        { header_level++; line++; }
      http_response_printf(res, "<h%d id='section%i'>", header_level, section);
      p = line;
    }
    else if ( *line == '-' && *(line+1) == '-' ) //rule
    {
      http_response_printf(res, "<hr/>\n");
      while ( *line == '-' ) line++;
    }
    else if ( *line == '\'' ) //quote
    {
      blockquote_flag=1;
      line++;
      http_response_printf(res, "<blockquote>\n");
    }

    line_content:

    /* 
     * now process rest of the line 
     */

    p = line;

    while ( *line != '\0' )
    {
      /* ignore link */
      if ( *line == '!' && !isspace(*(line+1))) 
      {                   /* escape next word - skip it */
        *line = '\0';
        http_response_printf(res, "%s", p);
        p = ++line;

        while (*line != '\0' && !isspace(*line)) line++;
        if (*line == '\0')
          continue;
      }
      /* search for link inside the line */
      else if ((link = check_for_link(line, &skip_chars)) != NULL)
      {
        http_response_printf(res, "%s", p);
        http_response_printf(res, "%s", link);

        line += skip_chars;
        p = line;

        continue;
      }
      /* TODO: Below is getting bloated and messy, need rewriting more
       *       compactly ( and efficently ).
       */
      else if  (*line == '{' && *(line+1) == '{')
      /* Proceed double braces */
      {
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)))
        { line=line+2; continue; }
        /* search closing braces */ 
        k=2;
        while (*(line+k) != '}')
        {
          if ( *(line+k) == '\0' )
            { line=line+2; continue; }
          k++;
        }
        k++;
        if (*(line+k) == '}')
          *(line+k)='\0'; //terminate the line
        else
          { line=line+2; continue; }

        /* Parse tags between double braces */
        // need to be rewritten
        
        /* search image/video size */
        wiki_parse_between_braces(line+2);
        /* control access */
        if ( (strstr(line+2,"private")) ) 
        {
          if ( !autorized )
          {
            http_response_printf(res, "<p>--- Sorry, the access below is denied ----</p>\n");
            *p = '\0';
            private=1; //will stop getting line
            break; //terminate the while loop
          }
        }
        /* Expand */
        if ( (str_ptr=strstr(line+2,"expand")) ) 
        {   
          if ( *(str_ptr-1) == '-' ) //terminate expand
          {
            http_response_printf(res,"</div></div>\n");
          }
          else          
          {
            /* search label */
            if ( *(str_ptr+6) == '=' )
            {
              i=0;
              str_ptr+=7;
              while ( (*str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
              {
                label[i]=*str_ptr;
                str_ptr++;
                i++;
              }
              label[i]='\0';
            }
            else
              strcpy(label,"Click here!");
            
            expand_collapse_num++; 
            //Note: use javascript  function expandcollapse()
            http_response_printf(res, 
              "<div id=\"wrapper\">"
              "<p>"
              "<a onclick=\"expandcollapse('myvar%i');\" "
              "title=\"Expand or collapse\">%s</a>"
              "</p>"
              "<div id=\"myvar%i\" style=\"display:none\">",
              expand_collapse_num,label,expand_collapse_num);
          }
        }
        /* Collapse */
        else if ( (str_ptr=strstr(line+2,"collapse")) ) 
        {   
          if ( *(str_ptr-1) == '-' ) //terminate expand
          {
            http_response_printf(res,"</div></div>\n");
          }
          else          
          {
            /* search label */
            if ( *(str_ptr+8) == '=' )
            {
              i=0;
              str_ptr+=9;
              while ( (*str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
              {
                label[i]=*str_ptr;
                str_ptr++;
                i++;
              }
              label[i]='\0';
            }
            else
              strcpy(label,"Click here!");
            
            expand_collapse_num++; 
            //Note: use javascript  function expandcollapse()
            http_response_printf(res, 
              "<div id=\"wrapper\">"
              "<p>"
              "<a onclick=\"expandcollapse('myvar%i');\" "
              "title=\"Expand or collapse\">%s</a>"
              "</p>"
              "<div id=\"myvar%i\">",
              expand_collapse_num,label,expand_collapse_num);
          }
        }
        /* Upload a file */
        if ( (strstr(line+2,"upload")) ) 
        {   
          http_response_printf(res, 
            "<p><FORM ACTION='Upload' METHOD='post' ENCTYPE='multipart/form-data'>"
            "<input type='file' name='filename' size='40'><br>"
            "<P><INPUT TYPE=submit VALUE='Send'></P>"
            "</form></p>\n");
        }
        /* table of contents */
        if ( strstr(line+2,"toc") )
        {
          int sectioncnt=0;
          while ( (str_ptr=strchr(sectionlist,'\n')) )
          {
            *str_ptr='\0';
            
            sectioncnt++;
            /* header level */
            int item_depth = 0;
            while ( *sectionlist == '=' ) 
            { 
              sectionlist++; 
              item_depth++; 
            }
            /* indent */
            for (j = 0; j < item_depth; j++)
                http_response_printf(res, "<ul>");
            /* skip first ! */
            if ( *sectionlist == '!' ) 
              sectionlist++;
            http_response_printf(res, 
              "<li><a href='#section%i'>%s</a></li>", 
              sectioncnt, sectionlist);
            /* reset indentation */
            for (j=0; j<item_depth; j++)
              http_response_printf(res, "</ul>\n");
            item_depth = 0;
            /* point to the next header */  
            sectionlist = str_ptr+1;
          }
        }
        /* entry  */
        if ( (str_ptr=strstr(line+2,"entry")) ) 
        { 
          /* close form already opened */
          if (form_on)
            http_response_printf(res, "</form>\n");
          
          form_on=1; //so we know we will have to close <form>
          form_cnt++; //for datafield
          http_response_printf(res, 
            "<form method=POST action='%s?entry' name='entryform'>\n",page);
          
          int size=30; //default entry size
          
          if ( *(str_ptr+5) )
          {
            if ( (strstr(str_ptr+5,"tiny")) )
              size=10;
            else if ( (strstr(str_ptr+5,"small")) )
              size=20;
            else if ( (strstr(str_ptr+5,"medium")) )
              size=40;
            else if ( (strstr(str_ptr+5,"large")) )
              size=60;
            else if ( (strstr(str_ptr+5,"huge")) )
              size=80;
            
            date[0]='\0';
            if ( (str_ptr=strstr(str_ptr+5,"date")) )
              strftime(date, 80, "%a %b %d %H:%M ", timeinfo);
              
          }
          /* hidden datafield gives the {{data#}} to use */
          http_response_printf(res, 
            "<p><input type='text' name='data' value='%s' size='%i' title='Entrer your text'>"
            "</p>"
            "<input type='hidden' name='datafield' value='%i' />"
            "<p><input type=submit name='add' value='Add' title='[alt-a]' accesskey='a'>"
            "</p>\n",date,size,form_cnt);
        }
        /* Simple checkbox */
        if ( (str_ptr=strstr(line+2,"checkbox")) ) 
        {
          num=state=0;   
          if (*(str_ptr+8) == '=')
            num=atol(str_ptr+9);
          if ( (str_ptr=strchr(str_ptr+9,';')) )
            state=atoi(str_ptr+1);
          /* little trick: checkbox unchecked is not posted
           * the input hidden returns the value 0
           * we have the values: 0 , 1 when checked
           * and 0 when unchecked. browser must operate sequentially!
           */
          http_response_printf(res,
            "<input type='hidden' name='checkbox%i' value='0' />"
            "<input type='checkbox' name='checkbox%i' value='1' %s /> ",
            num,num,state ? "checked='checked'":"");
        }
        /* Save checkboxes state */
        if ( (strstr(line+2,"save")) ) 
        {
          http_response_printf(res,
            "<input type=submit name='save' value='Save' title='[alt-s]' accesskey='s'>\n");
        }
        /* Delete field */
        if ( (strstr(line+2,"delete")) ) 
        {
          http_response_printf(res,
            "<input type=submit name='delete' value='Delete' title='[alt-d]' accesskey='d'>\n");
        }
        
        /* Display a text when Scheduled date == System date
         * the format is {{Schedule=date;text if true;text if false}}
         * date format is <Day of the week><space><Month><space><Day of the month>
         * example: Monday January 16
         * */
        if ( (str_ptr=strstr(line+2,"schedule")) ) 
        { 
          /* Extract the date */
          if ( *(str_ptr+8) == '=' )
          {
            i=0;
            str_ptr+=9;
            while ( (*str_ptr != ';' && *str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
            {
              label[i]=*str_ptr; 
              str_ptr++;
              i++;
            }
            label[i]='\0';
          }	  

          if (*str_ptr == ';') //date must be terminated by a semicolon
          {
            str_ptr++;
            /* Format the current date "Day of the week" "Month" "day of the month"*/
            strftime(date, 80, "%A %B %d", timeinfo);
		    if (strstr(date,label)) {
			  i=0;
			  while ( (*str_ptr != ';' && *str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
              {
                label[i]=*str_ptr; 
                str_ptr++;
                i++;
              }
              label[i]='\0';
              /* The content of the label is copied
                and will be proceeded as text*/
              line+=k-strlen(label);
              *line=' ';
              strcpy(line+1,label); //label length < k so strcpy is safe
              goto line_content; //proceed again the line
		    }
		    else
            {
              while ( (*str_ptr != ';' && *str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
                str_ptr++;
              if (*str_ptr == ';') 
              {
                str_ptr++;
                i=0;
			    while ( (*str_ptr != ';' && *str_ptr != '}' && *str_ptr != '\0') && i < LABEL_SIZE)
                {
                  label[i]=*str_ptr; 
                  str_ptr++;
                  i++;
                }
                label[i]='\0';
                /* The content of the label is copied
                and will be proceeded as text*/
                line+=k-strlen(label);
                *line=' ';
                strcpy(line+1,label);
                goto line_content; //proceed again the line
	          }
            }
		 }
         else
         {
             /* no semicolon, so we suppose the user wants to test a date 
              * format: d/m/n 
              * d is an interger = 0...6 (Sunday...Saturday)
              * m is an interger = 1...12 (January...December)
              * n is an integer = 1...31 */
             int val;
             val=atoi(label);
             if(val<0) val=0;
             if(val>6) val=6;
             timeinfo->tm_wday=val;
             i=0;
             while(i<LABEL_SIZE && isdigit(label[i])) i++;
             i++;
             val=atoi(label+i);
             if(val<1) val=1;
             if(val>12) val=12;
             timeinfo->tm_mon=val-1;
             while(i<LABEL_SIZE && isdigit(label[i])) i++;
             i++;
             val=atoi(label+i);
             if(val<1) val=1;
             if(val>31) val=31;
             timeinfo->tm_mday=val;
             strftime(date, 80, "%A %B %d", timeinfo);
             http_response_printf(res, "<p>Temporary date is %s</p>\n",date);
         }
		
       }
        /* Add new cmd below if needed */
        /* ... */
        
        line+= k; //point to '\0' (the last closing brace)
        p= line+1; //point just after '\0'
      } //end of the double braces
      
      /* single braces */
      else if (*line == '{' && strchr("RGBYPCD",*(line+1)) && *(line+2) == '}')
      {
        /* Text color */
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !color_on)
        { line++; continue; }

        if ((isspace(*(line+3)) && !color_on))
        { line++; continue; }

        /* color */
        *line = '\0';
        color_k = *(line+1);
        switch( color_k ) 
        {
          case 'R':
            strcpy(color_str,"<FONT COLOR=\"#FF0000\">");
            break;
          case 'G':
            strcpy(color_str,"<FONT COLOR=\"#00FF00\">");
            break;
          case 'B':
            strcpy(color_str,"<FONT COLOR=\"#0000FF\">");
            break;
          case 'Y':
            strcpy(color_str,"<FONT COLOR=\"#FFFF00\">");
            break;
          case 'P':
            strcpy(color_str,"<FONT COLOR=\"#FF00FF\">");
            break;
          case 'C':
            strcpy(color_str,"<FONT COLOR=\"#00FFFF\">");
            break;
          case 'D':
            strcpy(color_str,"<FONT COLOR=\"#000000\">");
            break;  
        }
        
        if (color_prev && color_k != color_prev) { 
          color_on = 0; /* reset flag */  
          http_response_printf(res, "%s","</FONT>\n");
        }
          color_prev = color_k;

        http_response_printf(res, "%s%s\n", p, color_on ? "</FONT>" : color_str);
        color_on ^= 1; /* switch flag */
        p = line+3;
      }
       else if (*line == '(' && strchr("RGBYPCD",*(line+1)) && *(line+2) ==')' )
      {
        /* bgcolor */
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !bgcolor_on)
        { line++; continue; }

        if ((isspace(*(line+3)) && !bgcolor_on))
        { line++; continue; }

        /* color */
        *line = '\0';
        bgcolor_k = *(line+1);
        switch( bgcolor_k ) 
        {
          case 'R':
            strcpy(color_str,"<SPAN STYLE=\"background: #FF0000\">");
            break;
          case 'G':
            strcpy(color_str,"<SPAN STYLE=\"background: #00FF00\">");
            break;
          case 'B':
            strcpy(color_str,"<SPAN STYLE=\"background: #0000FF\">");
            break;
          case 'Y':
            strcpy(color_str,"<SPAN STYLE=\"background: #FFFF00\">");
            break;
          case 'P':
            strcpy(color_str,"<SPAN STYLE=\"background: #FF00FF\">");
            break;
          case 'C':
            strcpy(color_str,"<SPAN STYLE=\"background: #00FFFF\">");
            break;
        }
        
        if (bgcolor_prev && bgcolor_k != bgcolor_prev) {
          bgcolor_on = 0; /* reset flag */    
          http_response_printf(res, "%s","</SPAN>\n");
        }
        bgcolor_prev = bgcolor_k;

        http_response_printf(res, "%s%s\n", p, bgcolor_on ? "</SPAN>" : color_str);
        bgcolor_on ^= 1; /* switch flag */
        p = line+3;

      }
      else if (*line == '`')
      {
        /* code */
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !code_on)
        { line++; continue; }

        if ((isspace(*(line+1)) && !code_on))
        { line++; continue; }

        *line = '\0';
        http_response_printf(res, "%s%s\n", p, code_on ? "</CODE>" : "<CODE>");
        code_on ^= 1; /* switch flag */
        p = line+1;
      }
      else if (*line == '+')
      {
        /* highlight */
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !highlight_on)
        { line++; continue; }

        if ((isspace(*(line+1)) && !highlight_on))
        { line++; continue; }

        *line = '\0';
        http_response_printf(res, "%s%s\n", p, highlight_on ? "</SPAN>" : "<SPAN STYLE=\"background: #FFFF00\">");
        highlight_on ^= 1; /* switch flag */
        p = line+1;
      }
      else if (*line == '*')
      {
        /* Try and be smart about what gets bolded */
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !bold_on)
        { line++; continue; }

        if ((isspace(*(line+1)) && !bold_on))
        { line++; continue; }

        /* bold */
        *line = '\0';
        http_response_printf(res, "%s%s\n", p, bold_on ? "</b>" : "<b>");
        bold_on ^= 1; /* reset flag */
        p = line+1;

      }
      else if (*line == '_' )
      {
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !underline_on)
        { line++; continue; }

        if (isspace(*(line+1)) && !underline_on)
        { line++; continue; }
        /* underline */
        *line = '\0';
        http_response_printf(res, "%s%s\n", p, underline_on ? "</u>" : "<u>"); 
        underline_on ^= 1; /* reset flag */
        p = line+1;
      }
      else if (*line == '-')
      {
        if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !strikethrough_on)
        { line++; continue; }

        if (isspace(*(line+1)) && !strikethrough_on)
        { line++; continue; }
           
          /* strikethrough */
          *line = '\0';
          http_response_printf(res, "%s%s\n", p, strikethrough_on ? "</del>" : "<del>"); 
          strikethrough_on ^= 1; /* reset flag */
          p = line+1; 
        }
      else if (*line == '/' )
        {
          if (line_start != line 
          && !is_wiki_format_char_or_space(*(line-1)) 
          && !italic_on)
        { line++; continue; }

          if (isspace(*(line+1)) && !italic_on)
        { line++; continue; }

          /* crude path detection */
          if (line_start != line && isspace(*(line-1)) && !italic_on)
        { 
          char *tmp   = line+1;
          int slashes = 0;

          /* Hack to escape out file paths */
          while (*tmp != '\0' && !isspace(*tmp))
            { 
              if (*tmp == '/') slashes++;
              tmp++;
            }

          if (slashes > 1 || (slashes == 1 && *(tmp-1) != '/')) 
            { line = tmp; continue; }
        }

          if (*(line+1) == '/')
        line++;     /* escape out common '//' - eg urls */
          else
        {
          /* italic */
          *line = '\0';
          http_response_printf(res, "%s%s\n", p, italic_on ? "</i>" : "<i>"); 
          italic_on ^= 1; /* reset flag */
          p = line+1; 
        }
      }
      else if (*line == '|' && table_on) /* table column */
      {
        *line = '\0';
        http_response_printf(res, "%s", p);
        http_response_printf(res, "</td><td>\n");
        p = line+1;
      }

      line++;

    } /* while loop, next word */

    if (*p != '\0')           /* accumulated text left over */
      http_response_printf(res, "%s", p);

      /* close any html tags that could be still open */

    if (listtypes[ULIST].depth)
      http_response_printf(res, "</li>");

    if (listtypes[OLIST].depth)
      http_response_printf(res, "</li>");

    if (table_on)
      http_response_printf(res, "</td></tr>\n");

    if (header_level)
      http_response_printf(res, "</h%d>\n", header_level);  
    else if (blockquote_flag)
      http_response_printf(res, "</blockquote>\n");  
    else
      http_response_printf(res, "\n");
    } /* next line */

  /*** clean up anything thats still open ***/

  if (pre_on)
    http_response_printf(res, "</pre>\n");
  
  /* close any open lists */
  for (i=0; i<listtypes[ULIST].depth; i++)
    http_response_printf(res, "</ul>\n");

  for (i=0; i<listtypes[OLIST].depth; i++)
    http_response_printf(res, "</ol>\n");
  
  /* close any open paras */
  if (open_para)
    http_response_printf(res, "</p>\n");

  /* close table */
  if (table_on)
    http_response_printf(res, "</table>\n");
  /* close form */
  if (form_on)
    http_response_printf(res, "</form>\n");
    
  return private;
}
