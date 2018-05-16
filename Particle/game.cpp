/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"

// Game-related State data
ParticleGenerator *Particles;
BallObject        *Ball;
SpriteRenderer    *Renderer;


Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height) 
{ 

}

Game::~Game()
{
    delete Particles;
    delete Ball;
}

void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // Load textures
    ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");
    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));    
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

    glm::vec2 ballPos = {0.0f, 0.0f};
    Ball = new BallObject(ballPos, 1.0f, {1.0f, 1.0f}, ResourceManager::GetTexture("face"));
}

void Game::Update(GLfloat dt)
{
   // Update particles
    Particles->Update(dt, *Ball, 2);
}

void Game::ProcessInput(GLfloat dt)
{

}

void Game::Render()
{
 if (this->State == GAME_ACTIVE)
    {
        // Draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("sprite"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
        // Draw particles	
        Particles->Draw();
        // Draw ball
        Ball->Draw(*Renderer);
    }   
}