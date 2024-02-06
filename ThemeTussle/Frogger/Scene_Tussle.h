#ifndef BREAKOUT_SCENE_BREAKOUT_H
#define BREAKOUT_SCENE_BREAKOUT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"



class Scene_Tussle : public Scene {
private:
    sPtrEntt        m_player{nullptr};
    sf::View        m_worldView;
    sf::FloatRect   m_worldBounds;

    bool			m_drawTextures{true};
    bool			m_drawAABB{false};
    bool			m_drawGrid{false};
    bool            m_canJump{ true };
    int             m_player1HP{ 400 };
    int             m_player1HPCheck{ 400 };
    sf::Time        m_time = sf::seconds(120);
    sf::Time        m_idleTime = sf::seconds(10);



    //systems
    void            sMovement(sf::Time dt);
    void            sCollisions();
    void            sUpdate(sf::Time dt);
    void            sAnimation(sf::Time dt);

    void	        onEnd() override;


    // helper functions
    void            playerMovement();
    void            adjustPlayerPosition();
    void            checkPlayerState();
    void	        registerActions();
    void            spawnPlayer();

    void            init();
    void            loadLevel(const std::string &path);
    sf::FloatRect   getViewBounds();

public:

    Scene_Tussle(GameEngine *gameEngine, const std::string &levelPath);
    void		  update(sf::Time dt) override;
    void		  sDoAction(const Command& command) override;
    void		  sRender() override;

};



#endif //BREAKOUT_SCENE_BREAKOUT_H
