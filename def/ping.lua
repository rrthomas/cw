if posix.uname("%s") == "SunOS" then
  match("bright", "----")
  match("neutral", "is alive")
else
  match("bright", "---")
end
match("bright", "%b()")
match("bright", "%b[]")
match("lowlight", " from ")
match("lowlight", "From ")
match("lowlight", "PING")
