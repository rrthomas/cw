$NOCOLOR=1
command ltrace {} 2>&1
base cyan
match cyan+:cyan+ \+\+\+
match cyan+:cyan+ ---
