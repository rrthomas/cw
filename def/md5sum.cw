path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default /
ifarg --h
match cyan+:default -
ifarg-else
match cyan+:default [^ ]+
