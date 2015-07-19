#ifndef _HAVE_WIKIASHTML_HEADER
#define _HAVE_WIKIASHTML_HEADER

int
wiki_print_data_as_html(
HttpResponse *res, char *raw_page_data, int autorized, char *page);

#endif
