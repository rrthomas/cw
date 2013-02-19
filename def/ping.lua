if posix.uname("%s") == "SunOS" then
  match("bright", "----")
  match("neutral", "is alive")
else
  match("bright", "---")
end
if not arg["--help"] then
  match("highlight", "=")
else
  match("neutral", ":")
  match("highlight", "%b()")
  match("highlight", "%b[]")
  match("bright", ",")
  match("highlight", " ms")
  match("default", " from ")
  match("default", "From ")
  match("punctuation", "PING")
end
