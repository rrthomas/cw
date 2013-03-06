if not arg["-h"] and not arg["--help"] then
  match("bright", " %d+ ")
  match("bright", "%b()")
  match("bright", "%b[]")
  match("bright", "Module")
  match("bright", "Size")
  match("bright", "Used by")
end
