#include "atlas_parser.h"
#include <SDL2/SDL_image.h>
#include <fstream>
#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>

AtlasParser::AtlasParser()
	: m_atlasTexture(nullptr, SDL_DestroyTexture) 
{
}

AtlasParser::~AtlasParser() {
}

bool AtlasParser::LoadAtlas(SDL_Renderer* renderer, const std::string& jsonFilePath, const std::string& imageFilePath) {
	std::ifstream jsonFile(jsonFilePath);
	if(!jsonFile.is_open()) {
		SDL_Log("Failed to open JSON file: %s", jsonFilePath.c_str());
		return false;
	}
	
	std::stringstream jsonStream;
	jsonStream << jsonFile.rdbuf();
	std::string jsonContent = jsonStream.str();
	jsonFile.close();
	
	nlohmann::json jsonData;
	try {
		jsonData = nlohmann::json::parse(jsonContent);
	} catch (const nlohmann::json::parse_error& e) {
		SDL_Log("JSON parse error: %s", e.what());
		return false;
	}

	//SDL_Log("Is JsonData an object? %s", jsonData.is_object() ? "true" : "false");
	//SDL_Log("Is JsonData an array? %s", jsonData.is_array() ? "true" : "false");
	 
	 //trying to get file i/o json to work again 
	// Load the atlas image
	SDL_Surface* surface = IMG_Load(imageFilePath.c_str());
	if (!surface) {
		SDL_Log("Failed to load atlas image: %s", IMG_GetError());
		return false;
	}

	m_atlasTexture.reset(SDL_CreateTextureFromSurface(renderer, surface));
	SDL_FreeSurface(surface);
	
	if (!m_atlasTexture) {
		SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
		return false;
	}
	
	bool hasSlicesKey = jsonData.contains("slices");
	bool isSlicesArray = false; // Initialize to false
	if (hasSlicesKey) { // Only check if it's an array if the key exists
		isSlicesArray = jsonData["slices"].is_array();
	}
	
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AtlasParser: DEBUG Check: jsonData.contains(\"slices\") = %s", hasSlicesKey ? "true" : "false");
SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AtlasParser: DEBUG Check: jsonData[\"slices\"].is_array() = %s (only valid if contains 'slices' is true)", isSlicesArray ? "true" : "false");
SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AtlasParser: DEBUG Check: Full IF condition will be %s", (hasSlicesKey && isSlicesArray) ? "true" : "false");

	//SDL_Log("AtlasParser: DEBUG Check: jsonData[\"slices\"] = %s", jsonData["slices"].dump().c_str());
	SDL_Log("AtlasParser: Debug Check: jsonData dump = %s", jsonData.dump().c_str());

	// Parse the JSON data
	if (jsonData.contains("meta") && jsonData["meta"].contains("slices") && jsonData["meta"]["slices"].is_array()) {
    for (const auto& slice : jsonData["meta"]["slices"]) {
        sliceInfo info;
        info.filename = slice["name"].get<std::string>();
        if (slice.contains("keys") && slice["keys"].is_array() && !slice["keys"].empty()) {
            const auto& bounds = slice["keys"][0]["bounds"];
            info.sourceRect = {
                bounds["x"].get<int>(),
                bounds["y"].get<int>(),
                bounds["w"].get<int>(),
                bounds["h"].get<int>()
            };

            if (info.sourceRect.h != 17) {
                info.baseYOffset = 17 - info.sourceRect.h;
            } else {
                info.baseYOffset = 0;
            }

            m_spriteDefinitions[info.filename] = info;
        } else {
            SDL_Log("Slice '%s' does not contain valid bounds.", info.filename.c_str());
            return false;
        }
    }
} else {
    SDL_Log("No slices found in JSON.");
    m_atlasTexture.reset(nullptr);
    return false;
}


	return true; // successfully loaded the atlas
}

const sliceInfo* AtlasParser::GetSliceInfo(const std::string& filename) const {
	auto it = m_spriteDefinitions.find(filename);
	if(it != m_spriteDefinitions.end()) {
		return &it->second;
	}
	SDL_Log("Slice info not found for filename: %s", filename.c_str());
	return nullptr;
}