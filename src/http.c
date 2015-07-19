/* 
 *  CiWiki is a fork of DidiWiki 
 *  it will stay a small lightweight wiki engine with new features added.
 *  Current developer: Jean-Pierre Redonnet (inphilly@gmail.com)
 *
 *  Parts of this http and util code based on cvstrac sources. 
 *  See http://www.cvstrac.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "ci.h"
#include <unistd.h>
#include <signal.h>

extern int max_upload_size;
int upload_status;

struct HttpRequestParam  {
  char *key;
  char *val;
};

struct HttpRequest  
{
  HttpRequestParam *params;
  int               n_params;
  int               n_params_alloc;
  char             *uri;
  char             *path_info;
  char             *query_string;
  char             *ip_src;
};

struct HttpResponse 
{
  char *content_type;
  char *status_str;
  int   status;

  char *extra_headers;

  char *data;
  int   data_len;
  int   data_len_alloced;
};

/*
** Dump
*/
static int
save_dump_file(char *data, int len)
{
    int k=0;
    int ok_write=0;
    char* fullfilename=NULL;
    
    //look for name="filename"
    while( data && (*(data+k) != '=') && (k < 200) ) k++;
    if ( (*(data+k) == '=') && (!strncmp(data+k-4,"name",4)) )
    {   
        if ( !strncmp(data+k+2,"filename",8) )
        {
            data=data+k+11; //point after name="filename"
            //look for filename= and get the value
            char* ptr1=strstr(data,"filename=");
            if (ptr1) {
                data=ptr1+9; ////point after "filename="
                //skip space and double quotes
                k=0;
                while( data && ((*(data+k) == '"') || (*(data+k) == ' ')) && (k < 10) ) k++;
                data=data+k;              
                //look for the end of line
                k=0;
                while( data && (*(data+k) != '\r') && (*(data+k) != '\n') && (k < 255) ) k++;
                //copy the file name
                char* filename = strndup(data,k);
                *(filename+k)=0;
                *strchr(filename,'"')=0;
                //we are at the end of the line
                if ((*(data+k) == '\r') || (*(data+k) == '\n'))
                {
                    data=data+k+1;
                    //look for the content-type
                    char* ptr2=strcasestr(data,"content-type");
                    if (ptr2) {
                        data=ptr2+12;
                        k=0;
                        //skip space and colon
                        while( data && ((*(data+k) == ' ') || (*(data+k) == ':')) && (k < 10) ) k++;
                        data=data+k;
                        //is it an image uploaded?
                        if ( !strncasecmp(data,"image",5) )
                        {
                            if (asprintf(&fullfilename,"%s/%s",PICSFOLDER,filename) <= 0)
                                return -1;
                            data=data+5;
                            ok_write=1;
                        } 
                        //is it an application?  
                        else if ( !strncasecmp(data,"application",11) )
                        {
                            if (asprintf(&fullfilename,"%s/%s",FILESFOLDER,filename) <= 0)
                                return -1;
                            data=data+5;
                            ok_write=1;
                        }
                        //is it html file?  
                        else if ( !strncasecmp(data,"text/html",9) )
                        {
                            if (asprintf(&fullfilename,"%s/%s",HTMLFOLDER,filename) <= 0)
                                return -1;
                            data=data+5;
                            ok_write=1;
                        }
                        //is it text plain?  
                        else if ( !strncasecmp(data,"text/plain",10) )
                        {
                            if (asprintf(&fullfilename,"%s/%s",FILESFOLDER,filename) <= 0)
                                return -1;
                            data=data+5;
                            ok_write=1;
                        }
                        else
                        //return error
                            return -1;
                            
                        if (ok_write)
                        {  
                            //go to the end of the line
                            k=0;
                            while( data && (*(data+k) != '\r') && (*(data+k) != '\n') && (k < 40) ) k++;
                            data=data+k+1;
                            //skip the CR and LF
                            k=0;
                            while( data && ((*(data+k) == '\r') || (*(data+k) == '\n')) && (k < 40) ) k++;
                            data=data+k; //point the data to save on the file
                      
                            FILE*       fp;
                            int         bytes_written = 0;
                  
                            if (!(fp = fopen(fullfilename, "wb"))) 
                                return -1;
                 
                            while ( len > 0 )
                            {
                                bytes_written = fwrite(data, sizeof(char), len, fp);
                                len = len - bytes_written;
                                data = data + bytes_written;
                            }
                            fclose(fp);
                            return bytes_written;
                        } return -1; //because content-type not allowed
                    }
                }
            }
        }
    }
    return 0; //not a file to upload
}

