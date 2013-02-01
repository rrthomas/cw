path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default [[:digit:]]
match grey:default \[
match grey:default \]
match white:default (
match white:default )
match white:default \+
match cyan+:default -
match green+:default :
match red+:default root
ifnarg --help
match cyan+:default [^ ]+
