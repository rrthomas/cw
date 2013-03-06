if not arg["--help"] then
  if arg["-"] then
    match("highlight", " %+ ")
    match("lowlight", " - ")
  end
  match("punctuation", "%b()")
  match("highlight", "^%S+")
  match("bright", "tty%S+")
  match("bright", "pts%S+")
  match("punctuation", "LOGIN")
  match("warning", "root")
end
