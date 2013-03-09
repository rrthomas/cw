if posix.uname("%s") == "SunOS" then
  match("highlight", "----")
  match("lowlight", "is alive")
else
  match("highlight", "---")
end
match("highlight", "%b()")
match("highlight", "%b[]")
match("lowlight", " from ")
match("lowlight", "From ")
match("lowlight", "PING")
