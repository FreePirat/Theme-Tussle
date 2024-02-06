#include <fstream>
#include <iostream>

#include "Scene_Tussle.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"


Scene_Tussle::Scene_Tussle(GameEngine* gameEngine, const std::string&levelPath)
    : Scene(gameEngine)
      , m_worldView(gameEngine->window().getDefaultView()) {
    loadLevel(levelPath);
    registerActions();

    //MusicPlayer::getInstance().play("gameTheme");

    init();
}


void Scene_Tussle::init() {
    spawnPlayer();
}

void Scene_Tussle::sMovement(sf::Time dt) {
    playerMovement();

    // move all objects
    for (auto e: m_entityManager.getEntities()) {
        if (e->hasComponent<CTransform>()) {
            auto&tfm = e->getComponent<CTransform>();

            tfm.pos += tfm.vel * dt.asSeconds();
            tfm.angle += tfm.angVel * dt.asSeconds();
        }
    }
}


void Scene_Tussle::registerActions() {
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "BACK");
    registerAction(sf::Keyboard::Q, "QUIT");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");

    registerAction(sf::Keyboard::I, "LIGHT");
    registerAction(sf::Keyboard::O, "MEDIUM");
    registerAction(sf::Keyboard::P, "HEAVY");
}


void Scene_Tussle::onEnd() {
    m_game->changeScene("MENU", nullptr, false);
}

void Scene_Tussle::playerMovement() {
    // no movement if player is dead
    //if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead")
        //return;

    auto&dir = m_player->getComponent<CInput>().dir;
    auto&pos = m_player->getComponent<CTransform>().pos;
    auto& vel = m_player->getComponent<CTransform>().vel;

    if (dir & CInput::UP && m_canJump) {
        vel.y = -sqrtf(2.0 * 981.0 * 200.f);
        m_canJump = false;
    }
    
    if (dir & CInput::DOWN) {
        //CROUCH STATE
    }

    if (dir & CInput::LEFT) {
        pos.x -= 20.f;
    }

    if (dir & CInput::RIGHT) {
        pos.x += 20.f;
    }
}


void Scene_Tussle::sRender() {
    m_game->window().setView(m_worldView);

    sf::Text lives("Player 1 Health: " + std::to_string(m_player1HP), Assets::getInstance().getFont("main"));
    lives.setPosition(5, 20);
    int currentTime = m_time.asSeconds();
    sf::Text time("Time: " + std::to_string(currentTime), Assets::getInstance().getFont("main"));
    time.setPosition(350, 20);

    // draw bkg first
    for (auto e: m_entityManager.getEntities("bkg")) {
        if (e->getComponent<CSprite>().has) {
            auto&sprite = e->getComponent<CSprite>().sprite;
            m_game->window().draw(sprite);
        }
        m_game->window().draw(lives);
        m_game->window().draw(time);
    }


    for (auto&e: m_entityManager.getEntities()) {
        if (!e->hasComponent<CAnimation>())
            continue;

        // Draw Sprite
        auto&anim = e->getComponent<CAnimation>().animation;
        auto&tfm = e->getComponent<CTransform>();
        anim.getSprite().setPosition(tfm.pos);
        anim.getSprite().setRotation(tfm.angle);
        m_game->window().draw(anim.getSprite());

        if (m_drawAABB) {
            if (e->hasComponent<CBoundingBox>()) {
                auto box = e->getComponent<CBoundingBox>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f{box.size.x, box.size.y});
                centerOrigin(rect);
                rect.setPosition(e->getComponent<CTransform>().pos);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color{0, 255, 0});
                rect.setOutlineThickness(2.f);
                m_game->window().draw(rect);
            }
        }
    }
}


void Scene_Tussle::update(sf::Time dt) {
    sUpdate(dt);
}

void Scene_Tussle::sDoAction(const Command&action) {

    if (action.type() == "START") {
        if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
        else if (action.name() == "QUIT") { m_game->quitLevel(); }
        else if (action.name() == "BACK") { m_game->backLevel(); }

        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

        if (action.name() == "LEFT") {
            m_player->getComponent<CInput>().dir = CInput::LEFT; 
            m_player->addComponent<CState>("block");
            m_idleTime = sf::seconds(10); 
        }
        else if (action.name() == "RIGHT") {
            m_player->getComponent<CInput>().dir = CInput::RIGHT;
            m_player->addComponent<CState>("forward");
            m_idleTime = sf::seconds(10); 
        }
        else if (action.name() == "UP") {
            m_player->getComponent<CInput>().dir = CInput::UP;
            m_idleTime = sf::seconds(10); 
        }
        else if (action.name() == "DOWN") {
            m_player->getComponent<CInput>().dir = CInput::DOWN;
            m_player->addComponent<CState>("crouch");
            m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("Crouch"));
            m_idleTime = sf::seconds(10);
        }

        else if (action.name() == "LIGHT") {
            m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("5L"));
            m_idleTime = sf::seconds(10);
        }
        else if (action.name() == "MEDIUM") {
            m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("5M"));
            m_idleTime = sf::seconds(10);
        }
        else if (action.name() == "HEAVY") {
            m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("5H"));
            m_idleTime = sf::seconds(10); 
        }
    }
    else if (action.type() == "END" && (action.name() == "LEFT" || action.name() == "RIGHT" || action.name() == "UP" ||
                                        action.name() == "DOWN")) {
        m_player->getComponent<CInput>().dir = 0;
    }
}


