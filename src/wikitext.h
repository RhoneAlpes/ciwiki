/* 
 *  CiWiki a fork of DidiWiki, a small lightweight wiki engine. 
 *  CiWiki Copyright 2010...2015 Jean-Pierre Redonnet <inphilly@gmail.com>
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

#ifndef _HAVE_WIKI_TEXT_HEADER
#define _HAVE_WIKI_TEXT_HEADER

#define BACK \
"href='javascript:javascript:history.go(-1)'>&#60&#60&#60Back\n" \

#define EDITHEADER \
\
"<div id='header'>\n" \
"<form method='get' action='Search'>\n" \
"<table border='0' width='100%%'>\n" \
"<tr>\n" \
"<td align='left' ><strong>%s</strong>" \
" ( <a href=%s?edit title='Edit this wiki page contents. [alt-j]' accesskey='j'>Edit</a> | " \
"<a href=%s?delete title='Delete page. [alt-d]' accesskey='d'>Delete</a> ) </td>" \
"<td align='right' >\n" \
"<a href='Login' title='Login page. [alt-l]' accesskey='l'>%s</a> |\n"  \
"<a href='WikiHome' title='Visit Wiki home page. [alt-z]' accesskey='z'>Home</a> |\n"  \
"<a href='Index' title='List all pages in the wiki. [alt-i]' accesskey='i' >Index</a> | \n"  \
"<a href='Changes' title='List recent changes in the wiki. [alt-r]' accesskey='r' >Changes</a> | \n"  \
"<a href='Create' title='Create a new wiki page by title. [alt-c]' accesskey='c'>New</a> | \n"    \
"<a href='WikiHelp' title='Get help on wiki usage and formatting.'>Help</a>  \n"    \
"<input type='text' name='expr' size='15' title='Enter text to search for and press return.' /> </td>\n"  \
"</tr>\n"                                                                \
"</table>\n"                                                             \
"</form>\n"                                                              \
"</div>\n"                                                               \
"<div id='wikidata'>\n"

#define PAGEHEADER                                                       \
                                                                         \
"<div id='header'>\n"                                                    \
"<form method='get' action='Search'>\n"                                 \
"<table border='0' width='100%%'>\n"                                      \
"<tr>\n"                                                                 \
"<td align='left' ><strong>%s</strong> %s</td>\n"         \
"<td align='right' >\n"                                    \
"<a href='WikiHome' title='Visit Wiki home page. [alt-z]' accesskey='z'>Home</a> |\n"  \
"<a href='Index' title='List all pages in the wiki. [alt-i]' accesskey='i' >Index</a> | \n"  \
"<a href='Changes' title='List recent changes in the wiki. [alt-r]' accesskey='r' >Changes</a> | \n"                                    \
"<a href='Create' title='Create a new wiki page by title. [alt-c]' accesskey='c'>New</a> | \n"                                         \
"<a href='WikiHelp' title='Get help on wiki usage and formatting.'>Help</a>  \n"                                       \
"<input type='text' name='expr' size='15' title='Enter text to search for and press return.' /> </td>\n"                               \
"</tr>\n"                                                                \
"</table>\n"                                                             \
"</form>\n"                                                              \
"</div>\n"                                                               \
"<div id='wikidata'>\n"


#define PAGEFOOTER                                                       \
                                                                         \
"</div><div id='footer'>CiWiki alias DidiWiki, Version: " VERSION "</div>\n"


#define SHORTHELP                                                    \
                                                                         \
"<div id='header'>\n"                                                    \
"<table border='0' width='100%%'>\n"                                      \
"<tr>\n"                                                                 \
"<td align='left' > =H1 ==H2 ===H3 | *bold* | /italic/ | _underscore_ | -strikethrough- | +highlight+ | 'quote | "\
"[link] | * list | ** sublist | # ordered | ---- | {{toc}} | {{private}}</td>\n"\
"</tr>\n"                                                           \
"</table>\n"                                                             \
"</div>\n"                                                               \
"<div id='wikidata'>\n"


#define LOGINFORM                                                       \
                                                                        \
"<h1>Login</h1>"                                                        \
"<p><a href='NewAccount'>New user? Please create a new account.</a></p>\n" \
"<form method=POST action='Login'>\n"                                  \
"<table border='0' width='100%%'>\n"                                      \
"<tr>\n"                                                                 \
"<td align='right'>User name:</td>"                                      \
"<td align='left'><input type='text' name='username' /></td>\n"         \
"</tr><tr>\n"                                                           \
"<td align='right'>Password:</td>"                                       \
"<td align='left'><input type='password' name='password' /></td>\n"        \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' value='Login' /></td>\n" \
"</tr></table>\n"                                                      \
"</form>\n" 



#define LOGINFORM2                                                      \
                                                                        \
"<h1>Login</h1>"                                                        \
"<p><a href='NewAccount'>New user? Please create a new account.</a></p>\n" \
"<form method=POST action='Login'>\n"                                  \
"<table border='0' width='100%%'>\n"                                      \
"<tr>\n"                                                                 \
"<td align='right'>User name:</td>"                                      \
"<td align='left'><input type='text' name='username' /></td>\n"         \
"</tr><tr>\n"                                                           \
"<td align='right'>Password:</td>"                                       \
"<td align='left'><input id='password' name='password' style='padding: 5px;' type='password' />\n" \
"<i>Show it:</i><input id='reveal' name='reveal' onchange='reveal_pass(this); ' type='checkbox' value='1' /></td>\n"    \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' value='Login' /></td>\n" \
"</tr></table>\n"                                                      \
"</form>\n"                                                      \
"<script type='text/javascript'>\n"        \
"function reveal_pass(check_box){\n"        \
" var textbox_elem = document.getElementById('password');\n"        \
" if(check_box.checked)\n"        \
" textbox_elem.setAttribute('type', 'text');\n"        \
" else\n"        \
" textbox_elem.setAttribute('type', 'password');\n"        \
"}\n"        \
"</script>\n"



#define NEWLOGINFORM                                                       \
                                                                        \
"<h1>Create a new Account</h1>"                                         \
"<p>Please choose your user name, your password and enter a valid email address.<br>" \
"You will receive your validation code on your email</p>\n" \
"<form method=POST action='Login'>\n"                                  \
"<table border='0' width='100%%'>\n"                                      \
"<tr>\n" \
"<td align='right'>User name:</td>"                                      \
"<td align='left'><input type='text' name='username' /></td>\n"         \
"</tr><tr>\n"                                                           \
"<td align='right'>Password:</td>"                                       \
"<td align='left'><input type='text' name='password' /></td>\n"        \
"</tr><tr>\n"                                                           \
"<td align='right'>Your Email:</td>"                                     \
"<td align='left'><input type='text' name='email' /></td>\n"  \
"</tr><tr>\n"                                                           \
"<td align='left' colspan=2><br>A validation code will be returned by mail.<br><br></td>\n"     \
"</tr><tr>\n"                                                           \
"<td align='right'>Validation Code:</td>"                                \
"<td align='left'><input type='text' name='code' /></td>\n" \
"</tr><tr>\n"                                                           \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' value='Process it' /></td>\n" \
"</tr></table>\n"                                                      \
"<input type='hidden' name='newaccount' value='on' />  \n"              \
"</form>\n" 


#define CHGPWDFORM                                                      \
                                                                        \
"<form method=POST action='Login'>\n"                                   \
"<table border='0' width='100%%'>\n"                                    \
"<tr>\n"                                                                \
"<td align='right'>Old password:</td>"                                  \
"<td align='left'><input type='text' name='password' /></td>\n"         \
"</tr><tr>\n"                                                           \
"<td align='right'>New password:</td>"                                  \
"<td align='left'><input type='text' name='newpassword' /></td>\n"      \
"</tr><tr>\n"                                                           \
"<td align='right'>Reenter your password:</td>"                         \
"<td align='left'><input type='text' name='newpasswordbis' /></td>\n"   \
"</tr><tr>\n"                                                           \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' value='Login' /></td>\n"\
"</tr></table>\n"                                                       \
"</form>\n" 


#define LOGINFO                                                         \
                                                                        \
"<h1>Login Information</h1>"                                            \
"<form method=POST action='Login'>\n"                                  \
"<table border='0' width='100%%'>\n"                                    \
"<tr>\n"                                                                \
"<td align='right'>IP Address:</td><td>%s</td>"                         \
"</tr><tr>\n"                                                           \
"<td align='right'>Login name:</td><td>%s</td>"                         \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' name='logoff' value='Log Off' /></td>\n"\
"</tr><td span=2><br>Change your password:<br></td><tr>\n"             \
"<td align='right'>Old password:</td>"                                  \
"<td align='left'><input type='text' name='password' /></td>\n"         \
"</tr><tr>\n"                                                           \
"<td align='right'>New password:</td>"                                  \
"<td align='left'><input type='text' name='newpassword' /></td>\n"      \
"</tr><tr>\n"                                                           \
"<td align='right'>Reenter your password:</td>"                         \
"<td align='left'><input type='text' name='newpasswordbis' /></td>\n"   \
"</tr><tr>\n"                                                           \
"<td></td><td align='left'><input type='submit' name='chgpwd' value='Change Password' /></td>\n"\
"</tr><tr>\n"                                                           \
"</tr><td span=2><br><a href='javascript:javascript:history.go(-1)'>Return to the previous page.</a></td><tr>\n"             \
"</tr></table>\n"                                                      \
"</form>\n" 


#define CREATEFORM                                                       \
                                                                        \
"<form method=POST action='Create'>\n"                                  \
"Page name:<input type='text' name='title' />\n"                        \
"Select a template:\n"         \
"<select name='template'>\n"                        \
"  <option value='none'>none</option>\n"                        \
"  <option value='01.template'>template 1</option>\n"                        \
"  <option value='02.template'>template 2</option>\n"                        \
"  <option value='03.template'>template 3</option>\n"                        \
"  <option value='04.template'>template 4</option>\n"                        \
"  <option value='05.template'>template 5</option>\n"                        \
"</select>\n"                        \
"<p><input type='submit' value='Create' /></p>\n"                        \
"</form>\n" 


#define EDITFORM                                                         \
                                                                         \
"<form method=POST action='%s?edit' name='editform'>\n"                        \
"<textarea name='wikitext' rows='20' cols='80' wrap='virtual'>%s</textarea>\n" \
"<p><input type='submit' name='save' value='Save' title='[alt-k]' accesskey='k'>" \
"<input type='submit' name='preview' value='Preview' title='[alt-p]' accesskey='p'>" \
"<input type='submit' name='cancel' value='Cancel' title='[alt-c]' accesskey='c'></p>\n"                            \
"</form>\n"             \
"<script language='JavaScript'>\n"   \
"<!--\n"                             \
"/* Give the textarea focus ...less mouse usage but may annoy people ?*/ \n" \
"document.editform.wikitext.focus()\n" \
"document.editform.wikitext.scrollTop = document.editform.wikitext.scrollHeight;\n"          \
"//-->\n"                            \
"</script>\n"


