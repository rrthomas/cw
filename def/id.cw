# shell scripts use this a lot, will cause problems being colored.
ifnarg -g:-G:-n:-r:-u:--group:--groups:--name:--real:--user
match bright:default :
match bright:default =
match bright:default /
match bright:bright \[
match bright:default \]
match bright:bright (
match bright:default )
ifarg --help
match bright:default -