void Scene_Tussle::spawnPlayer() {

    auto pos = m_worldView.getSize();

    pos.x = pos.x / 5;
    pos.y = pos.y / 1.5;

    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(pos);
    m_player->addComponent<CBoundingBox>(sf::Vector2f(180.f, 300.f));
    m_player->addComponent<CInput>();
    m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("Default"));

    //SET ORIGIN TO MAKE IT CENTERED

    
}


sf::FloatRect Scene_Tussle::getViewBounds() {
    return sf::FloatRect();
}

void Scene_Tussle::sCollisions() {
    adjustPlayerPosition();
}


void Scene_Tussle::sUpdate(sf::Time dt) {
    SoundPlayer::getInstance().removeStoppedSounds();
    m_entityManager.update();
    
    auto playerPos = m_player->getComponent<CTransform>().pos;
    auto vel = m_player->getComponent<CTransform>().vel;

    float deltaTime = dt.asSeconds();
    float currentVel = 981.0f * deltaTime;
    vel.y = currentVel * deltaTime;
    //MAKE GRAVITY WORK
    
    if (m_isPaused)
        return;

    int idleTimer = m_idleTime.asSeconds();
    m_time -= dt;
    if (idleTimer != 0) {
        m_idleTime -= dt;
    }
    else {
        m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("NinaIdle"));
        m_idleTime = sf::seconds(10);
    }

    int timer = m_time.asSeconds();
    if (timer == 0) {
        //FINISH THE GAME
    }
    if (m_player1HP != m_player1HPCheck) {
        m_time = sf::seconds(120);
        m_player1HPCheck -= 1;
    }

    sAnimation(dt);
    sMovement(dt);
    sCollisions();
}


void Scene_Tussle::sAnimation(sf::Time dt) {

    for (auto e: m_entityManager.getEntities()) {
    
        if (e->hasComponent<CAnimation>()) {
            auto&anim = e->getComponent<CAnimation>();
            anim.animation.update(dt);

            for (auto dead : m_entityManager.getEntities("dead")) {
                if (dead->getComponent<CAnimation>().animation.hasEnded()) {
                    dead->destroy();
                }
            }

            if (anim.animation.hasEnded()) {
                if (m_player->getComponent<CState>().state != "crouch") {
                    m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("Default"));
                }
                else {
                    m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("CrouchDefault"));
                }
            }
        }
    }
}


void Scene_Tussle::adjustPlayerPosition() {

    auto center = m_worldView.getCenter();
    sf::Vector2f viewHalfSize = m_worldView.getSize() / 2.f;


    auto left = center.x - viewHalfSize.x;
    auto right = center.x + viewHalfSize.x;
    auto top = center.y - viewHalfSize.y;
    auto bot = center.y + viewHalfSize.y;

    auto&player_pos = m_player->getComponent<CTransform>().pos;
    auto halfSize = sf::Vector2f{20, 20};
    // keep player in bounds
    player_pos.x = std::max(player_pos.x, left + halfSize.x);
    player_pos.x = std::min(player_pos.x, right - halfSize.x);
    player_pos.y = std::max(player_pos.y, top + halfSize.y);
    player_pos.y = std::min(player_pos.y, bot - halfSize.y);
}

void Scene_Tussle::checkPlayerState() {

}

void Scene_Tussle::loadLevel(const std::string&path) {
    std::ifstream config(path);
    if (config.fail()) {
        std::cerr << "Open file " << path << " failed\n";
        config.close();
        exit(1);
    }

    std::string token{""};
    config >> token;
    while (!config.eof()) {
        if (token == "Bkg") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("bkg");

            // for background, no textureRect its just the whole texture
            // and no center origin, position by top left corner
            // stationary so no CTransfrom required.
            auto&sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }
        else if (token[0] == '#') {
            std::cout << token;
        }

        config >> token;
    }

    config.close();
}