#define EDITPREVIEW                                                      \
                                                                         \
"<div id=\"header\"><strong>Preview:</strong></div>\n"


#define DELETEFORM                                                       \
                                                                         \
"<form method=POST action='%s?delete' name='deleteform'>\n"              \
"Are you really sure you want to delete the page <strong>%s</strong>?\n" \
"<p><input type='submit' name='confirm' Value='OK'>\n"                   \
"<input type='submit' name='cancel' value='Cancel'></form>"


#define HOMETEXT ""                                                      \
                                                                         \
"==Welcome to !CiWiki [images/ciwiki.png] alias !DidiWiki\n"                                 \
"/!CiWiki / is a fork of /!DidiWiki / with several new features added.\n" \
"!CiWiki is a small and simple [http://en.wikipedia.org/wiki/Wiki WikiWikiWeb] implementation\n" \
"Ideal for personal note-taking, \"to do\" lists, and any other uses you can think of.\n" \
"!CiWiki is fast and does not require webserver, database mng and php interpreter.\n\n\n"  \
"For an example of how a !CiWiki entry looks in text form you can [?edit edit] this page. Also see WikiHelp for infomation on usage and formatting rules. Use The WikiSandbox to experiment.\n" \
"/!CiWiki / is written by [mailto://inphilly@gmail.com Jean-Pierre Redonnet] and released under [http://www.gnu.org GNU] [http://www.gnu.org/copyleft/gpl.html GPL]. \n" \
"Matthew Allum is the original author of /!DidiWiki (2004) \n"


