require "posix"
match("neutral", "[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}")
if posix.uname("%s") == "SunOS" then
  match("bright", "Device")
  match("bright", "-----")
else
  match("bright", "Address")
  match("bright", "-")
end
match("bright", "\\.")
match("bright", "\\[")
match("bright", "\\]")
match("neutral", "<")
match("neutral", ">")
match("highlight", "\\(")
match("highlight", "\\)")
