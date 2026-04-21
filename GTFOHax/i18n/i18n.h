#pragma once
#include <string>
#include <unordered_map>

#ifndef ENABLE_CHINESE
#define ENABLE_CHINESE 1
#endif

namespace I18N {
    enum class Language {
        English,
#if ENABLE_CHINESE
        Chinese,
#endif
    };

    // Initialize i18n system
    void Initialize();
    
    // Initialize after font loading, set language based on font availability
    void InitializeAfterFontLoad(bool chineseFontAvailable = false);

    // Get current language
    Language GetCurrentLanguage();

    // Set language
    void SetLanguage(Language lang);

    // Get translation text
    const char* T(const char* key);

    // Language names for UI display
    extern const char* LanguageNames[];
    extern const int LanguageCount;
}

