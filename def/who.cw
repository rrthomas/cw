path /usr/bin:/bin:/usr/sbin:/sbin:<env>
base grey+
ifarg --help
match white:default \?
match white:default \+
match white:default  - 
ifarg -
match green+:default  \+ 
match green:default  - 
ifarg <any>
match white:default -
match white:default :
match white:default \.
match white:default =
match grey:default /
match grey:default (
match grey:default )
match grey:default <
match grey:default >
match grey:default \[
match grey:default \]
match white:default tty
match white:default pts
match grey:default LOGIN
match white:default root
match cyan+:default ... .. ..:..
