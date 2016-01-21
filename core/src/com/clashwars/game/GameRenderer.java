package com.clashwars.game;


import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.graphics.g3d.Material;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.clashwars.screens.MenuScreen;
import com.clashwars.side.AssetLoader;
import com.clashwars.side.GameInputHandler;

import java.io.Console;
import java.io.InterruptedIOException;

public class GameRenderer {
    // Screen Display Constants
    public static final int WIDTH = (int) (Gdx.graphics.getWidth());
    public static final int HEIGHT = (int)(Gdx.graphics.getHeight());

    // Camera manage view window
    private OrthographicCamera camera;

    // Game World the Renderer renders
    private GameWorld world;

    Game game;
    private SpriteBatch batch;
    private float stateTime;
    private TextureRegion currentStateGoku, currentStateBuu, currentGokuClash, currentBuuClash;
    private int initCounter = -1;
    public int percent = 0;
    int goku_x = (int) (0.025f*WIDTH),
            goku_y = (int) (0.075f*HEIGHT),
            goku_width = (int) (0.22f*WIDTH),
            goku_height = (int) (0.32f*HEIGHT),
            buu_width = goku_width,
            buu_height = goku_height,
            buu_x = (int) (0.975*WIDTH-buu_width),
            buu_y = goku_y;
    int seperation = buu_x - goku_x - goku_width;
    int gokuBeam_x = goku_x + goku_width,
            gokuBeam_y = goku_y,
            gokuBeam_width,
            gokuBeam_height = goku_height;
    int buuBeam_x = buu_x,
            buuBeam_y = buu_y,
            buuBeam_width = (int) (seperation*(1-percent/100f)),
            buuBeam_height = goku_height;
    int gokuClash_x = gokuBeam_x + gokuBeam_width - (int)(0.35f*seperation),
            gokuClash_y = (int) ((0.75)*goku_y),
            gokuClash_width = (int)(0.35f*seperation),
            gokuClash_height = (int) goku_height;
    int buuClash_x = buuBeam_x - buuBeam_width,
            buuClash_y = (int) ((0.75)*buu_y),
            buuClash_width = (int)(0.25f*seperation),
            buuClash_height = (int) buu_height;
    public boolean percentIsChangeable = false;
    public Connection c;





    public GameRenderer(Game game, GameWorld world, Connection c){
        this.world = world;
        camera = new OrthographicCamera(WIDTH, HEIGHT);

        batch = new SpriteBatch();

        stateTime = 0f;

        //Gdx.input.setInputProcessor(null);
        this.c = c;
    }

    public void render(float delta){
        Gdx.app.log("RENDER", String.valueOf(percent));
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT | GL20.GL_DEPTH_BUFFER_BIT);
        batch.begin();
        batch.draw(AssetLoader.background,0,0,WIDTH,HEIGHT);
        stateTime += delta;

        currentStateGoku = AssetLoader.gokuAnimation.getKeyFrame(stateTime, false);
        currentStateBuu = AssetLoader.buuAnimation.getKeyFrame(stateTime, false);

        batch.draw(currentStateBuu,buu_x,buu_y,buu_width,buu_height);
        batch.draw(currentStateGoku,goku_x,goku_y,goku_width, buu_height);

        batch.draw(AssetLoader.clouds,stateTime/1000,(int)(HEIGHT/1.5));

        if(initCounter != -1 ){
            gokuBeam_width = Math.round(seperation*percent/100f);
            buuBeam_width = Math.round(seperation*Math.abs((1-percent/100f)));
            gokuClash_x = gokuBeam_x + gokuBeam_width - (int)(0.3f*seperation);
            buuClash_x = buuBeam_x - buuBeam_width;

            batch.draw(AssetLoader.gokuBeam,gokuBeam_x,gokuBeam_y,gokuBeam_width,gokuBeam_height);
            batch.draw(AssetLoader.buuBeam,buuBeam_x-buuBeam_width,buuBeam_y,buuBeam_width,buuBeam_height);

            currentGokuClash = AssetLoader.gokuClashAnimation.getKeyFrame(stateTime,true);
            currentBuuClash = AssetLoader.buuClashAnimation.getKeyFrame(stateTime,true);

            batch.draw(currentGokuClash,gokuClash_x,gokuClash_y,gokuClash_width,gokuClash_height);
            batch.draw(currentBuuClash,buuClash_x,buuClash_y,buuClash_width,buuClash_height);
        }
        else {
            setCharacters(delta);
        }
        batch.end();
    }

    public void setCharacters(float delta){

        if(AssetLoader.gokuAnimation.isAnimationFinished(stateTime)){

            percent++;
            gokuBeam_width = Math.round(seperation*percent/100f);
            buuBeam_width = Math.round(seperation*(Math.abs(1-percent))/100f);
            gokuClash_x = gokuBeam_x + gokuBeam_width - (int)(0.3f*seperation);
            buuClash_x = buuBeam_x - buuBeam_width;

            batch.draw(AssetLoader.gokuBeam,gokuBeam_x,gokuBeam_y,gokuBeam_width,gokuBeam_height);
            batch.draw(AssetLoader.buuBeam,buuBeam_x-buuBeam_width,buuBeam_y,buuBeam_width,buuBeam_height);

            if(percent == 50){
                percentIsChangeable = true;
                currentGokuClash = AssetLoader.gokuClashAnimation.getKeyFrame(stateTime,true);
                currentBuuClash = AssetLoader.buuClashAnimation.getKeyFrame(stateTime,true);

                batch.draw(currentGokuClash,gokuClash_x,gokuClash_y,gokuClash_width,gokuClash_height);
                batch.draw(currentBuuClash,buuClash_x,buuClash_y,buuClash_width,buuClash_height);
                initCounter = 1;
            }
        }
    }

    public void setPercent(int percent){
        if(percentIsChangeable) this.percent = percent;
    }

}
