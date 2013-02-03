ifarg -h:--help
base cyan
match cyan+:default >
match cyan+:default <
match cyan+:green+ \[
match cyan+:default \]
match cyan+:default -
ifarg-else
base white
match cyan+:default >
match green+:default <
match yellow:default |
match cyan+:default \+
match yellow:default ---
match green+:default -
match yellow:none @@
