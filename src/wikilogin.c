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

/*  Note:  The login and access grant is simple and old fashion, 
 * without extra dependency. It is just intended to protect 
 * a little bit the privacy of personal pages and decrease the risk 
 * of vandalism.
 * There is no hash table for fast lock up, so the login can be long 
 * if there are a lot of users online.
 * 
 * it's not very secure:
 *  registered users, session open, unvanted ips files are just hidden, 
 *  The user pwd is simply crypted (hashed)
 * 
 * Unwanted ips can be added manually:
 *   create .unwanted.txt in .ciwiki folder
 *   and add line with ip address to reject.
 * 
 * The file ~/newwikiusers.txt contains the new users, 
 * the code after C: and the user name after U:
 * must be sent by email, with sendmail and little bash script.
 * 
 * */


#include "ci.h"
#include "time.h"

extern int hostlogin;
extern int nologin;
extern int dosendmail;
extern int port;

/* djb2 */
unsigned long long
hash(char *str)
{
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

/* remove logged off ip - one loop */
void
wikilogin_logoff(char *ipsrc)
{
  FILE *fp;
  char line[100];
  char *str_ptr;
  int lgipsrc=strlen(ipsrc);

  /*  search */
  if ((fp = fopen(ACCESSFOLDER"/.session.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
    {
      if ( *line != '*' && (str_ptr=strchr(line,':')) )
      {
        /* skip space */
        str_ptr=line;
        while (*str_ptr == ' ')
        str_ptr++;
        /* same ip? */
        if ( !strncmp(str_ptr,ipsrc,lgipsrc) ) 
        {
          /* mark this address dead */
          fseek(fp,-(strlen(line)),SEEK_CUR);
          fputc('*',fp);
          fclose(fp);
          return; //done
        }
      }
    }
    fclose(fp);
  }
  //there is a problem if we exit here!
}

/* remove dead ip - one loop */
void
wikilogin_cleanpermission(void)
{
  FILE *fp;
  char line[100];
  char *str_ptr;
  long val;
  /* cancel ip after a while without page read */
  long now=(time(NULL)/60)-DISCONNECT; //defined in ci.h
  
  /*  search */
  if ((fp = fopen(ACCESSFOLDER"/.session.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
    {
      if ( *line != '*' && (str_ptr=strchr(line,':')) )
      {
        /* skip space */
        str_ptr++;
        while (*str_ptr == ' ')
          str_ptr++;
        val= atol(str_ptr); //time last page read
        if ( val != 0 && val < now) //nothing for a while? 
        {
          /* mark this address dead */
          fseek(fp,-(strlen(line)),SEEK_CUR);
          fputc('*',fp);
          fseek(fp,strlen(line)-1,SEEK_CUR);
        }
      }
    }
    fclose(fp);
  }
}

/* routine with 2 successive loops 
 * identify with the ip address not the most secure
 * acceptable for non-critic informations */
void
wikilogin_setpermission(char *ipsrc, char *username)
{
  FILE *fp;
  char line[100];
  int lgipsrc=strlen(ipsrc);
  char *str_ptr;
  
  if ((fp = fopen(ACCESSFOLDER"/.session.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
    {
      /* skip space */
      str_ptr=line;
      while (*str_ptr == ' ')
        str_ptr++;
      /* same ip? */
      if ( !strncmp(line,ipsrc,lgipsrc) )
      {
        fclose(fp);
        return; // already logged, nothing to do!
      }
    }
    /* search for a dead ip and replace it */
    rewind(fp);
    while ( fgets(line,100,fp) )
    {
      if (line[0] == '*')
      {
        fseek(fp,-(strlen(line)), SEEK_CUR);
        fprintf(fp,"%32s:%16li:%24s\n", ipsrc, time(NULL)/60, username);
        fclose(fp);
        return; //ip recorded
      }
    }
  }
  /* just add ip at the end */
  fp = fopen(ACCESSFOLDER"/.session.txt", "a");
  fprintf(fp,"%32s:%16li:%24s\n", ipsrc, time(NULL)/60, username); 
  fclose(fp);
  return; //ip recorded
}

/* This routine is called at each page
 * it looks in hidden files to grant or not permission
 * look at .unwanted.txt to reject ip (login or not)
 * look at .alwayswanted.txt to accept ip without login required
 * look at .session.txt to accept logged used
 *  with two imbricated loops (sequential search)   */
int
wikilogin_getpermission(char *ipsrc)
{
  FILE *fp;
  char line[100];
  int lgipsrc=strlen(ipsrc);
  char *str_ptr;

  /* Unwanted ips: theorically not useful because their logins 
   * has been refused. */    
  if ((fp = fopen(ACCESSFOLDER"/.unwanted.txt", "r"))) 
  {
    while ( fgets(line,100,fp) )
      if ( !strncmp(line,ipsrc,lgipsrc) )
      {
        fclose(fp);
        return 0; // refused
      }
    fclose(fp);
  }
  
  if ( nologin )
  /* any user registered or not is automatically authorized */
    return 1;
  
  if ( hostlogin )
  /* this computer (localhost is automatically authorized)*/
  {
    if ( !strncmp("127.0.0.1",ipsrc,lgipsrc) )
      return 1;
  }
    
  /* Always accepted ips: automatically logged in */    
  if ((fp = fopen(ACCESSFOLDER"/.alwayswanted.txt", "r"))) 
  {
    while ( fgets(line,100,fp) )
      if ( !strncmp(line,ipsrc,lgipsrc) )
      {
        fclose(fp);
        return 1; // accepted
      }
    fclose(fp);
  }

  /*  sequential search */
  if ((fp = fopen(ACCESSFOLDER"/.session.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
    {
      /* skip space */
      str_ptr=line;
      while (*str_ptr == ' ')
        str_ptr++;
      /* same ip? */
      if ( !strncmp(str_ptr,ipsrc,lgipsrc) )
      {
        /* update the access time 
         * move back 24 chars username + ':' + 12 chars time + '\n' */
        fseek(fp,-42,SEEK_CUR);
        fprintf(fp, "%16li",time(NULL)/60); 
        fclose(fp);
        return 1;
      }
    }
    fclose(fp);
  }
  return 0;
}


/* A very simple login procedure, old fashion!
 * Acceptable for a semi private wiki with few users
 * return permission val > 0
 * or error code < 0 
 * -1   : name or pwd too short
 * -2   : invalid char in name or pwd
 * -3   : name or pwd too long
 * -10  : wrong pwd
 * -20  : wrong usr name
 * -30  : existing username
 * -40  : wrong access code
 * -100 : prog error
 * */
int
wikilogin_isvalid(char *username, char *password, 
                  char *email, char *code, 
                  char *newaccount, char *ipsrc)
{
  FILE *fp;
  int newac;
  char line[100];
  char *str_ptr;
  int lgusr=strlen(username);
  int lgpwd=strlen(password);
  int lgipsrc=strlen(ipsrc);
  int permission=1,activate=1;
  int i;
  char pwd[128]="",codeok[128]="";
  char newusersfile[512] = { 0 };
  char sendmailcmd[512] = { 0 };
  
  /* oportunity to make room */
  wikilogin_cleanpermission();

  if ( newaccount && !strcmp(newaccount,"on") ) newac=1;
    else newac=0;
  
  /* too short */
  if (lgusr < 8 || lgpwd < 8)
    return -1;
  /* too long */
  if (lgusr > 24 || lgpwd > 24)
    return -3;
    
  /* space is used as separator
   * else any other chars are accepted */
  for (i=0; i<lgusr; i++)
    if (isspace(username[i]))
      return -2;
  for (i=0; i<lgpwd; i++)
    if (isspace(password[i]))
      return -2;
      
  sprintf(pwd,"%016llx",hash(password)); 
  lgpwd=strlen(pwd);
  
  /* Unwanted ips */    
  if ((fp = fopen(ACCESSFOLDER"/.unwanted.txt", "r"))) 
  {
    while ( fgets(line,100,fp) )
      if ( !strncmp(line,ipsrc,lgipsrc) )
      {
        fclose(fp);
        return 0; // refused
      }
    fclose(fp);
  }
     
  /* verify username and password 
   * Normal login procedure */
  if (!newac)
  {
    /*  search */
    if ((fp = fopen(ACCESSFOLDER"/.login.txt", "r"))) 
    {
      while ( fgets(line,100,fp) )
        if ( !strncmp(line,username,lgusr) )
        {
          fclose(fp);
          if ( (str_ptr=strchr(line,' ')) &&
            !strncmp(str_ptr+1,pwd,lgpwd) )
            return 1; //okay
          else
            return -10; //wrong password
        }
      fclose(fp);
      return -20; //username not found
    }
    else
    {
      /* store in log file */
      syslog(LOG_LOCAL0|LOG_INFO, "Error: cannot open login.txt");  
      return -20; //perhaps the first user
    }
  }
  
  /* * * * * * * New user routine  * * * * * */
  
  /* check if existing username */
  if ((fp = fopen(ACCESSFOLDER"/.login.txt", "r"))) 
  {
    while ( fgets(line,100,fp) )
      if ( !strncmp(line,username,lgusr) )
      {
        fclose(fp);
        return -30; //sorry existing username
      }
  }
  
  /* check if email seems valid */
  if ( !strchr(email,'@') || !strchr(email,'.') )
    return -35;
  
  /* is it a valid code ? */
  sprintf(codeok,"%llx",hash(username)); //access code with username
  if ( code && strcmp(code,codeok) != 0 )
  {
    /* new registrations are in ~/newwikiusers.txt */
    snprintf(newusersfile, 512, "%s/newwikiusers.txt", getenv("HOME"));
    if ( !(fp = fopen(newusersfile, "a")) )
      return -100; //check write permission!
    fprintf(fp, "U:%s P:%s M:%s I:%s T:%li C:%s\n",
          username,password,email,
          ipsrc,time(NULL)/60,codeok); 
    fclose(fp);  

		//if ( dosendmail )
		///* system calls sh script to send an email */
		//{
		//  snprintf(sendmailcmd, 512, "%s%s", getenv("HOME"),SCRIPTMAIL);
		//  system(sendmailcmd);  //system is not secure execl is better
		//}
		//no login yet (user will have to return with the validation code)
		//return  -40;
   	    //}
   
    if ( dosendmail )
    /* call sh script to send email */
    {
      snprintf(sendmailcmd, 512, "%s -p %i >/dev/null 2>&1 &", SCRIPTMAIL,port);
      /* Attempt to fork and check for errors */
      pid_t pid = fork();
      switch( pid )
      {
        case 0:
            /* Replace the child fork with a new process */
            if(execl("/bin/sh","sh","-c",sendmailcmd, NULL) == -1)
            {
              /* store failure in log file */
              syslog(LOG_LOCAL0|LOG_INFO, "Error: excl %s\n",sendmailcmd); 
              return -100;
            }
            else
            {
               //syslog(LOG_LOCAL0|LOG_INFO, "Success excl: %s\n",sendmailcmd); 
			   //mail immediatly sent and user will have return with the validation code)	
              return -40;
            }
        
        case -1:
            /* should not occur */
            fprintf(stderr,"Wikilogin line %i Cannot fork!\n",__LINE__);
            return -100;
            
        default:
            /* Parent record the pid */
            syslog(LOG_LOCAL0|LOG_INFO, "Sendmail pid=%i\n",pid);
      }
    }
    //mail is not immediatly sent but user will have to return with the validation code!
    return -40;
  }
   
          
  if ( code )
  {
    /* add a new user */
    if (!(fp = fopen(ACCESSFOLDER"/.login.txt", "a+"))) 
      return -100; //check write permission!

    fprintf(fp, "%s %s %s %s %li %i %i\n",
            username,pwd,email,
            ipsrc,time(NULL)/60,permission,activate); 
    fclose(fp);
    return 1; //account created
  }
  
  /* There's something wrong if we arrive here! */
  return -100;
}

char *wikirac_isvalid(char *command, char *ipsrc)
/* this function validates a new user registration with the
 * direct link provided in the email */
{
  FILE *fp;
  char *password,*email,*username;
  char *code;
  char codeok[128],pwd[128];
  int permission=1,activate=1;
  char *str_ptr1,*str_ptr2;
  
  /* command should contains the email,code,username,pwd */
  str_ptr1=command;
  if (( str_ptr2=strchr(str_ptr1,',')) )
  {
    *str_ptr1='\0';
    email=str_ptr1;
    str_ptr1=str_ptr2+1;
  }
  else return NULL;
  
  if (( str_ptr2=strchr(str_ptr1,',')) )
  {
    *str_ptr2='\0';
    code=str_ptr1;
    str_ptr1=str_ptr2+1;
  }
  else return NULL;
  
  if (( str_ptr2=strchr(str_ptr1,',')) )
  {
    *str_ptr2='\0';
    username=str_ptr1;
    str_ptr1=str_ptr2+1;
  }
  else return NULL;
  
  if ( strlen(str_ptr1) <= 24 )
    password=str_ptr1;
  else return NULL;
  
  /* is it a valid code ? */
  sprintf(codeok,"%llx",hash(username)); //access code with username
  if ( strcmp(code,codeok) == 0 )
  {
    sprintf(pwd,"%016llx",hash(password));
    
    /* add a new user */
    if (!(fp = fopen(ACCESSFOLDER"/.login.txt", "a+"))) 
      return NULL; //check write permission!

    fprintf(fp, "%s %s %s %s %li %i %i\n",
          username,pwd,email,
          ipsrc,time(NULL)/60,permission,activate); 
    fclose(fp);
    
    return username; //account created
  }
  else
    return NULL; //wrong code
}

char 
*wikilogin_username(char *ipsrc)
{
  FILE *fp;
  static char line[100];
  char *str_ptr;
  int lgipsrc=strlen(ipsrc);
  
  if ((fp = fopen(ACCESSFOLDER"/.session.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
    {
      /* skip space */
      str_ptr=line;
      while (*str_ptr == ' ')
        str_ptr++;
      /* same ip? */
      if ( !strncmp(str_ptr,ipsrc,lgipsrc) )
      {
        fclose(fp);
        /* will point to the username */
        if ( (str_ptr=strchr(str_ptr,':')) 
          && (str_ptr=strchr(str_ptr+1,':')) )
        {
          /* skip space, suppress eol */
          str_ptr++;
          while ( *str_ptr == ' ' )
            str_ptr++;
          *strchr(str_ptr,'\n')='\0';
          return str_ptr; 
        }
      }
    }
  }
  return NULL;
}

int
wikilogin_chgpwd(char *ipsrc, char *password, char *newpassword)
{
  FILE *fp;
  static char line[100];
  char *str_ptr;
  char  *username;
  char pwd[128]="",newpwd[128]="";
  int i;
  int lgpwd = strlen(newpassword);
  
  if ( lgpwd < 8) return(-1); //new password too short
  if (lgpwd > 24) return(-3); //new password too long
 
  for (i=0; i<lgpwd; i++)
    if (isspace(newpassword[i]))  //no space
      return -2;
  
  username = wikilogin_username(ipsrc);
  int lgusr = strlen(username);
  sprintf(pwd,"%016llx",hash(password)); 
  lgpwd = strlen(pwd);
  sprintf(newpwd,"%016llx",hash(newpassword)); 
  
  /* store in log file */
  //syslog(LOG_LOCAL0|LOG_INFO, "OPWD:<%s><%s>***NPWD:<%s><%s>",
  //        password,pwd,newpassword,newpwd);  
  
  /*  search */
  if ((fp = fopen(ACCESSFOLDER"/.login.txt", "r+"))) 
  {
    while ( fgets(line,100,fp) )
      if ( !strncmp(line,username,lgusr) )
      {
        if ( (str_ptr=strchr(line,' ')) &&
          !strncmp(str_ptr+1,pwd,lgpwd) )
        {
          fseek(fp,1-(strlen(str_ptr)), SEEK_CUR);
          fprintf(fp, "%s",newpwd);
          fclose(fp);
          return 1; //okay
        }
        else
        {
          fclose(fp);
          return -10; //wrong password
        }
      }
    fclose(fp);
    return -20; //username not found
  }
  return -100; //cannot open
}

