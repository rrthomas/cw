$NOCOLOR=1
command strace {} 2>&1
match bright \(.+\).*[ ]=[ ]
match bright \+\+\+
match bright ---
