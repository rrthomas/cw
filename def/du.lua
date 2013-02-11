if arg["--help"] then
  match("highlight", "\\[")
  match("highlight", "\\]")
  match("highlight", "<")
  match("highlight", ">")
  match("highlight", ":")
  match("bright", ",")
  match("bright", "=")
  match("bright", "-")
else
  match("bright", "/")
  match("default", "M")
  match("default", "K")
  match("highlight", "[^\\t]+")
end
match("default", "Try")