/*
** Send a reply indicating that the HTTP request was malformed
*/
static void 
malformed_request(int code, char *info)
{
  HttpResponse *res = NULL;

  res = http_response_new(NULL);

  http_response_set_status(res, 501, "Not Implemented");
  http_response_printf(res, 
    "<html><body>Unrecognized HTTP Request Code=%i</body></html>\n",
    code);
  http_response_send(res);
  
  /* log Error */
  syslog(LOG_LOCAL0|LOG_INFO, "Malformed request 501\nCode=%i\n%s\n", code,info);  
  exit(0);
}

/*
 * Add another query parameter or cookie to the parameter set.
 */
static void 
http_request_add_param(HttpRequest *req, char *key, char *value)
{
  if( req->n_params_alloc <= req->n_params )
    {
      req->n_params_alloc = req->n_params_alloc*2 + 10;
      req->params = realloc( req->params, 
			     req->n_params_alloc * sizeof(HttpRequestParam));
      if( !req->params ) exit(1); /* XXX handle better  */
    }

  req->params[req->n_params].key = key;
  req->params[req->n_params].val = value;

  req->n_params++;
}


/*
 * Add a list of query parameters or cookies to the parameter set.
 *
 * Each parameter is of the form NAME=VALUE.  Both the NAME and the
 * VALUE may be url-encoded ("+" for space, "%HH" for other special
 * characters).  But this routine assumes that NAME contains no
 * special character and therefore does not decode it.
 *
 * Parameters are separated by the "terminator" character.  Whitespace
 * before the NAME is ignored.
 *
 * The input string "z" is modified but no copies is made.  "z"
 * should not be deallocated or changed again after this routine
 * returns or it will corrupt the parameter table.
 */
static void 
http_request_parse_params (HttpRequest *req, 
			   char        *data, 
			   int          terminator)
{
  while( *data )
    {
      char *key = NULL, *val = NULL;

      while( isspace(*data) ){ data++; }
      
      key = data;

      while( *data && *data != '=' && *data != terminator )
      data++; 
      
      if( *data == '=' )
    {
      *data = '\0';
      data++;
      val= data;

      while( *data && *data != terminator ) 
        data++; 

      if( *data ) { *data = '\0'; data++; }
      
      util_dehttpize(val);

      http_request_add_param(req, key, val);

    }
      else
    {
      if( *data ){ *data++ = 0; }

      http_request_add_param(req, key, NULL);
    }
  }
}


char*
http_request_param_get(HttpRequest *req, char *key)
{
  int i;

  for (i=0; i<req->n_params; i++)
    if (!strcmp(req->params[i].key, key))
      return req->params[i].val;

  return NULL;
} 

char*
http_request_file_param_get(HttpRequest *req, char *key)
{
    int ix;
    HttpRequest *reqx;
    reqx=req;

    for (ix=0; ix<reqx->n_params; ix++) {
        return(reqx->params[ix].val);
    }
    return NULL;
}


char*
http_request_checkbox_get(HttpRequest *req)
{
  int i,lenght=0;
  static char string[1000]; //!limitation

  string[0]='\0';
  for (i=0; i<req->n_params; i++)
    if (!strncmp(req->params[i].key, "checkbox", 8))
    {
      lenght+=strlen(req->params[i].key+8);
      lenght+=strlen(req->params[i].val);
      if (lenght > 999) break; //!silent bug
      strcat(string,req->params[i].key+8);
      strcat(string,"=");
      strcat(string,req->params[i].val);
      strcat(string,";");
    }
    
  return string;
} 


