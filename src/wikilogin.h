#ifndef _HAVE_WIKILOGIN_HEADER
#define _HAVE_WIKILOGIN_HEADER

void
wikilogin_setpermission(char *ipsrc, char *username);

int
wikilogin_getpermission(char *ipsrc);

int 
wikilogin_isvalid(char *username, char *password, 
                  char *email, char *code, 
                  char *newaccount, char *ipsrc);

char *wikirac_isvalid(char *command, char *ipsrc);

void
wikilogin_logoff(char *ipsrc);

char 
*wikilogin_username(char *ipsrc);

int
wikilogin_chgpwd(char *ipsrc, char *password, char *newpassword);

#endif
