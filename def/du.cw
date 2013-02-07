ifnarg --help
match bright:default /
match default:default M
match default:default K
match highlight:default [^	]+
ifarg-else
match highlight:default \[
match highlight:default \]
match highlight:default <
match highlight:default >
match highlight:default :
match bright:default ,
match bright:default =
match bright:default -
ifarg <any>
match default:none Try
