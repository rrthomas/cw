if not arg["-i"] and not arg["-ki"] then
  match("neutral", "-")
  match("neutral", "/")
  match("punctuation", "\\.")
  match("bright", ":")
  match("bright", "|")
  match("punctuation", "\\[")
  match("punctuation", "\\]")
  match("neutral", "\\(")
  match("neutral", "\\)")
  if #arg > 0 then
    match("punctuation", "[[:digit:]]")
    match("neutral", "USER")
  end
end
