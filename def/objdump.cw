# this is a bit heavy on the regex, may be slow on old-old systems.
ifarg --help:<none>
match bright:default <.+>
match bright:default --[^ ]+
ifarg -s
match bright:default [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8}
ifarg -R
match bright:default ^[0-9A-Fa-f]{8,16}[ ]
match bright:default [ ](_{1,2})[A-Za-z0-9_]+$
ifnarg -s:-R
match bright:default ^[ ]*[A-Za-z0-9\/]+:
ifnarg --help:<none>:-s:-R
match bright:default 0x[0-9A-Fa-f]{1,16}
match bright:default [\<\>\(\)\*\+\$\%\,\.]
