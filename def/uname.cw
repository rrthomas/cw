path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
digit cyan+:default
match cyan+:default /
match cyan+:default :
match cyan+:default \.
match cyan+:default -
match cyan+:default #
ifnarg --help:--version
token cyan+:default 0 32
