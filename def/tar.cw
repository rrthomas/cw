# coloring the output stream of tar would create a broken archive.
ifnarg -c:--create
match neutral:punctuation :
match neutral:default =
match bright:default /
match bright:default -
match bright:default \.
match bright:default `
match bright:default '
match neutral:default \[
match neutral:default \]
match neutral:default <
match neutral:default >
