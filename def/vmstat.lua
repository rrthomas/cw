if arg["-m"] then
  match("lowlight", "_")
  match("lowlight", "%-")
  match("lowlight", "Cache")
  match("lowlight", "Num")
  match("lowlight", "Total")
  match("lowlight", "Size")
  match("lowlight", "Pages")
  match("punctuation", " .*")
else
  match("punctuation", "%d")
  match("lowlight", "%-")
  match("lowlight", "%[")
  match("lowlight", "%]")
end
match("highlight", "%b()")
