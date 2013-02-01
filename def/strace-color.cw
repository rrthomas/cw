$NOCOLOR_NEXT=1
other strace {} 2>&1
base cyan
match cyan+:cyan+ \(.+\).*[ ]=[ ]
match cyan+:none \+\+\+
match cyan+:none ---
