path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default [[:digit:]]
match cyan+:default /
match cyan+:default :
match cyan+:default \.
match cyan+:default -
match cyan+:default #
ifnarg --help:--version
token cyan+:default 0 32
