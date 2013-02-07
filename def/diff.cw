ifarg -h:--help
match bright:default >
match bright:default <
match bright:highlight \[
match bright:default \]
match bright:default -
ifarg-else
base neutral
match bright:default >
match highlight:default <
match warning:default |
match bright:default \+
match warning:default ---
match highlight:default -
match warning:none @@
