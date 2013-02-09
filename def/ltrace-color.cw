$NOCOLOR=1
command ltrace {} 2>&1
match bright \+\+\+
match bright ---
