path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default /
ifarg --h
match cyan+:default -
ifarg-else
token cyan+:none 0 32
token none:default 1 32
