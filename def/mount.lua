require "posix"
match("bright", "\\([^)]*\\)")
if posix.uname("%s") == "SunOS" then
  match("bright", " on")
end
