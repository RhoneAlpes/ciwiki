/* 
 *  CiWiki - a small lightweight wiki engine. 
 *  based on Didiwiki (Matthew Allum <mallum@o-hand.com>)
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

#ifndef _HAVE_CIWIKI_HEADER
#define _HAVE_CIWIKI_HEADER

#define _GNU_SOURCE

#include "config.h"

#if defined(__MINGW32__)

/* This is broken */

#undef DATADIR

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <dirent.h>

#ifndef WNOHANG
#define WNOHANG 1
#endif

#ifndef F_OK
#    define F_OK 00
#endif
#ifndef X_OK
#    define X_OK 01
#endif
#ifndef W_OK
#    define W_OK 02
#endif
#ifndef R_OK
#    define R_OK 04
#endif

#else

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <getopt.h>
#include <syslog.h>

#endif

#define TRUE  1
#define FALSE 0
//min. change disconnect ip below
#define DISCONNECT 30
//scripts are located in /usr/share/ciwiki/scripts
//path to notifynewuser. sddout+stderr are redirected and script runs in background
#define SCRIPTMAIL "/usr/local/libexec/notifynewuser.sh" 
//path to styles.css
#define PATH_CSS "/usr/share/ciwiki/styles/styles.css"
//these folders are located in $HOME/.ciwiki
#define PICSFOLDER "images"
#define FILESFOLDER "files"
#define HTMLFOLDER "html"
#define SCRIPTSFOLDER "scripts"
#define ACCESSFOLDER "permission"
//max file size (byte) for http_response_send_smallfile(), larger = more memory required
#define MAXFILESIZE 1000000

#include "util.h"
#include "http.h"
#include "wiki.h"
#include "wikilogin.h"
#endif

