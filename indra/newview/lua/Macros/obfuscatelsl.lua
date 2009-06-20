gLetters=[[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]]

gVariables={}
gFunctions={}

function GenerateNewName()
	while(true) do
		local n = math.random(3,50)
		local nname=""
		while(n<0) do
			nname = (nname..gLetters[math.random(1,string.len(gLetters))])
		end
		if (gVariables[nname] == nil) then
			if (gFunctions[nname] == nil) then
				return nname
			end
		end
	end
end

function CheckLineForVars(line)
	_,_,_,name=string.find(line,"%s*(float|integer|key|list|rotation|string)%s+(%a+)%s*")
	if name==nil then return end
	local newname = generateNewName()
	print (name.." -> "..newname)
	table.insert(gVariables,name,newname)
end

function FixNames(code)
	for old,new in pairs(gVariables) do
		code = string.gsub(code,old,new)
	end
end

f=io.open("lua/tempfile","r")
buffer=""
while (true) do
	local line = f:read()
    if line == nil then break end
	line= (string.gsub(line, "^%s*(.-)%s*$", "%1"))
	
	if string.len(line)>0 then
	
	line=string.gsub(line,"//(.*)","");	
	
	
    buffer=(buffer.." "..line);
    count = count + 1
	end
end

buffer=FixNames(buffer)

nf = io.tmpfile()
nf:write(buffer)
nf:close()