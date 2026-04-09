#pragma once
#include <string>
#include <unordered_map>

namespace I18N {
    enum class Language {
        English,
        Chinese
    };

    // Initialize i18n system
    void Initialize();
    
    // Initialize after font loading, set language based on font availability
    void InitializeAfterFontLoad(bool chineseFontAvailable);

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