char*
http_request_get_uri(HttpRequest *req)
{
  return req->uri;
}

char*
http_request_get_path_info(HttpRequest *req)
{
  return req->path_info;
}

char*
http_request_get_query_string(HttpRequest *req)
{
  return req->query_string;
}

char*
http_request_get_ip_src(HttpRequest *req)
{
  return req->ip_src;
}

/*
 * This routine handles a single HTTP request which is coming in on
 * standard input ( and which replies on standard output. ). 
 * 
 */
HttpRequest*
http_request_new(void)
{
  char              *token, *content_type, *post_data, *z;
  int                i, len;
  struct sockaddr_in remoteName;
  socklen_t          size = sizeof(struct sockaddr_in);
  char               request_line[2048];     /* A single line of input. */
  HttpRequest       *req = NULL;

  req = malloc(sizeof(HttpRequest));
  memset(req, 0, sizeof(HttpRequest)); 

  if( fgets(request_line, sizeof(request_line), stdin) == NULL )
    malformed_request(1, "Empty request");

  if ((token = util_extract_token(request_line, &z)) == NULL)
    malformed_request(2, request_line); //not token

  if(!(!strcmp(token,"GET") || !strcmp(token,"POST") || !strcmp(token,"HEAD")))
    malformed_request(3, token); //token not accepted

  putenv("GATEWAY_INTERFACE=CGI/1.0");
  putenv(util_mprintf("REQUEST_METHOD=%s",token));

  if ((token = util_extract_token(z, &z)) == NULL)
    malformed_request(4, z); //no uri

  putenv(util_mprintf("REQUEST_URI=%s", token));

  if (token)
    req->uri = strdup(token);

  /* eg. GET /blah.htm?crap HTTP/1.0 */

  for(i=0; token[i] && token[i]!='?'; i++);

  if( token[i] ) token[i++] = 0;

  putenv(util_mprintf("PATH_INFO=%s", token));

  if (token)
    req->path_info = strdup(token);

  putenv(util_mprintf("QUERY_STRING=%s", &token[i]));

  if (&token[i])
    req->query_string = strdup(&token[i]);

  if( getpeername(fileno(stdin), (struct sockaddr*)&remoteName, &size) >=0 ) {
    putenv(util_mprintf("REMOTE_ADDR=%s", inet_ntoa(remoteName.sin_addr)));
	req->ip_src = inet_ntoa(remoteName.sin_addr);
  }

  /* Get all the optional fields that follow the first line.
  */
  while( fgets(request_line, sizeof(request_line), stdin) )
  {
    char *key = NULL, *val = NULL;

    key = util_extract_token(request_line, &val);

    if (key == NULL || *key == '\0')
      break;

    /* strip any spacing around key */
    while( isspace(*val) ){ val++; }
    i = strlen(val);
    while( i>0 && isspace(val[i-1]) ){ i--; } val[i] = '\0';

    /* and lower case it */
    for(i=0; key[i]; i++)
      key[i] = tolower(key[i]); 

    if(!strcmp(key,"user-agent:")) 
    {
      putenv(util_mprintf("HTTP_USER_AGENT=%s", val));
    } 
    else if (!strcmp(key,"content-length:"))
    {
      putenv(util_mprintf("CONTENT_LENGTH=%s", val));
    }
    else if (!strcmp(key,"referer:"))
    {
      putenv(util_mprintf("HTTP_REFERER=%s", val));
    } 
    else if (!strcmp(key,"host:"))
    {
      putenv(util_mprintf("HTTP_HOST=%s", val));
    }
    else if (!strcmp(key,"content-type:"))
    {
      putenv(util_mprintf("CONTENT_TYPE=%s", val));
    }
    else if (!strcmp(key,"cookie:"))
    {
      putenv(util_mprintf("HTTP_COOKIE=%s", val));
    }
    else if (!strcmp(key,"action:"))
    {
      putenv(util_mprintf("ACTION=%s", val));
    }
    
  }

  /* Parse and store QUERY_STRING/POST/Cookie data in req object */

  if (getenv("QUERY_STRING"))
    http_request_parse_params (req, strdup((getenv("QUERY_STRING"))), '&'); 

  /* POST type data */

  content_type = getenv("CONTENT_TYPE");
  len = getenv("CONTENT_LENGTH") ? atoi(getenv("CONTENT_LENGTH")) : 0;
  
  /* Don't accept post size above the limit */
  if (len > max_upload_size) 
  {
    syslog(LOG_LOCAL0|LOG_INFO, "Upload rejected - file to big %i", len);  
    upload_status=-413;
    while( --len > 0 ) fgetc(stdin); //flush stdin
    return req;
  }

  if (len && content_type)
    {
      if (strncmp(content_type,"application/x-www-form-urlencoded",19) == 0)
      {
        post_data = malloc( len+1 );
        len = fread(post_data, 1, len, stdin);
        post_data[len] = '\0';
        http_request_parse_params (req, post_data, '&');
      }
      else if (strncmp(content_type,"multipart/form-data",19) == 0)
      {
        post_data = malloc( len+1 );
        len = fread(post_data, 1, len, stdin);
        post_data[len] = '\0';
        
        /* check if it's an upload, if not extract parameters */
        upload_status = save_dump_file(post_data,len);
        if ( upload_status == 0 )
            /* it's a form - extract parameters */
            http_request_parse_params (req, post_data, '&');
        else
            /* That was an upload */
            syslog(LOG_LOCAL0|LOG_INFO, "Upload status=%i(KB or err code)", 
                                         upload_status);    
      }
      else malformed_request(5, content_type);
  }

  if (getenv("HTTP_COOKIE"))
    http_request_parse_params (req, strdup((getenv("HTTP_COOKIE"))), '&'); 

  return req;
}

