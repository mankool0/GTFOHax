#include "esp.h"

namespace ESP
{
    std::map<std::string, std::string> espItemsReverse;
    std::map<std::string, WorldESPItem*> espItemsMap;

    KeyBindToggle worldESPToggleKey;

    AgentESP enemyESP;

    std::vector<WorldPickupItem> worldItems;
    std::vector<WorldArtifactItem> worldArtifacts;
    std::vector<WorldCarryItem> worldCarryItems;
    std::vector<WorldKeyItem> worldKeys;
    std::vector<WorldGenericItem> worldGenerics;
    std::vector<WorldResourceItem> worldResourcePacks;
    std::vector<WorldHSUItem> worldHSUItems;

    std::vector<WorldBulkheadDC> worldBulkheadDCs;
    std::vector<WorldTerminalItem> worldTerminals;

    void Init()
    {
        for (auto it = espItems.begin(); it != espItems.end(); ++it)
        {
            WorldESPItem* temp = new WorldESPItem();
            espItemsMap.insert(std::pair<std::string, WorldESPItem*>((*it).first, temp));

            espItemsReverse.insert(std::pair<std::string, std::string>((*it).second, (*it).first));
        }
    }

    void UpdateInput()
    {
        worldESPToggleKey.handleToggle();
        enemyESP.toggleKey.handleToggle();
    }
}
