path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
match cyan+:default [[:digit:]]
match cyan+:default [[:upper:]]
match white:default :
match white:default \.
match white:default (
match white:default )
match white:default -
match white:default `
match white:default '
match cyan+:default /
match cyan+:default %
match cyan+:default  %{?}
