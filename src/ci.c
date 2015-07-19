#include "ci.h"

static int debug;
int hostlogin; //declared external in wikilogin.c
int nologin; //declared external in wikilogin.c
int dosendmail; //declared external in wikilogin.c
int lgindex; //declared external in wiki.c
int loginform; //declared external in wiki.c
int port = 8000; //declared external in wikilogin.c
int max_upload_size = 100000; //declared external in http.c
int Exec_allowed=1; //declared external in wiki.c
int Upload_allowed=1; //declared external in wiki.c

int
usage()
{ 
  fprintf(stderr, "Usage: ciwiki [options]\n");
  fprintf(stderr, "   -a, --autologin       : login localhost automatically\n");
  fprintf(stderr, "   -n, --nologin         : login automatically all users\n");
  fprintf(stderr, "   -d, --debug           : debug mode, listens to requests from stdin\n");
  fprintf(stderr, "   -h, --home   <path>   : specify ciwiki's home directory\n");
  fprintf(stderr, "   -l, --listen <ipaddr> : specify IP address\n");
  fprintf(stderr, "   -p, --port   <port>   : specify port number \n");
  fprintf(stderr, "   -r, --restore <what>  : -r=help,home,style :restore the default WikiHelp, WikiHome, Styles.css\n");
  fprintf(stderr, "   -t, --html            : redirect WikiHome to /html/index.html page\n");
  fprintf(stderr, "       --help            : display this help message\n");
  fprintf(stderr, "   -s, --sendmail        : run sendmail script automatically\n");
  fprintf(stderr, "   -i, --index <length>  : override the default index length\n");
  fprintf(stderr, "   -m, --maxsize <length>: override the default upload max file size\n");
  fprintf(stderr, "   -u, --unmasked        : login password is visible\n");
  fprintf(stderr, "   -v, --version         : display the version\n");
  exit(1);
}

int 
main(int argc, char **argv)
{
  HttpRequest    *req  = NULL;
  int             c;
  char           *ciwiki_home = NULL;
  unsigned int  restore_Wiki = 0;
  unsigned int  create_htmlHome = 0;
  struct in_addr address;

  /* default values */
  debug = 0; //normal mode
  hostlogin = 0; //host will have to login
  nologin = 0; //users will have to login.
  dosendmail = 0; //don't send systematically email at each registration
  lgindex = 20; //print 20 files before to make a new index box
  loginform = 0; //use LOGINFORM (see wikitext.h) => password is hidden
  

  /* by default bind server to "0.0.0.0" */
  address.s_addr = inet_addr("0.0.0.0");

  while (1)
  {
    static struct option long_options[] = 
    {
      {"autologin",  no_argument,   0, 'a'},
      {"nologin",  no_argument,     0, 'n'},
      {"debug",  no_argument,       0, 'd'},
      {"version",  no_argument,     0, 'v'},
      {"listen", required_argument, 0, 'l'},
      {"port",   required_argument, 0, 'p'},
      {"home",   required_argument, 0, 'h'},
      {"restore",required_argument, 0, 'r'},
      {"html",   no_argument,       0, 't'},
      {"sendmail",  no_argument,    0, 's'},
      {"index",  required_argument, 0, 'i'},
      {"maxsize",required_argument, 0, 'm'},
      {"unmasked",     no_argument, 0, 'u'},
      {"help",   no_argument,       0,  10 },
      {0, 0, 0, 0}
    };

    /* getopt_long stores the option index here */
    int option_index = 0;
    
    c = getopt_long (argc, argv, "andl:p:h:i:m:r:tsvu", long_options, &option_index);

    /* detect the end of the options */
    if (c == -1)
    break;

    switch (c)
    {
      case 0:
        break;
      case 'u': //password is visible during login
         loginform=2;
         break;
      case 'i': //set index length
        lgindex = atoi(optarg);
        if (lgindex==0) lgindex=20;
        fprintf(stderr,"Index length = %i\n",lgindex);
        break;   
      case 'm': //max file size to upload
        max_upload_size = atoi(optarg);
        if (max_upload_size<0) max_upload_size=0;
        fprintf(stderr,"Max size allowed to upload = %i kB\n",max_upload_size);
        break;     
      case 'a': //autologin for the localhost
        hostlogin = 1;
        fprintf(stderr,"Localhost is logged in.\n");
        break;
      case 'n': //autologin any user
        nologin = 1;
        fprintf(stderr,"Any user registrered or not will be logged in.\n");
        break;  
      case 'd':
        debug = 1;
        break;      
      case 'v':
        printf("CiWiki - version %s\n\n",VERSION);
        return 0;         
      case 'p': //default port is 8000
        port = atoi(optarg);
        break;   
      case 'h': //default home directory is ~/.ciwiki
        ciwiki_home = optarg;
        break;
      case 'l': //listen a inet address
        if(inet_aton(optarg,&address) == 0) 
        {
          fprintf(stderr, "ciwiki: invalid ip address \"%s\"\n", optarg);
          exit(1);
        } else
          address.s_addr = inet_addr(optarg);
        break;
      case 'r': //rewrite Wikihelp, Wikihome and styles.css
        if ( strstr(optarg,"help") )
            restore_Wiki|= 1;
        if ( strstr(optarg,"home") )
            restore_Wiki|= 2;
        if ( strstr(optarg,"style") )
            restore_Wiki|= 4;
        fprintf(stderr, "Restore wiki code:%u\n", restore_Wiki); 
        break;
      case 't': //redirect Wikihome to /html/index.html
        create_htmlHome=1; 
        break;        
      case 's':
        dosendmail= 1;
        break;
      case 10:
        usage();
        break;
      default:
        usage();
    }
  } //end while

  wiki_init(ciwiki_home,restore_Wiki,create_htmlHome); /* create folders... */

  if (debug)
  {
    req = http_request_new();   /* reads request from stdin  */
  }
  else 
  {
    req = http_server(address, port);    /* forks here - http_request_new will handle a single http request*/
  }
  /* the child continues here */
  wiki_handle_http_request(req); /* proceeds request (main function in wiki.c) */
  /* the child terminates there */
  return 0;
}
