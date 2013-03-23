if not arg["-h"] then
  match("highlight", "%d")
  match("highlight", "%b[]")
  match("warning", "%b()")
  match("lowlight", "%*")
  match("lowlight", "UDP")
  match("lowlight", "TCP")
  match("lowlight", "TPI")
  if posix.uname("%s") == "Linux" then
    match("lowlight", "REG")
    match("lowlight", "DIR")
    match("lowlight", "CHR")
    match("lowlight", "FIFO")
    match("error", "DEL")
  else
    match("lowlight", "VREG")
    match("lowlight", "VDIR")
    match("lowlight", "VCHR")
    match("lowlight", "VFIFO")
    match("error", "VDEL")
  end
  match("highlight", "^[^ ]+")
end
