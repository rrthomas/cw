if arg["-s"] then
  match("highlight", "%x+ %x+ %x+ %x+ %x+")
end
if arg["-R"] then
  match("highlight", "^%x+[ ]")
  match("highlight", "[ ](_+)[%w_]+$")
end
if not arg["-s"] and not arg["-R"] then
  match("highlight", "^[ ]*[%w/]+:")
end
if #arg > 0 and not arg["--help"] and not arg["-s"] and not arg["-R"] then
  match("highlight", "0x%x+")
  match("highlight", "[<>%(%)%*%+%$%%,%.]")
end
