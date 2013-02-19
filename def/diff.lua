if arg["-h"] or arg["--help"] then
  match("bright", ">")
  match("bright", "<")
  match("highlight", "%b[]")
  match("bright", "%-")
else
  match("bright", ">")
  match("highlight", "<")
  match("warning", "|")
  match("bright", "%+")
  match("warning", "---")
  match("highlight", "%-")
  match("warning", "@@")
end
