#ifndef ATLAS_PARSER_H //if not defined then process the code until the end of the file
#define ATLAS_PARSER_H // define the macro to prevent multiple inclusions

#include <SDL2/SDL.h>
#include <string>
#include <map> //for std::map which is a sorted associative container used to store elements in key-value pairs
#include <memory> //for std::unique_ptr which are smart pointers

#include "../src/include/json.hpp"

struct sliceInfo {
    std::string filename;
    SDL_Rect sourceRect;
    int baseYOffset = 0;
};

class AtlasParser {
public:
    AtlasParser();
    ~AtlasParser(); // destructor to clean up resources

    bool LoadAtlas(SDL_Renderer* renderer, const std::string& jsonFilePath, const std::string& imageFilePath);

    const sliceInfo* GetSliceInfo(const std::string& filename) const;

    SDL_Texture* GetAtlasTexture() const { return m_atlasTexture.get(); }
private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> m_atlasTexture;
    std::map<std::string, sliceInfo> m_spriteDefinitions;
};

#endif // ATLAS_PARSER_H