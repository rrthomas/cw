$NOCOLOR=1
command strace {} 2>&1
match bright:bright \(.+\).*[ ]=[ ]
match bright:none \+\+\+
match bright:none ---
