if arg["-m"] then
  match("lowlight", "_")
  match("lowlight", "%-")
  match("neutral", "Cache")
  match("neutral", "Num")
  match("neutral", "Total")
  match("neutral", "Size")
  match("neutral", "Pages")
  match("punctuation", " .*")
else
  match("punctuation", "%d")
  match("neutral", "%-")
  match("neutral", "%[")
  match("neutral", "%]")
end
match("bright", "%b()")
