# this is a bit heavy on the regex, may be slow on old-old systems.
path /sbin:/usr/sbin:/bin:/usr/bin:<env>
base cyan
ifarg --help:<none>
match cyan+:default <.+>
match cyan+:default --[^ ]+
ifarg -s
match cyan+:default [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8}
ifarg -R
match cyan+:default ^[0-9A-Fa-f]{8,16}[ ]
match cyan+:default [ ](_{1,2})[A-Za-z0-9_]+$
ifnarg -s:-R
match cyan+:default ^[ ]*[A-Za-z0-9\/]+:
ifnarg --help:<none>:-s:-R
match cyan+:default 0x[0-9A-Fa-f]{1,16}
match cyan+:default [\<\>\(\)\*\+\$\%\,\.]
