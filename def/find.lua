-- for -exec's, if another color wrapped program is called it will not
-- be colored. (lets this definition file take over)
$NOCOLOR=1
match("bright", "/")
match("bright", "\\[")
match("bright", "\\]")
match("bright", "\\(")
match("bright", "\\)")
match("punctuation", ":")
match("punctuation", ";")
match("punctuation", "\\.")
