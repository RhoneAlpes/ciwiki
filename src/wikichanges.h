#ifndef _HAVE_WIKICHANGES_HEADER
#define _HAVE_WIKICHANGES_HEADER

void wiki_show_changes_page(HttpResponse *res);

void wiki_show_diff_between_pages(HttpResponse *res, char *page, int mode);

void wiki_show_changes_page_rss(HttpResponse *res);

#endif
