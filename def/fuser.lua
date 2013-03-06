if not arg["-i"] and not arg["--interactive"] and not arg["-k"] and not arg["--kill"] then
  match("neutral", "%-")
  match("neutral", "/")
  match("punctuation", "%.")
  match("bright", ":")
  match("bright", "%|")
  match("punctuation", "%[")
  match("punctuation", "%]")
  match("neutral", "%(")
  match("neutral", "%)")
  if #arg > 0 then
    match("punctuation", "%d")
    match("neutral", "USER")
  end
end
