path /bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin:<env>
ifnarg --interactive
base cyan
match cyan+:default [[:digit:]]
match green+:default :
match white:default \.
match white:default /
match white:default \[
match white:default \]
match white:default (
match white:default )
match yellow:default \*
match cyan+:default -
match cyan+:default ,
match cyan+:default nmap
match green+:default tcp
match yellow:default udp
match green+:default open
match yellow:default filtered
match red+:default closed
match white:none Port
