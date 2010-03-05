--[[
	Luna Package Manager (LPM)
		By N3X15

	Mostly version checking/Deptracking in here.
]]--
Packages={}
LPM={}

--dofile "download.lua"
--dofile "config.lua"

function lpm(command,argument)
	if command=="install" then
		local pkg = RemotePackages[argument]
		if(pkg==nil) then 
			error( "Package "..argument.." doesn't exist.")
			return
		end
		LPM.CheckForDependancies(remote_pkg)
		UpdateQueue[pkgname]="I"
	elseif command=="update" then
		LPM.DownloadManifests()
	end
end

function LPM.DownloadManifests()
	RemotePackages={}
	for i,v in pairs(LPM.Repositories) do
		print("Checking",v)
		local m = "manifests/"..v..".txt";
		LPM.Download(i.."MANIFEST.txt",m)
		LPM.ParseManifest(m);
	end
	RemotePackages=LPM._man
	LPM._man={}
end

function LPM.ParseLocalManifests()
	Packages={}
	local dirlist = dirtree(".")
	if(dirlist==nil) then
		error ("Dirlist = null (.)")
		return
	end
	for ent in dirlist do
		-- packdef
		local ext=string.sub(ent,-7)
		--print(ext)
		if ext=="packdef" then
			LPM.ParseManifest(ent)
		end
	end
	Packages=LPM._man
	LPM._man={}
end

function DumpManifest(manifest)
	print(" Dumping "..#manifest.." items...")
	for name,package in pairs(manifest) do
		local deps = "DEPS: "
		for _,d in pairs(package.Depends) do
			deps=deps..", "..d
		end
		print(" * "..name,package.Version)
	end
end

LPM._man={}
function LPM.ParseManifest(file)
	local cp=nil
	local cd={}
	local prov={}
	for line in io.lines(file) do
		-- Variable: Value
		local d,args = string.match(line,"(%w+):%s+(.*)")
		if not(d == nil) then
			if d=="Package" then
				if not cp == nil then
					LPM._man[cp].Provides=prov
					LPM._man[cp].Depends=cd
				end
				cp=args
				LPM._man[cp]={}
				LPM._man[cp].Name=cp;
				cd={}
				prov={}
			elseif d=="Depends" then
				cd[#cd+1]=args
			elseif d=="Provides" then
				prov[#prov+1]=args
			else
				LPM._man[cp][d]=args
			end
		end
	end
	if not cp == nil then
		LPM._man[cp].Provides=prov
		LPM._man[cp].Depends=cd
	end
end

function LPM.FindTokens(str)
  return str:match( ("([^ ]*) "):rep(3) )
end

function LPM.CheckForDependancies(package)
	if LPM.Verbose==true then 
		print("Checking "..package.Name.." "..package.Version.."'s dependancies.")
	end
	for _,packdef in pairs(package["Dependancies"]) do
		if not CheckDep(packdef) then return false end
	end
	return true
end

function LPM.CheckDep(packdef)
	-- package operator version
	-- Luna    >=       1.23.5.217
	local tokens = LPM.FindTokens(packdef)

	local pkgname = tokens[1]
	local operator = tokens[2]
	local version = tokens[3]
	
	local our_pkg = Packages[pkgname]
	-- Do we have an existing installation?
	if our_pkg == nil then
		-- No, prepare to download new package.
		local remote_pkg = LPM.RemotePackages[pkgname]
		-- Does the remote server have the package?
		if remote_pkg == nil then
			-- Well shit.
			error(pkgname.." doesn't exist on the remote repository, nor on our own installation!")
			return false
		end
		LPM.CheckForDependancies(remote_pkg)
		UpdateQueue[pkgname]="I"
		return true
	end

	-- Grab the remote package so we can check the version
	local remote_pkg = LPM.RemotePackages[pkgname]
	if remote_pkg == nil then
		-- ... Wut
		error(pkgname.." doesn't exist on the remote repository!")
		return false
	end

	-- Check the version (Oh god I hated writing that function)
	local verdiff = vercmp(our_pkg.Version,remote_pkg.Version)

	-- Is it older?  Is it REQUIRED to be older?
	if verdiff==-1 and (operator=="<" or operator=="<=") then
		return true
	-- Is it newer/Required to be newer?
	elseif verdiff==1 and (operator==">" or operator==">=") then
		return true
	-- Are they both the fucking same package, and is that allowed?
	elseif verdiff==0 and (operator=="<=" or operator==">=" or operator=="==") then
		return true
	end
	-- Update required.
	LPM.CheckForDependancies(remote_pkg)
	UpdateQueue[pkgname]="U"
	return true
end

--[[
	By N3X15

	> =vercmp("1.0-r3","1.0-r4")
	-1
	> =vercmp("1.0-r4","1.0-r3")
	1
	> =vercmp("1.0-r5","1.0-r5")
	0
]]--
function vercmp(ver1, ver2)
	
	local v_reg="(%w+)"
	
	if ver1 == ver2 then 
		return 0 
	end
	local mykey=ver1..":"..ver2
--	for i,v in string.gmatch(ver1,LPM.ver_regexp) do
	local match1={}
	local match2={};
	for a in string.gmatch(ver1, v_reg) do
		if not string.find(a,"svn|cvs")==nil then 
			a="VCS"
		end
		if string.sub(a,1,1)=="r" then
			a = tonumber(string.sub(a,2,-1))/10000 -- r1000 < 1000
		end
		match1[#match1+1]=a
   	end
	for a in string.gmatch(ver2, v_reg) do
		if not string.find(a,"svn|cvs")==nil then 
			a="VCS"
		end
		if string.sub(a,1,1)=="r" then
			a = tonumber(string.sub(a,2,-1))/10000 -- r1000 < 1000
		end
		match2[#match2+1]=a
   	end

	-- shortcut for VCS packages
	if match1[1]=="VCS" and not match2[1]=="VCS" then
		return 1
	elseif match2[1]=="VCS" and not match1[1]=="VCS" then
		return -1
	end
	
	local i=1
	while i < math.max(#match1,#match2) do
		local a = tonumber(match1[i])
		local b = tonumber(match2[i])

		if not (a==nil or b==nil) then
			if a > b then
				return 1 
			elseif a < b then 
				return -1 
			end
		end
		i=i+1
	end
	return 0
end

