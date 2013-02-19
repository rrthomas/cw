match("bright", "%d")
match("bright", "/")
match("bright", ":")
match("bright", "%.")
match("bright", "%-")
match("bright", "#")
if not arg["--help"] and not arg["--version"] then
  match("bright", "[^ ]+")
end
