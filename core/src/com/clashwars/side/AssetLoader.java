package com.clashwars.side;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

import java.awt.TextComponent;
import java.util.Arrays;


public class AssetLoader {
    public static Texture background, gokuSheet1, gokuSheet2, gokuSheet3, buuSheet1, buuSheet2, buuSheet3, gokuBeam, buuBeam, gokuClashSheet, buuClashSheet, title, clouds,win,loss;
    public static Animation gokuAnimation, buuAnimation, gokuClashAnimation, buuClashAnimation;
    public static TextureRegion[] gokuFrames, buuFrames, gokuClash, buuClash;

    public static void load(){
        background = new Texture(Gdx.files.internal("bg.jpg"));
        gokuSheet1 = new Texture(Gdx.files.internal("goku_sprite/goku_01.png"));
        gokuSheet2 = new Texture(Gdx.files.internal("goku_sprite/goku_02.png"));
        gokuSheet3 = new Texture(Gdx.files.internal("goku_sprite/goku_03.png"));
        buuSheet1 = new Texture(Gdx.files.internal("buu_sprite/kidbuu_01.png"));
        buuSheet2 = new Texture(Gdx.files.internal("buu_sprite/kidbuu_02.png"));
        buuSheet3 = new Texture(Gdx.files.internal("buu_sprite/kidbuu_03.png"));
        buuBeam = new Texture(Gdx.files.internal("kidbuu_light.png"));
        gokuBeam = new Texture(Gdx.files.internal("light.png"));
        gokuClashSheet = new Texture(Gdx.files.internal("goku_fire.png"));
        buuClashSheet = new Texture(Gdx.files.internal("kidbuu_fire.png"));
        title = new Texture(Gdx.files.internal("Title.png"));
        win = new Texture(Gdx.files.internal("winner.png"));
        loss = new Texture(Gdx.files.internal("loser.png"));

        gokuFrames = new TextureRegion[19];
        gokuClash = new TextureRegion[2];
        buuFrames = new TextureRegion[19];
        buuClash = new TextureRegion[2];

        gokuClash[0] = new TextureRegion(gokuClashSheet,26,0,139,496);
        gokuClash[1] = new TextureRegion(gokuClashSheet,226,0,139,496);
        buuClash[0] = new TextureRegion(buuClashSheet,38,0,139,496);
        buuClash[1] = new TextureRegion(buuClashSheet,237,0,139,496);

        final int PARSE_INT = 320;
        for(int i = 0; i < gokuFrames.length; i++){

            if(i < 6) {
                gokuFrames[i] = new TextureRegion(gokuSheet1, PARSE_INT * i, 0, PARSE_INT, gokuSheet1.getHeight());
                buuFrames[buuFrames.length - i - 1] = new TextureRegion(buuSheet1, PARSE_INT * i, 0, PARSE_INT, buuSheet1.getHeight());
            } else if(i < 12) {
                gokuFrames[i] = new TextureRegion(gokuSheet2, PARSE_INT * (i-6), 0, PARSE_INT, gokuSheet2.getHeight());
                buuFrames[buuFrames.length - i - 1] = new TextureRegion(buuSheet2, PARSE_INT * (i-6), 0, PARSE_INT, buuSheet2.getHeight());
            } else if(i < 19) {
                gokuFrames[i] = new TextureRegion(gokuSheet3, PARSE_INT * (i-12), 0, PARSE_INT, gokuSheet3.getHeight());
                buuFrames[buuFrames.length - i - 1] = new TextureRegion(buuSheet3, PARSE_INT * (i-12), 0, PARSE_INT, buuSheet3.getHeight());
            }
        }

        final float FRAME_SPEED = 1.5f;
        gokuAnimation = new Animation(FRAME_SPEED,gokuFrames);
        buuAnimation = new Animation(FRAME_SPEED,buuFrames);

        final float SLOW_RATE = 4.5f;
        gokuClashAnimation = new Animation(SLOW_RATE,gokuClash);
        buuClashAnimation = new Animation(SLOW_RATE,buuClash);
    }

    public static void dispose(){
        background.dispose();
        gokuSheet1.dispose();
        gokuSheet2.dispose();
        gokuSheet3.dispose();
        buuSheet1.dispose();
        buuSheet2.dispose();
        buuSheet3.dispose();
    }

}
