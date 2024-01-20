#include "Game.h"
#include <iostream>
#include <math.h>

#define W_H 1280
#define W_W 720

Game::Game(const std::string &config)
{
    init(config);
}

void Game::init(const std::string &path)
{
    // TODO read in config file here

    m_window.create(sf::VideoMode(W_H, W_W), "Ass2");
    m_window.setFramerateLimit(60);
    if (!m_font.loadFromFile("../fonts/tech.ttf"))
    {
        std::cout << "Cannot load" << std::endl;
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(16);
    m_text.setFillColor(sf::Color(sf::Color::White));

    spawneEnemy();
    spawnPlayer();
}

void Game::run()
{
    // TODO: add pause

    while (m_running)
    {
        m_entities.update();
        sMovement();
        sCollision();
        sUserInput();
        sRender();
        sLifespan();

        m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{
    // TODO
}

void Game::spawnPlayer()
{
    // TODO FINISH adding all properties;

    auto entity = m_entities.addEntity("player");
    entity->cTransform = std::make_shared<CTransform>(Vec2(W_H / 2, W_W / 2), Vec2(1.0f, 1.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(16.0f, 36, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->cInput = std::make_shared<CInput>();
    entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);
    entity->cScore = std::make_shared<CScore>(0);

    m_player = entity;
}

void Game::spawneEnemy()
{
    float w_x = 200;
    float w_y = 300;
    auto enemy = m_entities.addEntity("enemy");
    enemy->cTransform = std::make_shared<CTransform>(Vec2(w_x, w_y), Vec2(2.0f, 2.0f), 0.0f);
    enemy->cShape = std::make_shared<CShape>(144, 6, sf::Color(255, 44, 12), sf::Color(255, 255, 0), 3);
    enemy->cShape->circle.setPosition(enemy->cTransform->pos.x, enemy->cTransform->pos.y);

    w_x = 900;
    w_y = 250;
    auto enemy1 = m_entities.addEntity("enemy");
    enemy1->cTransform = std::make_shared<CTransform>(Vec2(w_x, w_y), Vec2(2.0f, 2.0f), 0.0f);
    enemy1->cShape = std::make_shared<CShape>(100, 4, sf::Color(255, 44, 12), sf::Color(255, 255, 0), 3);
    enemy1->cShape->circle.setPosition(enemy1->cTransform->pos.x, enemy1->cTransform->pos.y);

    w_x = 250;
    w_y = 600;
    auto enemy2 = m_entities.addEntity("enemy");
    enemy2->cTransform = std::make_shared<CTransform>(Vec2(w_x, w_y), Vec2(2.0f, 2.0f), 0.0f);
    enemy2->cShape = std::make_shared<CShape>(124, 8, sf::Color(255, 44, 12), sf::Color(255, 255, 0), 3);
    enemy2->cShape->circle.setPosition(enemy2->cTransform->pos.x, enemy2->cTransform->pos.y);

    w_x = 800;
    w_y = 550;
    auto enemy3 = m_entities.addEntity("enemy");
    enemy3->cTransform = std::make_shared<CTransform>(Vec2(w_x, w_y), Vec2(2.0f, 2.0f), 0.0f);
    enemy3->cShape = std::make_shared<CShape>(144, 4, sf::Color(255, 44, 12), sf::Color(255, 255, 0), 3);
    enemy3->cShape->circle.setPosition(enemy3->cTransform->pos.x, enemy3->cTransform->pos.y);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)

{

    // TODO spawn small enemies at the location of the input enemy e
    // spawn niumber of small enemies equal to vertices of the dead
    // set eac small enemy to the same color as the original half the size
}
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos)
{

    // Vec2 destination(mousePos.x - m_player->cShape->circle.getPosition().x, mousePos.y - m_player->cShape->circle.getPosition().y);
    // float length = sqrtf(destination.x * destination.x + destination.y * destination.y);
    // Vec2 normal(destination.x / length, destination.y / length);
    // auto bullet = m_entities.addEntity("bullet");
    // bullet->cTransform = std::make_shared<CTransform>(Vec2(mousePos), Vec2(8 * normal.x, 8 * normal.y), 0.0f);
    // bullet->cLifespan = std::make_shared<CLifespan>(250);
    // bullet->cShape = std::make_shared<CShape>(18.0f, 32, sf::Color(255, 255, 255, 255), sf::Color(0, 255, 0), 1.0f);
    // bullet->cShape->circle.setPosition(entity->cShape->circle.getPosition().x, entity->cShape->circle.getPosition().y);
}

void Game::sLifespan()
{
    for (auto &e : m_entities.getEntities("bullet"))
    {
        if (e->cLifespan->remaining <= 0)
        {
            e->destroy();
        }
        if (e->cLifespan->total / 2 >= e->cLifespan->remaining)
        {
            sf::Color currentC = e->cShape->circle.getFillColor();
            sf::Color curretOutC = e->cShape->circle.getOutlineColor();
            e->cShape->circle.setFillColor(sf::Color(currentC.r, currentC.g, currentC.b, currentC.a - 2));
            e->cShape->circle.setOutlineColor(sf::Color(curretOutC.r, curretOutC.g, curretOutC.b, curretOutC.a - 2));
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime > 40)
    {
        spawneEnemy();
    }
}

void Game::sCollision()
{
    for (auto &b : m_entities.getEntities("bullet"))
    {
        for (auto &e : m_entities.getEntities("enemy"))
        {
            Vec2 delta(abs(b->cShape->circle.getPosition().x - e->cShape->circle.getPosition().x), abs(b->cShape->circle.getPosition().y - e->cShape->circle.getPosition().y));
            int ox = (b->cShape->circle.getLocalBounds().width / 2 + e->cShape->circle.getLocalBounds().width / 2) - delta.x;
            int oy = (b->cShape->circle.getLocalBounds().height / 2 + e->cShape->circle.getLocalBounds().height / 2) - delta.y;
            if (ox > 0 && oy > 0)
            {
                m_player->cScore->score += 15;
                //  e->destroy();
                // b->destroy();
            }
        }
    }

    for (auto &e : m_entities.getEntities("enemy"))
    {
        Vec2 m_delta(abs(m_player->cShape->circle.getPosition().x - e->cShape->circle.getPosition().x), abs(m_player->cShape->circle.getPosition().y - e->cShape->circle.getPosition().y));
        int m_ox = (m_player->cShape->circle.getLocalBounds().width / 2 + e->cShape->circle.getLocalBounds().width / 2) - m_delta.x;
        int m_oy = (m_player->cShape->circle.getLocalBounds().height / 2 + e->cShape->circle.getLocalBounds().height / 2) - m_delta.y;

        if (m_ox > 0 && m_oy > 0)
        {
            //  e->destroy();
            // m_player->cShape->circle.setPosition(W_H / 2, W_W / 2);
        }
    }
}

void Game::sRender()
{
    m_window.clear();
    for (auto &rectangle : m_entities.getEntities("enemy"))
    {
        sf::Vector2f playerCenter = m_player->cShape->circle.getPosition();
        for (size_t i = 0; i < rectangle->cShape->circle.getPointCount(); ++i)
        {
            sf::Vector2f circleVertex = rectangle->cShape->circle.getTransform().transformPoint(rectangle->cShape->circle.getPoint(i));

            // Extend the line by 1 pixel on each side
            sf::Vector2f extendedLineStart = circleVertex - (circleVertex - playerCenter) * 1.0f;
            sf::Vector2f extendedLineEnd = circleVertex + (circleVertex - playerCenter) * 1.0f;

            // Draw the extended line
            sf::VertexArray extendedLine(sf::Lines, 2);
            extendedLine[0] = sf::Vertex(extendedLineStart, sf::Color::White);
            extendedLine[1] = sf::Vertex(extendedLineEnd, sf::Color::White);
            m_window.draw(extendedLine);

            // Draw the triangle fan
            sf::VertexArray triangleFan(sf::TriangleFan, 3);
            triangleFan[0] = sf::Vertex(playerCenter, sf::Color::Green);
            triangleFan[1] = sf::Vertex(extendedLineStart, sf::Color::Blue);

            // Calculate the next vertex index (looping back to the start)
            std::size_t nextIndex = (i + 1) % rectangle->cShape->circle.getPointCount();
            sf::Vector2f nextVertex = rectangle->cShape->circle.getTransform().transformPoint(rectangle->cShape->circle.getPoint(nextIndex));

            // Extend the next line by 1 pixel on each side
            sf::Vector2f extendedNextLineStart = nextVertex - (nextVertex - playerCenter) * 1.0f;
            triangleFan[2] = sf::Vertex(extendedNextLineStart, sf::Color::Red);

            m_window.draw(triangleFan);
        }
        // e->cShape->circle.setRotation(e->cTransform->angle
        m_text.setString("Score: " + std::to_string(m_player->cScore->score));
    }

    for (auto &circle : m_entities.getEntities("enemy"))
    {

        m_window.draw(circle->cShape->circle);
    }

    for (auto &e : m_entities.getEntities("player"))
    {
        e->cTransform->angle += 3.0f;
        // e->cShape->circle.setRotation(e->cTransform->angle);
        std::cout << m_player->cScore->score << std::endl;
        m_text.setString("Score: " + std::to_string(m_player->cScore->score));
        m_window.draw(m_text);

        m_window.draw(e->cShape->circle);
    }
    m_window.display();
}

void Game::sMovement()
{
    if (m_player->cInput->up)
    {

        m_player->cShape->circle.move(0, -3);
    }

    if (m_player->cInput->down)
    {

        m_player->cShape->circle.move(0, 3);
    }

    if (m_player->cInput->left)
    {

        m_player->cShape->circle.move(-3, 0);
    }

    if (m_player->cInput->right)
    {
        m_player->cShape->circle.move(3, 0);
    }
    // for (auto &e : m_entities.getEntities("bullet"))
    // {
    //     sf::Vector2f shapePos = e->cShape->circle.getPosition();

    //     e->cShape->circle.move(e->cTransform->velocity.x, e->cTransform->velocity.y);
    //     e->cLifespan->remaining -= 1;
    // }

    // for (auto &e : m_entities.getEntities("enemy"))
    // {
    //     if (e->cShape->circle.getGlobalBounds().left <= 0.0 || e->cShape->circle.getGlobalBounds().left + e->cShape->circle.getGlobalBounds().width >= W_H)
    //     {
    //         e->cTransform->velocity.x *= -1.0f;
    //     }
    //     if (e->cShape->circle.getGlobalBounds().top <= 0.0 || e->cShape->circle.getGlobalBounds().top + e->cShape->circle.getGlobalBounds().height >= W_W)
    //     {
    //         e->cTransform->velocity.y *= -1.0f;
    //     }
    //     e->cShape->circle.move(e->cTransform->velocity.x, e->cTransform->velocity.y);
    // }
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}

void Game::sUserInput()
{

    sf::Event event;

    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = true;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = true;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = true;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = true;
                break;
            case sf::Keyboard::Escape:
                m_running = false;
                break;

            default:
                break;
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = false;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = false;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = false;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = false;
                break;

            default:
                break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
            }
        }
    }
}