SWIGPATH="newview/swig/"

function BuildModule(name)
	if not exists(SWIGPATH..name..".swig") then
		print(" * Not found.")
		return
	end
	local outfile = string.format("newview/_Lua%s",name)
	local cl = "swig -lua -c++ -o "..outfile..".cpp -oh "..outfile..".h "..SWIGPATH..name..".swig | tee "..SWIGPATH..name..".swig.log"
	print(cl)
	os.execute(cl);
end

function exists(filename)
	local file = io.open(filename)
	if file then
		io.close(file)
		return true
	else
		return false
	end
end

i=1
for i=1,#arg,1 do
--	print(i,arg[i])
	BuildModule(arg[i])
end
