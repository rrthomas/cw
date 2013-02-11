match("bright", "[[:digit:]]")
match("punctuation", "\\[")
match("punctuation", "\\]")
match("neutral", "\\(")
match("neutral", "\\)")
match("neutral", "\\+")
match("bright", "-")
match("highlight", ":")
match("error", "root")
if not arg["--help"] then
  match("bright", "[^ ]+")
end
