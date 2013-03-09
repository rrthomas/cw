if not arg["-h"] and not arg["--help"] then
  match("highlight", " %d+ ")
  match("highlight", "%b()")
  match("highlight", "%b[]")
  match("highlight", "Module")
  match("highlight", "Size")
  match("highlight", "Used by")
end