#define HOMEREDIRECT ""                                                  \
                                                                         \
"==Redirect yor home page to /html/index.html\n"                          \
"html=redirect.index.html\n"                                             \
"if you are not automatically redirected, please click on \n"             \
"html=index.html\n"                                                      \
"\n"


#define HELPTEXT ""                                                      \
                                                                         \
"=Quick Guide \n"                                                    \
"Top toolbar usage\n"                                                    \
"* [?edit Edit] Allows you to edit pages ( see below for rules )\n"       \
"* [?delete Delete] Allows you to delete pages\n"                        \
"* [WikiHome Home] Takes you to the wiki front page\n"                \
"* [Index Index] List all the pages in the wiki\n"                \
"* [Changes Changes] Lists the pages changed by date\n"                  \
"* [Create New] Creates a new wiki page by title\n"                      \
"* [WikiHelp Help] Takes you to this help page\n"                            \
"\n"                                                                     \
"Use the text entry box to perform a very simple keyword search on the\n" \
"Wiki contents. Hit return to activate the search.\n"                    \
"\n"                                                                     \
"To search through the page names: Open the Index (and the index boxes), \n" \
"open the built in browser search, keys <ctrl><f> \n"           \
"\n"                                                                     \
"Use the WikiSandbox to practice formatting.\n"                         \
"\n"                                                                     \
"----\n"                                                                 \
"=Formatting rules\n"                                                    \
"\n"                                                                     \
"=Top Level Heading\n"                                                   \
"  =Top Level Heading (H1)\n"                                            \
"==Second Level Heading\n"                                               \
"  ==Second Level Heading (H2)\n"                                        \
"More levels \n"                                                         \
"  === (H3), ==== (H4) etc\n"                                            \
"----\n"                                                                 \
"The horizontal lines in this page are made with 4 or more dashes:\n"     \
" ---- Horizonal line\n"                                                 \
"----\n"                                                                 \
"\n"                                                                     \
"Paragraphs are separated by an empty line\n"                            \
"\n"                                                                     \
"Like this. Another paragraph.\n"                                        \
"\n"                                                                     \
" Paragraphs are separated by a blank line.\n"                           \
"\n"                                                                     \
" Like this. Another paragraph.\n"                                       \
"----\n"                                                                 \
"*Bold text*, /italic text/, _underscore text_ and -strikethrough-.\n"    \
"  *Bold text*, /italic text/, _underscore text_ and -strikethrough-.\n"  \
"/*Combination of bold and italics*/\n"                                  \
"  /*Combination of bold and italics*/\n"                                \
"----\n"                                                                 \
"\n"                                                                     \
"{B}Text color{B}\n"                                                     \
" {B}Text color{B}\n"                                                    \
"(Y)Background color(Y)\n"                                               \
" (Y)Background color(Y)\n"                                             \
"\n"                                                                    \
"Colors: {B}B blue{B}  {C}C cyan{C}  {G}G green{G}  {P}P pink{P}  {R}R red{R}  {Y}Y yellow{Y}\n" \
"\n"                                                                    \
"Colors: (B)B blue(B)  (C)C cyan(C)  (G)G green(G)  (P)P pink(P)  (R)R red(R)  (Y)Y yellow(Y)\n" \
"----\n"                                                                 \
"\n"                                                                  \
"+Highlight+\n"                                                       \
" +Highlight+\n"                                                      \
"-----\n"                                                              \
"\n"                                                                   \
"WikiLinks are formed by two or more words in /camelcase/.\n"           \
" WikiLinks are formed by two or more words in /camelcase/.\n"          \
"External links begin with http://, like http://www.freepan.org\n"       \
" External links begin with http://, like http://www.freepan.org\n"      \
"Forced wiki [links] are a alphanumeric string surrounded by square brackets.\n" \
" Forced wiki [links] are a alphanumeric string surrounded by square brackets.\n" \
"Named http links have text with an !http:// link inside, like [http://www.freepan.org FreePAN]\n" \
" Named http links have text with an !http:// link inside, like [http://www.freepan.org FreePAN]\n" \
"Sometimes !WordsShouldNotMakeAWikiLink so put a '! ' beforehand.\n"      \
" Sometimes !WordsShouldNotMakeAWikiLink so put a '! ' beforehand.\n"     \
"Same thing with !http://foobar.com\n"                                   \
" Same thing with !http://foobar.com\n"                                  \
"Open external links in new tags:\n"                                     \
" {{wwwlink=new_tab}} opens external link in a new tab\n"                \
" {{wwwlink=current_tab}} opens external link in a current tab\n"        \
"----\n"                                                                 \
"\n"                                                                     \
"Links to images display the image:\n"                                   \
"\n"                                                                     \
" Pictures size and border: {{width=100 height=50 border=2}}\n"          \
" Return to the default width, height and border: {{picture=default}}\n"  \
"\n"                                                                     \
"Some text before\n"                                                     \
"{{width=100 height=50 border=1}}\n"                                     \
"http://www.google.com/images/logo.gif\n"                                \
"Some text after\n"                                                      \
" {{width=100 height=50 border=1 right}}                              \n"\
"{{width=100 height=50 border=1 right}} http://www.google.com/images/logo.gif\n"\
"Picture is on the right\n\nMore text\n\n"                                 \
" {{width=100 height=50 border=1 left}}                              \n"\
"{{width=100 height=50 border=1 left}} http://www.google.com/images/logo.gif\n"\
"Picture is on the left\n\nMore text\n\n"                                 \
"{{width=100 height=50 border=1}}\n" \
"Picture http links have picture with an !http:// link inside, like [http://www.google.com/images/logo.gif http://www.google.com]\n" \
" Picture http links have picture with an !http:// link inside, like [http://www.google.com/images/logo.gif http://www.google.com]\n" \
"----\n"                                                                 \
"\n"                                                                     \
" Video youtube, videomotion, veoh, vimeo, flash... embedded:\n"         \
" youtube=http://www.youtube.com/v/---adress--- \n"                       \
" dailymotion=http://www.dailymotion.com/swf/---adress--- \n"             \
" vimeo=http://..... veoh=http://..... flash=http://.....\n"             \
" line starts with youtube... and terminate the adress with a space\n"   \
"----\n"                                                                 \
"\n"                                                                     \
"Unordered lists begin with a '* '. The number of asterisks determines the level:\n" \
"* foo\n"                                                                \
"* bar\n"                                                                \
"** boom\n"                                                              \
"** bam\n"                                                               \
"* baz\n"                                                                \
"\n"                                                                     \
"  * foo\n"                                                              \
"  * bar\n"                                                              \
"  ** boom\n"                                                            \
"  ** bam\n"                                                             \
"  * baz\n"                                                              \
"\n"                                                                     \
"Ordered lists work the same, but use a '#'\n"                           \
"----\n"                                                                 \
"Tables begin the line with a '|' and before every new column after.\n"  \
"\n"                                                                     \
"| row:1,col:1 | row:1,col:2\n"                                          \
"| row:2,col:1 | row:2,col:2\n"                                          \
" | row:1,col:1 | row:1,col:2\n"                                         \
" | row:2,col:1 | row:2,col:2\n"                                         \
"---\n"                                                                  \
"Quotation begins with ' \n"                                             \
"\n"                                                                     \
" 'I had a monumental idea this morning, but I didn't like it\n"         \
"'I had a monumental idea this morning, but I didn't like it\n"          \
"----\n"                                                                 \
" {{expand}} and {{collapse}} use javascript and must be enabled in the user's browser.\n"\
"Expand text\n"                                                          \
"\n"                                                                     \
" {{expand}}  or  {{expand=your label here}}\n"                          \
" text, links, images\n"                                                 \
" {{-expand}}\n"                                                         \
"\n"                                                                     \
"{{expand}}\n"                                                           \
"text, links: http://www.google.com\n"                                  \
"{{-expand}}\n"                                                         \
"\nImbricated\n"                                                        \
" {{expand}}\n"                                                         \
" Some text\n"                                                          \
" {{expand}}\n"                                                         \
" More text\n"                                                           \
" {{-expand}}\n"                                                         \
" {{-expand}}\n"                                                         \
"{{expand}}\n"                                                         \
"Some text\n"                                                          \
"{{expand}}\n"                                                         \
"More text\n"                                                           \
"{{-expand}}\n"                                                         \
"{{-expand}}\n"                                                         \
"----\n"                                                                 \
"Collapse text, can be imbricated too.\n"                                    \
"\n"                                                                     \
" {{collapse}}  or  {{collapse=your label here}}\n"                       \
" text, links, images\n"                                                 \
" {{-collapse}}\n"                                                        \
"\n"                                                                     \
"{{collapse}}\n"                                                          \
"text, links: http://www.google.com\n"                                  \
"{{-collapse}}\n"                                                        \
"----\n" \
"Simple form: ToDo list\n" \
" {{entry}} or {{entry tiny/small/medium/large/huge date}}\n" \
" !{{data}}   (remove ! before this tag)\n" \
" Entries are added in the page after the tag data\n" \
" {{delete}} deletes the selected field\n" \
" {{save}} saves the checked fields\n" \
"{{entry}}\n" \
"{{data}}\n" \
"{{delete}}{{save}}\n" \
"----\n"  \
" Table of contents {{toc}} can be between collapse or expand\n" \
"{{collapse=Table of contents}}{{toc}}{{-collapse}}\n" \
"----\n" \
"<Inline> <html> <is> <escaped>.\n" \
"\n" \
"----\n" \
"Lines prefixed with a space are unformatted ( Like examples on this page )\n" \
"\n" \
"----\n" \
" There is a `keyword` in this line.\n" \
"There is a `keyword` in this line.\n" \
"\n" \
"----\n" \
" Schedule text with {{schedule=date;show this text if date is true;show this text if false}}\n" \
" or {{schedule=date;show it is true}} or {{schedule=date;;show it is false}}\n" \
" date can be the day of the week Monday...Sunday or the month January...December\n" \
" or the day of he month 1,2,...31 or the month + the day of he month, ex. July 04\n" \
" Example: {{schedule=Monday;+It's Monday Deal with it.+}}\n" \
"{{schedule=Monday;+It's Monday. Deal with it.+}}\n" \
" {{schedule=d/m/n}} set the date of the page to perform a test\n" \
" d=0...6 (Sunday...Saturday) m=1...12 (January...December) n=1...31 \n" \
"\n" \
"----\n" \
" To upload file or image into the server with {{upload}}\n" \
"Upload your file below then go to Index [Index?Folder=images images] \n" \
"or [Index?Folder=files files] to check your file uploaded:\n\n" \
"(Note:size allowed can be set to 0 or limited.)\n" \
"{{upload}}\n" \
"\n" \
"----\n" \
" Define a private zone in the page with {{private}}\n" \
" Login is required to show after this tag\n" \
"{{private}}\n" \
" This text is hidded if you are not logged!\n" \
"\n" \
"----\n" 


