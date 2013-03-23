if not arg["--help"] then
  match("lowlight", "%b[]")
  match("lowlight", "%b()")
  match("error", "root")
  match("highlight", "^[^ ]+")
end
