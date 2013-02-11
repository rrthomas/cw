if not arg["-c"] and not arg["-m"] and not arg["-l"] and not arg["-L"] and not arg["-w"] and not arg["--bytes"] and not arg["--chars"] and not arg["--lines"] and not arg["--max-line-length"] and not arg["--words"] then
  match("bright", "[[:digit:]]")
  match("bright", "/")
  match("bright", "\\.")
  match("bright", "-")
  match("bright", "<")
  match("bright", ">")
  match("bright", "\\[[^]]*\\]")
end