HttpResponse*
http_response_new(HttpRequest *req) 
{
  HttpResponse *res = NULL;

  res = malloc(sizeof(HttpResponse));
  memset(res, 0, sizeof(HttpResponse)); 

  res->status       = 200;
  res->status_str   = "OK";
  res->content_type = NULL;

  return res;
}

void
http_response_set_content_type(HttpResponse *res, char *type)
{
  res->content_type = strdup(type);
}

/* like Location: hello.html\r\n */
void
http_response_append_header(HttpResponse *res, char *header)
{
  if (res->extra_headers)
    {
      res->extra_headers 
	= realloc( res->extra_headers, 
		   strlen(res->extra_headers) + strlen(header) + 1);
      strcat(res->extra_headers, header);
    }
  else
    {
      res->extra_headers = strdup(header);
    }
}


void
http_response_printf_alloc_buffer(HttpResponse *res, int bytes)
{
  res->data_len_alloced += bytes;
  res->data = realloc( res->data, res->data_len_alloced );
}


void
http_response_printf(HttpResponse *res, const char *format, ...)
{
  va_list ap;
  char    *tmp = NULL;

  va_start(ap,format);
  
  if ( vasprintf(&tmp, format, ap) < 0 ) 
  {
    fprintf(stderr, "Error http_response_printf.\n");
    return;
  }
  
  va_end(ap);

  if ((res->data_len + strlen(tmp) + 1) < res->data_len_alloced)
    {
      if (res->data_len)
	memcpy(res->data + res->data_len - 1, tmp, strlen(tmp)+1);
      else
	memcpy(res->data, tmp, strlen(tmp)+1);
    }
  else if (!res->data_len) 		/* no data printed yet */
    {
      res->data = malloc(strlen(tmp)+1);
      memcpy(res->data, tmp, strlen(tmp)+1);
      res->data_len_alloced = strlen(tmp)+1;
    }
  else
    {
      res->data = realloc( res->data, res->data_len + strlen(tmp) );
      memcpy(res->data + res->data_len - 1, tmp, strlen(tmp)+1);
      res->data_len_alloced = res->data_len + strlen(tmp);
    }

  res->data_len = strlen(res->data)+1;      
  free(tmp);
}


