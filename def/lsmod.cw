path /sbin:/usr/sbin:/bin:/usr/bin:<env>
base cyan
digit cyan+:default
match cyan+:cyan+ (
match cyan+:default )
match cyan+:default \[
match cyan+:default \]
match cyan+:none Module
ifarg -h:--help
match cyan+:default -
