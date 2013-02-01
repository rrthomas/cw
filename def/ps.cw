path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default [[:digit:]]
match cyan+:default \.
match cyan+:default -
match cyan+:default \*
match white:default :
match white:default /
match white:cyan+ \[
match white:default \]
match white:cyan+ (
match white:default )
match white:cyan+ {
match white:default }
match white:default \?
match white:default tty
match white:default root
