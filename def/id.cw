# shell scripts use this a lot, will cause problems being colored.
ifnarg -g:-G:-n:-r:-u:--group:--groups:--name:--real:--user
base cyan
match cyan+:default :
match cyan+:default =
match cyan+:default /
match cyan+:cyan+ \[
match cyan+:default \]
match cyan+:cyan+ (
match cyan+:default )
ifarg --help
match cyan+:default -
