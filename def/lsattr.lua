match("neutral", ":")
match("neutral", ";")
match("highlight", "%.")
match("bright", "%-")
match("bright", "/")
match("highlight", "%[")
match("highlight", "%]")
match("highlight", "%(")
match("highlight", "%)")
if not arg["--help"] then
  match("highlight", "[^ ]+")
end
