-- don't want to color kernel dialog compiling.
if not arg["menuconfig"] then
  match("punctuation", "%.")
  match("punctuation", "%-")
  match("punctuation", "/")
  match("punctuation", "\\")
  match("bright", ":")
  match("bright", "'")
  match("bright", "`")
  match("bright", "\"")
  match("highlight", "&")
  match("neutral", "=")
  match("neutral", "%[")
  match("neutral", "%]")
  match("highlight", "%*%*%*")
  match("neutral", "%*")
  match("neutral", "checking")
  match("highlight", "warning: ")
  match("error", "error: ")
  match("error", "Error")
  match("warning", "Interrupt")
end
