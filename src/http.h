#ifndef _HAVE_HTTP_HEADER
#define _HAVE_HTTP_HEADER

typedef struct HttpResponse     HttpResponse;
typedef struct HttpRequest      HttpRequest;
typedef struct HttpRequestParam HttpRequestParam;

HttpRequest*
http_server(struct in_addr address, int iPort);

HttpRequest*
http_request_new(void);

char*
http_request_param_get(HttpRequest *req, char *key);

void http_request_param_print(HttpRequest *res,HttpRequest *req);

char*
http_request_file_param_get(HttpRequest *req, char *key);

char*
http_request_checkbox_get(HttpRequest *req);

char*
http_request_get_uri(HttpRequest *req);

char*
http_request_get_path_info(HttpRequest *req);

char*
http_request_get_query_string(HttpRequest *req);

char*
http_request_get_ip_src(HttpRequest *req);

HttpResponse*
http_response_new(HttpRequest *req);

void
http_response_printf(HttpResponse *res, const char *format, ...);

void
http_response_printf_alloc_buffer(HttpResponse *res, int bytes);

void
http_response_set_content_type(HttpResponse *res, char *type);

void
http_response_set_status(HttpResponse *res,
			 int           status_code,
			 char         *status_desc) ;

void
http_response_set_data(HttpResponse *res, void *data, int data_len);

void
http_response_send_smallfile
  (HttpResponse *res, char *filename, char *content, unsigned long sizelimit);
  
void
http_response_send_bigfile
  (HttpResponse *res, char *filename, char *content);

void
http_response_append_header(HttpResponse *res, char *header);

void
http_response_send_headers(HttpResponse *res);

void
http_response_send(HttpResponse *res);

void
sigint(int sig);

void
sigterm(int sig);

#endif
