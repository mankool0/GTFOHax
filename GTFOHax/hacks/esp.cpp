#include "esp.h"
#include <algorithm>
#include <atomic>

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

    static std::atomic_bool g_worldRescanRequested{true};

    template <typename T, typename TValue>
    static bool ContainsPtr(const std::vector<T>& items, TValue T::* member, TValue ptr)
    {
        return std::ranges::any_of(items, [member, ptr](const T& item) {
            return item.*member == ptr;
        });
    }

    static void AddTerminal(app::LG_ComputerTerminal* terminal)
    {
        if (!terminal || !app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(terminal), nullptr))
            return;

        std::lock_guard<std::mutex> lock(G::worldTerminalsMtx);
        if (!ContainsPtr(worldTerminals, &WorldTerminalItem::terminalItem, terminal))
            worldTerminals.push_back(WorldTerminalItem(terminal));
    }

    static void AddCarryItem(app::CarryItemPickup_Core* carryItem)
    {
        if (!carryItem || !app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(carryItem), nullptr))
            return;
        if (!carryItem->fields.m_sync)
            return;

        std::lock_guard<std::mutex> lock(G::worldCarryMtx);
        if (!ContainsPtr(worldCarryItems, &WorldCarryItem::carryItem, carryItem))
            worldCarryItems.push_back(WorldCarryItem(carryItem));
    }

    static bool TerminalCacheEmpty()
    {
        std::lock_guard<std::mutex> lock(G::worldTerminalsMtx);
        return worldTerminals.empty();
    }

    static bool CarryItemCacheEmpty()
    {
        std::lock_guard<std::mutex> lock(G::worldCarryMtx);
        return worldCarryItems.empty();
    }

    static void RescanTerminalsFromManager()
    {
        if (!app::LG_ComputerTerminalManager__TypeInfo || !*app::LG_ComputerTerminalManager__TypeInfo)
            return;

        auto manager = (*app::LG_ComputerTerminalManager__TypeInfo)->static_fields->Current;
        if (!manager || !manager->fields.m_terminals || !manager->fields.m_terminals->fields.entries)
            return;

        auto entries = manager->fields.m_terminals->fields.entries;
        const int count = (std::min)(manager->fields.m_terminals->fields.count, static_cast<int>(entries->max_length));
        for (int i = 0; i < count; ++i)
        {
            const auto& entry = entries->vector[i];
            if (entry.hashCode < 0 || !entry.value)
                continue;
            AddTerminal(entry.value);
        }
    }

    static void RescanCourseNodeMetadata(bool needsTerminals, bool needsCarryItems)
    {
        if (!app::StaticUpdateManager__TypeInfo || !*app::StaticUpdateManager__TypeInfo)
            return;

        auto courseNodesList = (*app::StaticUpdateManager__TypeInfo)->static_fields->courseNodes;
        if (!courseNodesList || !courseNodesList->fields._items)
            return;

        const int nodeCount = (std::min)(courseNodesList->fields._size, static_cast<int>(courseNodesList->fields._items->max_length));
        for (int i = 0; i < nodeCount; ++i)
        {
            auto courseNode = courseNodesList->fields._items->vector[i];
            if (!courseNode || !courseNode->fields.MetaData)
                continue;

            auto terminals = courseNode->fields.MetaData->fields.ComputerTerminals;
            if (needsTerminals && terminals && terminals->fields._items)
            {
                const int terminalCount = (std::min)(terminals->fields._size, static_cast<int>(terminals->fields._items->max_length));
                for (int j = 0; j < terminalCount; ++j)
                    AddTerminal(terminals->fields._items->vector[j]);
            }

            auto carryItems = courseNode->fields.MetaData->fields.MissionPickupItems;
            if (needsCarryItems && carryItems && carryItems->fields._items)
            {
                const int carryCount = (std::min)(carryItems->fields._size, static_cast<int>(carryItems->fields._items->max_length));
                for (int j = 0; j < carryCount; ++j)
                    AddCarryItem(carryItems->fields._items->vector[j]);
            }
        }
    }

    void RequestWorldRescan()
    {
        g_worldRescanRequested.store(true, std::memory_order_relaxed);
    }

    void RescanWorldIfNeeded()
    {
        if (!g_worldRescanRequested.exchange(false, std::memory_order_relaxed))
            return;
        if (app::GameStateManager_get_CurrentStateName(nullptr) != app::eGameStateName__Enum::InLevel)
        {
            RequestWorldRescan();
            return;
        }
        const bool needsTerminals = TerminalCacheEmpty();
        const bool needsCarryItems = CarryItemCacheEmpty();
        if (!needsTerminals && !needsCarryItems)
            return;

        if (needsTerminals)
            RescanTerminalsFromManager();
        RescanCourseNodeMetadata(needsTerminals, needsCarryItems);
    }

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