void
http_response_set_status(HttpResponse *res,
			 int           status_code,
			 char         *status_desc) 
{
  res->status       = status_code;
  res->status_str   = strdup(status_desc);;
}


void
http_response_set_data(HttpResponse *res, void *data, int data_len)
{
  res->data     = (char *)data;
  res->data_len = data_len; 
}

void
http_response_send_smallfile
  (HttpResponse *res, char *filename, char *content, unsigned long sizelimit)
/* ! file loaded in mem */
{
  unsigned char *data;
  FILE *fp=fopen(filename,"rb");
  /* get file size and alloc memory */
  fseek (fp , 0 , SEEK_END);
  unsigned long datasize = ftell(fp);
  if (datasize > sizelimit)
    exit(-1);
  rewind (fp);
  if (!(data = (unsigned char*)malloc(datasize)))
    exit(-1);
  /* load the file */
  if (!fread(data, datasize, 1, fp))
    exit(-1);
  http_response_set_content_type(res, content);
  http_response_set_data(res, data, datasize);
  http_response_send(res);
  fclose(fp);
}

void
http_response_send_headers(HttpResponse *res)
{
  printf("HTTP/1.0 %d %s\r\n", res->status, res->status_str);

  if (res->extra_headers) printf("%s", res->extra_headers);

  /* XXX ciwiki likes cookies etc */

  printf("Content-Type: %s; charset=UTF-8\r\n", 
	 res->content_type == NULL ? "text/html" : res->content_type);

  if (res->data_len)
    {
      printf("Content-Length: %d\r\n", res->data_len);
      printf("Connection: close\r\n"); /* if fullHttpReply */
    }

  printf("\r\n");
}

void
http_response_send(HttpResponse *res)
{
  http_response_send_headers(res);

  if( res->data )
    {
      int n_bytes = res->data_len;
      
      /* Dont send '\0' in response */
      // fix wrong response length
      //if (res->data[n_bytes-1] == '\0') 
      //  n_bytes--;

      fwrite(res->data, 1, n_bytes, stdout);
    }
}


void
http_response_send_bigfile
    (HttpResponse *res, char *filename, char *content)
/* ! basic, just read and copy file bit/bit : save memory but cost time */
{ 
  http_response_set_content_type(res, content);
  http_response_send_headers(res);  
  //unsigned char *data;
  FILE *fp=fopen(filename,"rb");
  /* get file size */
  fseek (fp , 0 , SEEK_END);
  unsigned long datasize = ftell(fp);
  /* copy bytes */
  rewind (fp);
  while ( datasize-- )
    fputc(fgetc(fp),stdout);
  fclose(fp);
}


/*
** SIGINT handler.
*/

void  
sigint(int sig)
{
  printf("\nCiwiki stopped.\n");
  syslog(LOG_LOCAL0|LOG_INFO, "Ciwiki stopped.\n");

  (void)signal(SIGINT, SIG_DFL);
  (void)kill(getpid(), SIGINT);

  /* NOTREACHED */
  exit(1);
}

/*
** SIGTERM handler
*/
void
sigterm(int sig)
{
  printf("\nCiwiki stopped.\n");
  syslog(LOG_LOCAL0|LOG_INFO, "Ciwiki stopped.\n");

  (void)signal(SIGTERM, SIG_DFL);
  (void)kill(getpid(), SIGTERM);

  /* NOTREACHED */
  exit(1);

}

/*
** Maximum number of child processes that we can have running
** at one time before we start slowing things down.
*/
#define MAX_PARALLEL 5

