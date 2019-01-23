local pcall, dofile, _G = pcall, dofile, _G

module "luci.version"

if pcall(dofile, "/etc/openwrt_release") and _G.DISTRIB_DESCRIPTION then
	distname    = ""
	distversion = _G.DISTRIB_DESCRIPTION
else
	distname    = "OpenWrt"
	distversion = "Development Snapshot"
end

luciname    = "LuCI QA170623_Kylin_2017-08-21_SQA_Dailybuild_2in1_NAS-1-gdc12a34 Release"
luciversion = "git-17.233.32017-dc12a34"
