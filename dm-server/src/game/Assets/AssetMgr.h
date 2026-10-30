// AssetMgr.h — Управление ассетами: музыка, модели, лор, иконки
#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

namespace dm {

struct MusicTrack {
    uint32_t id;
    std::string nameRu;
    std::string nameEn;
    std::string filePath;
    std::string category;
    uint32_t durationSec;
    bool loopTrack;
    float volume;
};

struct ModelDef {
    uint32_t id;
    std::string nameRu;
    std::string nameEn;
    std::string modelPath;
    std::string modelType;
    uint32_t polyCount;
    uint32_t textureSize;
};

struct IconDef {
    uint32_t id;
    std::string name;
    std::string filePath;
    std::string category;
    uint32_t sizePx;
};

struct LoreEntry {
    uint32_t id;
    std::string category;
    uint32_t entityId;
    std::string titleRu;
    std::string titleEn;
    std::string textRu;
    std::string textEn;
    std::string authorRu;
    std::string authorEn;
};

class AssetMgr {
public:
    static AssetMgr &Instance();

    // Музыка
    const MusicTrack *GetMusicTrack(uint32_t id) const;
    std::vector<MusicTrack> GetMusicByCategory(const std::string &category) const;
    bool HasMusicTrack(uint32_t id) const;

    // Модели
    const ModelDef *GetModel(uint32_t id) const;
    std::vector<ModelDef> GetModelsByType(const std::string &type) const;

    // Иконки
    const IconDef *GetIcon(uint32_t id) const;
    const IconDef *GetIconByName(const std::string &name) const;

    // Лор
    const LoreEntry *GetLore(uint32_t id) const;
    std::vector<LoreEntry> GetLoreByCategory(const std::string &category) const;

    // Загрузка данных
    void LoadAllAssets();

    // Конфигурация
    bool IsEnabled() const;
    float GetMusicVolume() const;
    float GetSfxVolume() const;
};

} // namespace dm