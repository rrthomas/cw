base cyan
match cyan+:default [[:digit:]]
match cyan+:default /
match cyan+:default :
match cyan+:default \.
match cyan+:default -
match cyan+:default #
ifnarg --help:--version
match cyan+:default [^ ]+
