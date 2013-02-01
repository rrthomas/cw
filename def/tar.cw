path /bin:/usr/bin:/sbin:/usr/sbin:<env>
# coloring the output stream of tar would create a broken archive.
ifnarg -c:--create
base cyan
match white:cyan :
match white:default =
match cyan+:default /
match cyan+:default -
match cyan+:default \.
match cyan+:default `
match cyan+:default '
match white:default \[
match white:default \]
match white:default <
match white:default >
