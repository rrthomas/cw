path /usr/bin:/bin:/usr/sbin:/sbin:<env>
base cyan
match cyan+:default [[:digit:]]
match cyan+:default (
match cyan+:default )
match cyan+:default \[
match cyan+:default \]
match cyan+:default {
match cyan+:default }
match cyan+:default :
match cyan+:default \.
match cyan+:default ,
match cyan+:default -
match cyan+:default =
match cyan+:default /
match default:default USER
ifnarg --help
token cyan+:default 0 32