#define STYLESHEET                                                       \
                                                                         \
"body {\n" \
"  font-family: Verdana, Arial, Helvetica, sans-serif;\n" \
"  font-size: 90%;  \n" \
"  color: #333333;\n" \
"  margin: 2px;\n" \
"  padding: 2px;\n" \
"}\n" \
"\n" \
"#header {\n" \
"    font-size: 90%;\n" \
"    background-color: #eef;\n" \
"    border: 1px solid #aaa;\n" \
"    font-family: Verdana, Arial, Helvetica, sans-serif;\n" \
"    padding: 5px;\n" \
"    margin-bottom:20px;\n" \
"}\n" \
"#header input {\n" \
"    margin:0px;\n" \
"    padding: 0;\n" \
"    background-color: White;\n" \
"    border: 1px solid #aaaaaa;\n" \
"    font-size: 90%;\n" \
"}\n" \
"\n" \
"#footer {\n" \
"    font-size: 10px;\n" \
"    color: #ddd;\n" \
"    margin-top:40px;\n" \
"    width: 100%;\n" \
"    text-align: center;\n" \
"}\n" \
"table.wikitable { background-color: #fff; border: 1px solid #aaa; }\n" \
"table.wikitable td { background-color: #fff; border: 1px solid #aaa; }\n" \
"\n" \
"pre {\n" \
"    font-family: monospace;\n" \
"    background-color: #eee;\n" \
"    padding: 2px;\n" \
"    padding-left: 10px;\n" \
"    margin-left: 20px;\n" \
"    margin-right: 20px;\n" \
"}\n" \
"\n" \
"hr {\n" \
" height: 1px;\n" \
" color: #aaaaaa;\n" \
" background-color: #aaaaaa;\n" \
" border: 0;\n" \
" margin: 0.2em 5px 0.2em 5px;\n" \
"}\n" \
"\n" \
"form {\n" \
"    border: none;\n" \
"    margin: 0;\n" \
"}\n" \
"\n" \
"textarea {\n" \
"    border: 1px solid #aaaaaa;\n" \
"    color: Black;\n" \
"    background-color: white;\n" \
"    width: 100%;\n" \
"    padding: 0.1em;\n" \
"    overflow: auto;\n" \
"}\n" \
"\n" \
"input {\n" \
"    margin-top:1px;\n" \
"    padding: 0 0.4em !important;\n" \
"    background-color: White;\n" \
"    border: 1px solid #aaaaaa;\n" \
"}\n" \
"\n" \
"a, ulink{\n" \
"   color: #333;\n" \
"   text-decoration:none;\n" \
"   border-bottom: 1px #333 dotted;\n" \
"   display: inline;\n" \
" }\n" \
"\n" \
"a:hover {\n" \
"   color: #333;\n" \
"   text-decoration: None;\n" \
"   border-bottom: 1px #000 solid;\n" \
"   }\n" \
"\n" \
"a:visited {\n" \
"   color: #333;\n" \
"}\n" \
"\n" \
"#wrapper {\n" \
"text-align:left;\n" \
"margin:0 auto;\n" \
"display: inline-block;\n" \
"min-height:10px;\n" \
"border:1px solid #ccc;\n" \
"padding:5px;\n" \
"}\n" \
"\n" \
"#myvar {\n" \
"border:1px solid #ccc;\n" \
"background:#f2f2f2;\n" \
"padding:5px;\n" \
"}\n" \
"\n" 


