if not arg["-i"] and not arg["--interactive"] and not arg["-k"] and not arg["--kill"] then
  match("lowlight", "%-")
  match("lowlight", "/")
  match("punctuation", "%.")
  match("highlight", ":")
  match("highlight", "%|")
  match("punctuation", "%[")
  match("punctuation", "%]")
  match("lowlight", "%(")
  match("lowlight", "%)")
  if #arg > 0 then
    match("punctuation", "%d")
    match("lowlight", "USER")
  end
end
