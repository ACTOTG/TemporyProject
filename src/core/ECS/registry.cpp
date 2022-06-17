//
// Created by Amo on 2022/6/17.
//

#include "core/ECS/registry.h"
#include "core.h"

namespace SymoCraft
{
    namespace ECS
    {
        typedef uint32 EntityIndex;
        typedef uint32 EntityType;
        typedef uint64 EntityId;
        typedef uint32 ComponentIndex;

        // ----------------------------------------------------------------------
        // Registry Implement

        void Registry::Free()
        {
            for (Internal::ComponentContainer& container : component_set)
                container.Free();
        }

        void Registry::Clear()
        {
            entities.clear();
            Registry::Free();
            component_set.clear();
            component_set = std::vector<Internal::ComponentContainer>();
            debug_component_names.clear();
            free_entities.clear();
        }

        EntityId Registry::CreateEntity()
        {
            if (!free_entities.empty())
            {
                EntityIndex new_index = free_entities.back();
                free_entities.pop_back();
                EntityId new_id = Internal::CreateEntityId(new_index
                        , Internal::GetEntityVersion(entities[new_index]));
                entities[new_index] = new_id;
                return new_id;
            }
            entities.emplace_back(Internal::CreateEntityId((uint32)entities.size(), 0));
            return entities.back();
        }

        int Registry::NumComponents(EntityId entity) const
        {
            int num_components = 0;
            for (int component_id = 0; component_id < component_set.size(); component_id++)
                if (HasComponentByType(entity, component_id))
                    num_components++;

            return num_components;
        }

        bool Registry::IsEntityValid(EntityId entity) const
        {
            return Internal::GetEntityIndex(entity) < entities.size() && Internal::GetEntityIndex(entity) != UINT32_MAX;
        }

        void Registry::DestroyEntity(EntityId entity)
        {
            RemoveAllComponent(entity);
            EntityId new_id = Internal::CreateEntityId(UINT32_MAX, Internal::GetEntityVersion(entity) + 1);
            entities[Internal::GetEntityIndex(entity)] = new_id;
            free_entities.push_back(Internal::GetEntityIndex(entity));
        }




        bool Registry::HasComponentByType(EntityId entity, int32 component_type) const
        {
            if (!IsEntityValid(entity))
                return false;

            if (component_type >= component_set.size() || component_type < 0)
            {
                AmoLogger_Warning("Tried to check if an entity had component '%d'"
                                ", but a component of type '%d' does not exist in the registry "
                                "which only has '%d' components."
                                , component_type, component_type, component_set.size());
                return false;
            }

            return component_set[component_type].IsComponentExist(entity);
        }


        uint8* Registry::GetComponentByType(EntityId entity, int32 component_type) const
        {
            if (!IsEntityValid(entity))
            {
                AmoLogger_Error("Cannot check if invalid entity %d has a component."
                                , entity);
                return nullptr;
            }

            if (component_type >= component_set.size() || component_type < 0)
            {
                AmoLogger_Warning("Tried to check if an entity had component '%d', "
                                  "but a component of type '%d' does not exist in the registry."
                                  , component_type, component_type);
                return nullptr;
            }

            return component_set[component_type].Get(Internal::GetEntityIndex(entity));
        }


        void Registry::RemoveAllComponent(EntityId entity)
        {
            if (!IsEntityValid(entity))
            {
                AmoLogger_Error("Tried to remove invalid entity %d 's component."
                                , entity);
                return;
            }

            EntityIndex entity_index = Internal::GetEntityIndex(entity);
            if (entity_index >= entities.size())
            {
                AmoLogger_Error("Tried to remove all components from invalid entity '%d'"
                , entity);
                return;
            }

            for (int i = 0; i < component_set.size(); i++)
                if (component_set[i].IsComponentExist(entity))
                    component_set[i].Remove(entity);
        }
    }
}