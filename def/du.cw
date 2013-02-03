base cyan
ifnarg --help
match cyan+:default /
match default:default M
match default:default K
match green+:default [^\t]+
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
