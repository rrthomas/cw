if not arg["-G"] and not arg["-U"] then
  match("highlight", "[[:digit:]]")
  match("neutral", "|")
  match("neutral", "---")
  match("bright", "-")
  match("neutral", "`")
  match("neutral", "\\+")
  match("warning", "\\*")
  match("neutral", "\\[")
  match("neutral", "\\]")
  match("neutral", "\\(")
  match("neutral", "\\)")
  match("neutral", "init")
end
