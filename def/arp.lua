match("neutral", "%x+:%x+:%x+:%x+:%x+:%x+")
if posix.uname("%s") == "SunOS" then
  match("bright", "Device")
  match("bright", "%-%-%-%-%-")
else
  match("bright", "Address")
  match("bright", "%-")
end
match("bright", "%b[]")
match("neutral", "%b<>")
match("highlight", "%b()")
