path /bin:/usr/bin:/sbin:/usr/sbin:<env>
ifarg -m
base green+
match green:default _
match green:default -
match white:none Cache
match white:none Num
match white:none Total
match white:none Size
match white:none Pages
match cyan:none  .*
ifarg-else
base cyan+
match cyan:default [[:digit:]]
match white:default -
match white:default \[
match white:default \]
ifarg <any>
match white:cyan+ (
match white:default )
