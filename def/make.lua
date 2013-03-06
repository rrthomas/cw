-- don't want to color kernel dialog compiling.
if not arg["menuconfig"] then
  match("highlight", "%*%*%*")
  match("neutral", "checking")
  match("highlight", "warning: ")
  match("error", "error: ")
  match("error", "Error")
  match("warning", "Interrupt")
end
