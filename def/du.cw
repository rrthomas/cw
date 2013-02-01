path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan
ifnarg --help
match cyan+:default /
match default:default M
match default:default K
token green+:default 0 9
ifarg-else
match green+:default \[
match green+:default \]
match green+:default <
match green+:default >
match green+:default :
match cyan+:default ,
match cyan+:default =
match cyan+:default -
ifarg <any>
match default:none Try
