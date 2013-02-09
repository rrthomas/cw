# coloring the output stream of tar would create a broken archive.
ifnarg -c:--create
match punctuation :
match neutral =
match bright /
match bright -
match bright \.
match bright `
match bright '
match neutral \[
match neutral \]
match neutral <
match neutral >
