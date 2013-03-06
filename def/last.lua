if not arg["--help"] then
  match("punctuation", "%b[]")
  match("neutral", "%b()")
  match("error", "root")
  match("bright", "^[^ ]+")
end
