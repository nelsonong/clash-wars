package com.clashwars.game;


import com.badlogic.gdx.graphics.OrthographicCamera;

public class GameRenderer {
    // Camera manage view window
    private OrthographicCamera camera;

    // Game World the Renderer renders
    GameWorld world;

    GameRenderer(GameWorld world){
        this.world = world;
    }

    public void render(float delta){

    }

}
