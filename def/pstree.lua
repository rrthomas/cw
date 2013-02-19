if not arg["-G"] and not arg["-U"] then
  match("highlight", "%d")
  match("neutral", "|")
  match("neutral", "---")
  match("bright", "%-")
  match("neutral", "`")
  match("neutral", "%+")
  match("warning", "%*")
  match("neutral", "%[")
  match("neutral", "%]")
  match("neutral", "%(")
  match("neutral", "%)")
  match("neutral", "init")
end
