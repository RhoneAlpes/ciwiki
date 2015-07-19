#ifndef _HAVE_WIKI_HEADER
#define _HAVE_WIKI_HEADER

typedef struct WikiPageList WikiPageList;

struct WikiPageList 
{
  char   *name;
  time_t  mtime;
  char  *user_name;
  char  *grp_name;
};

void
wiki_handle_http_request(HttpRequest *req);

void
wiki_show_header(HttpResponse *res, char *page_title, int want_edit, int autorized);

void
wiki_show_footer(HttpResponse *res);

int
wiki_print_data_as_html(
HttpResponse *res, char *raw_page_data, int autorized, char *page);

int
wiki_init(char *ciwiki_home,unsigned restore_Wiki,unsigned create_htmlHome);

WikiPageList**
wiki_get_pages(int  *n_pages, char *expr);

#endif
