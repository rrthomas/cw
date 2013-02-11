require "posix"
match("bright", "[[:digit:]]")
match("warning", "/")
match("bright", "-")
match("bright", "\\+")
match("highlight", "\\[")
match("highlight", "\\]")
match("warning", "\\([^)]*\\)")
match("neutral", "\\*")
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
if not arg["-h"] and not arg["--help"] then
  match("highlight", "[^ ]+")
end
