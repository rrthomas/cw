if not arg["--help"] then
  match("highlight", " %+ ")
  match("lowlight", " - ")
  match("lowlight", "%b()")
  match("highlight", "^%S+")
  match("highlight", "tty%S+")
  match("highlight", "pts%S+")
  match("lowlight", "LOGIN")
  match("warning", "root")
end
