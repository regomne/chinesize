#!/usr/bin/env lua5.3
local inspect = require('inspect')

local function join_str(ls, sep)
  local s = ''
  for i,v in ipairs(ls) do
    if i == 1 then
      s = v
    else
      s = s .. sep .. v
    end
  end
  return s
end

local function parse_dialog_text_tag(tag)
  local t = tag[1]
  if t == 'ruby' then
    return '[ruby text="' .. tag.text .. '"]'
  elseif t == 'rt2' then
    return '\\n'
  elseif t:sub(1,1) == '/' then
    return '[' .. t .. ']'
  else
    print('unknown tag:', t)
    return '[unk:' .. t .. ']'
  end
end

local function get_dialog_text(jatext)
  local s = ''
  for i,v in ipairs(jatext) do
    if type(v) == 'string' then
      s = s .. v
    elseif type(v) == 'table' then
      s = s .. parse_dialog_text_tag(v)
    end
  end
  return s
end

local function insert_select_text(ls, sel)
  for i,v in ipairs(sel) do
    table.insert(ls, v)
  end
end

local function exttext(ast)
  local ls = {}
  for i,v in ipairs(ast.text) do
    local ja = v.ja
    if ja.name ~= nil then
      if ja.name.text ~= nil then
        table.insert(ls, ja.name.text)
      else
        table.insert(ls, ja.name.name)
      end
    end
    if #ja > 1 or (#ja == 0 and v.select == nil) then
      print('error in ja field:', inspect(v))
    end
    if #ja == 1 then
      table.insert(ls, get_dialog_text(ja[1]))
    else
      insert_select_text(ls, v.select)
    end
  end
  return ls
end

local function read_ast_file(fname)
 local ast_func, err = loadfile(fname, 't')
  if ast_func == nil then
    print("err when loading", fname, ":", err)
    return nil
  end
  ast = nil
  ast_func()
  if ast == nil then
    print(fname, "'s format may be wrong: there is not 'ast' variable")
    return nil
  end
  return ast
end

local function extast(ast_file_name, txt_file_name)
  local ast = read_ast_file(arg[1])
  if ast == nil then
    return
  end
  
  local ls = exttext(ast)
  if ls ~= nil then
    local fs = io.open(txt_file_name, 'wb')
    if fs == nil then
      print("can't open file:", txt_file_name)
      return
    end
    fs:write(join_str(ls, "\r\n"))
    fs:close()
  end
end

if arg[1] == nil or arg[2] == nil then
  print('usage: extast.lua <astfile> <output_txt>')
  return
end
extast(arg[1], arg[2])
