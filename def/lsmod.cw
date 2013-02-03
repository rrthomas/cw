base cyan
match cyan+:default [[:digit:]]
match cyan+:cyan+ (
match cyan+:default )
match cyan+:default \[
match cyan+:default \]
match cyan+:none Module
ifarg -h:--help
match cyan+:default -
