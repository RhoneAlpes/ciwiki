/*
 *      Jean-Pierre Redonnet inphilly@gmail.com
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

/*  Note:  
 * 
 * */


#include "ci.h"
#include "wiki.h"
//#include "wikitext.h"

void
wiki_show_changes_page(HttpResponse *res)
{
  WikiPageList **pages = NULL;
  int            n_pages, i,j,m,lg;
  char          spacing[24];
  char          *difflink;
  int *table, *done; //alloc mem with n_pages
  char *str_ptr;
  
  wiki_show_header(res, "Changes", FALSE, 0);

  pages = wiki_get_pages(&n_pages, NULL); 
  
  table = malloc((1+n_pages)*sizeof(int)); 
  done = malloc((1+n_pages)*sizeof(int)); 
  
  /* regroup file and previous file */
  for (i=0; i<n_pages+1; i++) { done[i]=0; table[i]=0; }
  m=0; 
  for (i=0; i<n_pages; i++)
    if (!done[i] && !strstr(pages[i]->name,".prev.1"))
    {
      for (j=0; j<n_pages; j++)
        if ( !done[j] 
          && (str_ptr=strstr(pages[j]->name,pages[i]->name)) 
          &&  !strcmp(str_ptr+strlen(pages[i]->name),".prev.1") )
        {
          table[m++]=i;
          table[m++]=j;
          done[i]=1;
          done[j]=1;
          break;
        }
    }
  /* complete with new files */
  for (i=0; i<n_pages; i++)
    if (!done[i]) table[m++]=i;
    
  for (j=0; j<m; j++)
    {
      struct tm   *pTm;
      char   datebuf[64];
      i=table[j];
      if ( strstr(pages[i]->name,".prev.1") ) {
        strcpy(spacing,"&nbsp;&nbsp;");
        lg=asprintf(&difflink,
          "<a href='Changes?diff1=%s'>diff</a>\n"
          "<a href='Changes?diff2=%s'>comp</a>\n",
          pages[i]->name,pages[i]->name);
        if (lg==-1) exit(1); //error
      }
      else { 
        *spacing='\0';
        difflink=strdup("\0");
      }
      pTm = localtime(&pages[i]->mtime);
      strftime(datebuf, sizeof(datebuf), "%Y-%m-%d %H:%M", pTm);
      http_response_printf(res, "%s<a href='%s'>%s</a> %s %s<br />\n", 
                           spacing,
                           pages[i]->name, 
                           pages[i]->name, 
                           datebuf,
                           difflink);
    }

  http_response_printf(res, "<p>Wiki changes are also available as a "
                       "<a href='ChangesRss'>RSS</a> feed.\n");

  wiki_show_footer(res);
  http_response_send(res);
  free(table);
  free(done);
  exit(0);
}

/* Use cmd "diff" */
void
wiki_show_diff_between_pages(HttpResponse *res, char *page, int mode)
{
  FILE *fp;
  char line[128];
  char *str_ptr;
  char *cmd;
  int val1,val2,nbln=1,lg;
  char buffer[128];
  char action;
  
  if (!page) exit(0);

  char *current = strdup(page);
  *strstr(current,".prev")='\0';
  fp=fopen(page,"r");  
  lg = asprintf(&cmd,"diff -abB %s %s",page,current);
  if (lg==1) exit(1); //error
  
  wiki_show_header(res, "Changes", FALSE, 0);
  
  FILE* pipe = popen(cmd, "r");
  if (!pipe) 
    exit(0);
  
  http_response_printf(res, "<p>Current page: %s</p>\n",current);
  while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe) != NULL) {
      /* Analyze results returned by diff */
      if ( strchr("<>=",buffer[0]) )
        http_response_printf(res, "<B>%s%s</B><br>\n",
                             buffer[0]=='>'?"New:":"",&buffer[1]);
      else if ( (str_ptr=strpbrk(buffer,"acd")) ) {
        action=*str_ptr;
        *str_ptr='\0';
        val1=atoi(buffer);
        if ( (str_ptr=strchr(buffer,',')) )
          val2=atoi(str_ptr+1);
        else
          val2=val1;
          
        if (mode == 2) {  
          /* Show previous page markup */
          while (nbln < val2) {
            if ( !fgets(line,128,fp) )
              exit(0);
            http_response_printf(res, "%i:%s<br>\n",nbln,line);
            nbln++;
          }
        }
        
        /* Explain change */
        switch ( action ) {
          case 'd':
            http_response_printf(res, "<B>Deleted line %i-%i:<br></B>\n",val1,val2);
            break;
          case 'a':
            http_response_printf(res, "<B>Added line %i-%i:<br></B>\n",val1,val2);
            break;
          case 'c':
            http_response_printf(res, "<B>Changed line %i-%i:<br></B>\n",val1,val2);
            break;
        }
      }
    }
  }
  if (mode == 2) {
    while ( fgets(line,128,fp) ) {
      http_response_printf(res, "%i:%s<br>\n",nbln,line);
      nbln++;
    }
  }
  
  pclose(pipe);
  fclose(fp);
  
  wiki_show_footer(res);
  http_response_send(res);  
  exit(0);
}

void
wiki_show_changes_page_rss(HttpResponse *res)
{
  WikiPageList **pages = NULL;
  int            n_pages, i;
  /*char          *html_clean_wikitext = NULL;
    char          *wikitext; */

  pages = wiki_get_pages(&n_pages, NULL);

  http_response_set_content_type(res, "application/xhtml+xml");

  http_response_printf(res, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                            "<rss version=\"2.0\">\n"
                    "<channel><title>CiWiki Changes feed</title>\n");

  for (i=0; i<n_pages; i++)
  {
    struct tm   *pTm;
    char         datebuf[64];

    pTm = localtime(&pages[i]->mtime);
    strftime(datebuf, sizeof(datebuf), "%Y-%m-%d %H:%M", pTm);

    http_response_printf(res, 
             "<item><title>%s</title>"
             "<link>%s%s</link><description>"
             "Modified %s\n",
                         pages[i]->name,
             getenv("CIWIKI_URL_PREFIX") ? getenv("CIWIKI_URL_PREFIX") : "", 
                         pages[i]->name,
             datebuf);

    /*
    wikitext = file_read(pages[i]->name);
    http_response_printf_alloc_buffer(res, strlen(wikitext)*2);
    html_clean_wikitext = util_htmlize(wikitext, strlen(wikitext));
    wiki_print_data_as_html(res, html_clean_wikitext);
    */
    http_response_printf(res, "</description></item>\n");
  } //end for

  http_response_printf(res, "</channel>\n</rss>");

  http_response_send(res);
  exit(0);
}

