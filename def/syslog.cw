command tail -f /var/log/syslog
match bright \([^)]*\)
match bright \[[^]]*\]
match highlight ^[[:alpha:]]+ +[[:digit:]]+ +[[:digit:]:]+ [^ ]+
match neutral ^[[:alpha:]]+ +[[:digit:]]+ +[[:digit:]:]+
