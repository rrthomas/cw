if arg["-s"] then
  match("bright", "%x+ %x+ %x+ %x+ %x+")
end
if arg["-R"] then
  match("bright", "^%x+[ ]")
  match("bright", "[ ](_+)[%w_]+$")
end
if not arg["-s"] and not arg["-R"] then
  match("bright", "^[ ]*[%w/]+:")
end
if #arg > 0 and not arg["--help"] and not arg["-s"] and not arg["-R"] then
  match("bright", "0x%x+")
  match("bright", "[<>%(%)%*%+%$%%,%.]")
end
