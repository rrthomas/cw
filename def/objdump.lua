-- this is a bit heavy on the regex, may be slow on old-old systems.
if #arg == 0 or arg["--help"] then
  match("bright", "<.+>")
  match("bright", "--[^ ]+")
end
if arg["-s"] then
  match("bright", "[0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8} [0-9A-Fa-f]{8}")
end
if arg["-R"] then
  match("bright", "^[0-9A-Fa-f]{8,16}[ ]")
  match("bright", "[ ](_{1,2})[A-Za-z0-9_]+$")
end
if not arg["-s"] and not arg["-R"] then
  match("bright", "^[ ]*[A-Za-z0-9\\/]+:")
end
if #arg > 0 and not arg["--help"] and not arg["-s"] and not arg["-R"] then
  match("bright", "0x[0-9A-Fa-f]{1,16}")
  match("bright", "[\\<\\>\\(\\)\\*\\+\\$\\%\\,\\.]")
end
