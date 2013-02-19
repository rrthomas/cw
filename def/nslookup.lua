-- don't color any potential interactive mode situations.
if #arg > 0 then
  local opt = false
  for _, v in ipairs(arg) do
    if v:sub(1, 1) == "%-" then
      opt = true
    end
  end
  if not opt then
    match("bright", ":")
    match("bright", "`")
    match("bright", "'")
  end
end
