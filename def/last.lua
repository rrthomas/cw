if not arg["--help"] then
  match("punctuation", "%b[]")
  match("lowlight", "%b()")
  match("error", "root")
  match("highlight", "^[^ ]+")
end
