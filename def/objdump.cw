# this is a bit heavy on the regex, may be slow on old-old systems.
ifarg --help:<none>
match bright <.+>
match bright --[^ ]+
ifarg -s
match bright [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8}
ifarg -R
match bright ^[0-9A-Fa-f]{8,16}[ ]
match bright [ ](_{1,2})[A-Za-z0-9_]+$
ifnarg -s:-R
match bright ^[ ]*[A-Za-z0-9\/]+:
ifnarg --help:<none>:-s:-R
match bright 0x[0-9A-Fa-f]{1,16}
match bright [\<\>\(\)\*\+\$\%\,\.]
