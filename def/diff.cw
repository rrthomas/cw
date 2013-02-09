ifarg -h:--help
match bright >
match bright <
match highlight \[[^]]*\]
match bright -
ifarg-else
match bright >
match highlight <
match warning |
match bright \+
match warning ---
match highlight -
match warning @@
