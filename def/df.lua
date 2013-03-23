if not arg["--help"] then
  match("lowlight", "[KMGT] ")
  match("highlight", "[Kk]?-blocks")
  match("highlight", "Mounted on")
  match("highlight", "Filesystem")
  match("highlight", "Used")
  match("highlight", "Available")
  match("highlight", "Use%%")
  match("lowlight", "[%d%%]")
  match("lowlight", "%b<>")
  match("lowlight", "%b()")
  match("lowlight", "%b[]")
  match("lowlight", "`[^']*'")
end
