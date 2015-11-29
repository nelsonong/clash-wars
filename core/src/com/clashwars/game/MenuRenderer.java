//package com.clashwars.game;
//
//
//import com.badlogic.gdx.Gdx;
//import com.badlogic.gdx.graphics.OrthographicCamera;
//import com.badlogic.gdx.graphics.Texture;
//import com.badlogic.gdx.graphics.g2d.SpriteBatch;
//import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
//import com.clashwars.side.AssetLoader;
//
//public class MenuRenderer {
//    // Screen Display Constants
//    public static final int WIDTH = Gdx.graphics.getWidth();
//    public static final int HEIGHT = Gdx.graphics.getHeight();
//
//    // Camera manage view window
//    private OrthographicCamera camera;
//
//    // Game World the Renderer renders
//    private GameWorld world;
//
//    private ShapeRenderer shapeRenderer;
//    private SpriteBatch batch;
//
//    public MenuRenderer(GameWorld world){
//        this.world = world;
//        camera = new OrthographicCamera(WIDTH, HEIGHT);
//
//        shapeRenderer = new ShapeRenderer();
//        batch = new SpriteBatch();
//    }
//
//    public void render(float delta){
//        shapeRenderer.begin(ShapeRenderer.ShapeType.Filled);
//        shapeRenderer.setColor(255,0,0,1);
//        shapeRenderer.rect(0,0,WIDTH,HEIGHT);
//        shapeRenderer.end();
//
//        batch.begin();
//        batch.draw(AssetLoader.background,0,0,WIDTH,HEIGHT);
//        batch.end();
//    }
//
//}