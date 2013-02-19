-- coloring the output stream of tar would create a broken archive.
if not arg["-c"] and not arg["--create"] then
  match("punctuation", ":")
  match("neutral", "=")
  match("bright", "/")
  match("bright", "%-")
  match("bright", "%.")
  match("bright", "`")
  match("bright", "'")
  match("neutral", "%[")
  match("neutral", "%]")
  match("neutral", "<")
  match("neutral", ">")
end
