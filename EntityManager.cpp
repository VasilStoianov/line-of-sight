#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{
}

void EntityManager::update()
{

    for (auto &e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }

    removeDeadEntities(m_entities);
    for (auto &[tag, entitiVec] : m_entityMap)
    {
        removeDeadEntities(entitiVec);
    }
    m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{

    vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity> e)
                             { return !e -> isActive(); }),vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    // this is incorect return the correct vector from the map;
    return m_entityMap[tag];
}