/*
** Implement an HTTP server daemon.
*/
HttpRequest*
http_server(struct in_addr address, int iPort)
{
  int                listener;      /* The server socket */
  int                connection;    /* A socket for each connection */
  fd_set             readfds;       /* Set of file descriptors for select() */
  socklen_t          lenaddr;       /* Length of the inaddr structure */
  int                child;         /* PID of the child process */
  int                nchildren = 0; /* Number of child processes */
  struct timeval     delay;         /* How long to wait inside select() */
  struct sockaddr_in inaddr;        /* The socket address */
  int                reuse = 1;
  int                n = 0;
  char               url_prefix[256];
  int                err_flag=0; //not used

  /* catch SIGINT */
  (void) signal(SIGINT, sigint);

  /* catch SIGTERM */
  (void) signal(SIGTERM, sigterm);

  memset(&inaddr, 0, sizeof(inaddr));
  inaddr.sin_family = AF_INET;
  inaddr.sin_addr.s_addr = address.s_addr;
  inaddr.sin_port = htons(iPort);
  listener = socket(AF_INET, SOCK_STREAM, 0);

  fprintf(stderr,"CiWiki firing up ...\n");

  if( listener < 0 )
  {
    fprintf(stderr,"Can't create a socket\n");
    exit(1);
  }

#ifdef SO_REUSEADDR
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); 
#endif

  while (n < 10)
  {
    fprintf(stderr,"Attempting to bind to %s:%i .. ", inet_ntoa(address), iPort);

    inaddr.sin_port = htons(iPort + n);

    if( bind(listener, (struct sockaddr*)&inaddr, sizeof(inaddr)) < 0 )
    {
      fprintf(stderr,"Failed! \n");
      n++;
      continue;
    }

    fprintf(stderr,"Success! \n");
    break;
  }

  if (n == 10)
  {
    fprintf(stderr,"Can't bind to any ports, giving up.\n");
    exit(1);
  }

  fprintf(stderr,"CiWiki Started. Please point your browser at %s:%i\n", inet_ntoa(address), iPort);

  /* log starting information */
  openlog("Ciwiki", 0, 0);
  syslog(LOG_LOCAL0|LOG_INFO, "started with PID %d", getpid());

  /* Set CIWIKI_URL_PREFIX if not already set - rss uses it */
  snprintf(url_prefix, 256, "%s:%i/", inet_ntoa(address), iPort+n);
  setenv("CIWIKI_URL_PREFIX", url_prefix , 0);

  listen(listener,10);
  
  /* Listen undefinitely (the father stays in the loop) */
  while( 1 )
  {
    if( nchildren > MAX_PARALLEL )
    {
      /* Slow down if connections are arriving too fast */
      sleep( nchildren - MAX_PARALLEL );
    }

    delay.tv_sec = 60;
    delay.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET( listener, &readfds);

    /* check if incoming data are available */
    if( select( listener+1, &readfds, 0, 0, &delay) )
    {
      lenaddr = sizeof(inaddr);
      /* accept a new connection on a socket */
      connection = accept(listener, (struct sockaddr*)&inaddr, &lenaddr);
      /* if successful connection */
      if( connection >= 0 )
      {
        /* fork() returns 0 to the child and the child's pid to the father */
        child = fork(); //father+child are running now
        if( child != 0 )
        {
          /* Here, the father decreases the nb of children available 
           * and closes the connection (child will take it) */
          if( child > 0 ) nchildren++;
          close(connection);
        }
        else
        {
          /* There, the child connects stdin/out to the socket */
          /* then returns req object for caller to handle */
          /* dup creates an alias and returns the lowest free descriptor) 
           * dup fails if dup() < 0 */
          close(0); //close stdin
          if ( dup(connection) < 0) //dup connection on stdin
            err_flag=1;
          close(1); //close stdout
          if ( dup(connection) < 0) //dup connection on stdout
            err_flag=1;
          close(2); //close stderr
          if ( dup(connection) < 0) //dup connection on stderr
            err_flag=1;
          close(connection); //alias are done, don't need it anymore
           /* The child exits here, 
            * http_request_new() will be called and will return the request */
          return http_request_new();
        }
      }
    }
    /* Bury dead children */
    while( waitpid(0, 0, WNOHANG)>0 ) nchildren--;
  }
  /* NOT REACHED */  
  exit(1);
}
