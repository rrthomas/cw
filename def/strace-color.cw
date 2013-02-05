$NOCOLOR=1
command strace {} 2>&1
base cyan
match cyan+:cyan+ \(.+\).*[ ]=[ ]
match cyan+:none \+\+\+
match cyan+:none ---
