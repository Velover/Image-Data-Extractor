local StringUtilities = require("Core/StringUtilities");
local Zlib = require("Core/Zlib")

local config = {
	level = 5; --[[integer 0 -> 9 where 0 is no compression and 9 is most compression]]
	strategy = "fixed" --[["huffman_only", "fixed", "dynamic"]]
}

function Compress(_string)
	local compressed_string = Zlib.Deflate.Compress(_string, config);
	local readable_string = StringUtilities.ToBase64(compressed_string)

	return readable_string;
end