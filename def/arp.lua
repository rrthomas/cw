match("lowlight", "%x+:%x+:%x+:%x+:%x+:%x+")
if posix.uname("%s") == "SunOS" then
  match("highlight", "Device")
  match("highlight", "%-%-%-%-%-")
else
  match("highlight", "Address")
  match("highlight", "%-")
end
match("highlight", "%b[]")
match("lowlight", "%b<>")
match("highlight", "%b()")
