match("neutral", "%x+:%x+:%x+:%x+:%x+:%x+")
if posix.uname("%s") == "SunOS" then
  match("bright", "Device")
  match("bright", "-----")
else
  match("bright", "Address")
  match("bright", "%-")
end
match("bright", "%.")
match("bright", "%[")
match("bright", "%]")
match("neutral", "<")
match("neutral", ">")
match("highlight", "%(")
match("highlight", "%)")