/* use xxd -i favicon.png to generate 
 * png favicon is ok with Firefox >=1.0, Chrome, Opera >=7.0, Safari >=4.0
 * png favicon is ok with IE >= 11 */
#define FAVICONDATAMAX 1024 //enough memory to load favicon.ico file (wiki_init() in wiki.c)
unsigned char FaviconData[FAVICONDATAMAX+1] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
  0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7c, 0x08, 0x64,
  0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0b,
  0x13, 0x00, 0x00, 0x0b, 0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18, 0x00, 0x00,
  0x00, 0x1d, 0x74, 0x45, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e,
  0x74, 0x00, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x77, 0x69,
  0x74, 0x68, 0x20, 0x54, 0x68, 0x65, 0x20, 0x47, 0x49, 0x4d, 0x50, 0xef,
  0x64, 0x25, 0x6e, 0x00, 0x00, 0x00, 0x70, 0x49, 0x44, 0x41, 0x54, 0x38,
  0x8d, 0xc5, 0x52, 0xc1, 0x11, 0x80, 0x30, 0x08, 0x33, 0x9c, 0x13, 0x74,
  0xff, 0x19, 0xbb, 0x02, 0xbe, 0xea, 0x21, 0x26, 0xa0, 0xe7, 0xc3, 0x7c,
  0x5a, 0x52, 0x9a, 0xd2, 0x00, 0xa6, 0xbb, 0x6f, 0x1f, 0xb0, 0x33, 0x72,
  0x00, 0xe7, 0x7e, 0x36, 0xfa, 0x37, 0x81, 0x01, 0xd0, 0x4b, 0x8a, 0x37,
  0xf5, 0x72, 0x86, 0xaa, 0xc4, 0x28, 0x4b, 0xa0, 0xc4, 0x2f, 0x02, 0xdd,
  0x7f, 0x19, 0xc0, 0xba, 0x90, 0x4d, 0xac, 0x4c, 0xa5, 0x02, 0x6f, 0xf0,
  0xd8, 0x83, 0x52, 0x20, 0x1b, 0xb4, 0xe2, 0x6e, 0x6d, 0x2b, 0x50, 0xa6,
  0x46, 0xbe, 0x9c, 0x83, 0x18, 0xc7, 0x41, 0xa2, 0x15, 0xac, 0x04, 0xd5,
  0x6f, 0x75, 0x66, 0x31, 0xa1, 0x2a, 0x5b, 0xe1, 0xff, 0x36, 0x1e, 0x5a,
  0xb4, 0x3a, 0x45, 0x1e, 0x2f, 0xe4, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x49,
  0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};
unsigned int FaviconDataLen = 247;

#endif
