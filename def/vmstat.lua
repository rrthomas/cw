if arg["-m"] then
  match("lowlight", "[_%-]")
  match("lowlight", "Cache")
  match("lowlight", "Num")
  match("lowlight", "Total")
  match("lowlight", "Size")
  match("lowlight", "Pages")
  match("lowlight", " .*")
else
  match("lowlight", "[%-%d]")
  match("lowlight", "%b[]")
end
match("highlight", "%b()")
