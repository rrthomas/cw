if not arg["-h"] then
  match("highlight", "%d")
  match("highlight", "%b[]")
  match("warning", "%b()")
  match("lowlight", "%*")
  match("punctuation", "UDP")
  match("punctuation", "TCP")
  match("punctuation", "TPI")
  if posix.uname("%s") == "Linux" then
    match("punctuation", "REG")
    match("punctuation", "DIR")
    match("punctuation", "CHR")
    match("punctuation", "FIFO")
    match("error", "DEL")
  else
    match("punctuation", "VREG")
    match("punctuation", "VDIR")
    match("punctuation", "VCHR")
    match("punctuation", "VFIFO")
    match("error", "VDEL")
  end
  match("default", "COMMAND")
  match("highlight", "^[^ ]+")
end
