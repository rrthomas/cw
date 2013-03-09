if not arg["--help"] then
  if arg["-"] then
    match("highlight", " %+ ")
    match("lowlight", " - ")
  end
  match("punctuation", "%b()")
  match("highlight", "^%S+")
  match("highlight", "tty%S+")
  match("highlight", "pts%S+")
  match("punctuation", "LOGIN")
  match("warning", "root")
end
