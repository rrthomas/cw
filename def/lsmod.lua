match("bright", "[[:digit:]]")
match("bright", "\\([^)]*\\)")
match("bright", "\\[")
match("bright", "\\]")
match("bright", "Module")
if arg["-h"] or arg["--help"] then
  match("bright", "-")
end
