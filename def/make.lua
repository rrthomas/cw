-- Don't color kernel compilation dialogs.
if not arg["menuconfig"] then
  match("highlight", "%*%*%*")
  match("lowlight", "checking")
  match("highlight", "warning: ")
  match("error", "error: ")
  match("error", "Error")
  match("warning", "Interrupt")
end
