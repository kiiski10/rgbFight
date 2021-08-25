// Fighting game for arduino & rgb led strip.
// Needs controller with two buttons

#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
const int sigPin = 6;
const int btn1 = 2;
const int btn2 = 3;
const int stripLength = 130;
const int maxEnemies = 10;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripLength, sigPin, NEO_GRB + NEO_KHZ800);

// ------------------------------------- //
class Tile {
    public:
        int r = 0;
        int g = 0;
        int b = 0;
};
        
// ------------------------------------- //
class Pixel {
    public:
        int position = 5;
        int lives = 3;
        int color_r = 70;
        int color_g = 170;
        int color_b = 70;
        int movementPenalty = 10;
        int attackChargeTime = 300;
        long movementTimer = millis();
        long attackTimer = millis();
};

// ------------------------------------- //
void createFloor(Tile floorTiles[stripLength]) {
    
    for (int c = 0; c < stripLength; c++) {             // Create floor
        float sl = stripLength;
        sl = (c / sl) * 10;
        
        floorTiles[c].r = 0;
        floorTiles[c].g = sl;
        floorTiles[c].b = 10 - sl;
    }
}

// ------------------------------------- //
void spawnEnemies(Pixel enemies[maxEnemies]) {
    for (int c = 0; c < maxEnemies; c++) {              // Spawn enemies
        enemies[c].position = { random(0, stripLength) };        
        enemies[c].color_r = { 1 };
        enemies[c].color_g = { 0 };
        enemies[c].color_b = { 0 };  
        enemies[c].lives = { 3 };  
        enemies[c].movementPenalty = { random(40, 200) };         
    }
}

// ------------------------------------- //
void fill(int r, int g, int b) {
    for (int c = 0; c < stripLength; c ++) {
        strip.setPixelColor(c, r, g, b);
    }
    strip.show();
}

// ------------------------------------- //
void attack(int force, Pixel player, Pixel enemies[maxEnemies]) {
    for (int count = 0; count <= force; count += 1) {           // Special effect
        strip.setPixelColor(player.position + count, 250, 150, 0); 
        strip.setPixelColor(player.position - count, 250, 150, 0);
        strip.show();
    }
    for (int c = 0; c < maxEnemies; c++) {
        if ((enemies[c].position > player.position - force) && (enemies[c].position < player.position + force)) {
            if (enemies[c].lives >= 1) {
                enemies[c].lives --;
                               
                float r = enemies[c].lives;
                r = (r / 3) * 100;
                enemies[c].color_r = 100 - r;
                enemies[c].movementPenalty -= 50;
                if (enemies[c].movementPenalty < 13) {
                    enemies[c].movementPenalty = 13;
                }
            }
            int deads = 0;
            for (int c = 0; c < maxEnemies; c++) {
                if (enemies[c].lives == 0) {
                    deads ++;
                }
                if (deads >= maxEnemies) {      // Level complete
                    deads = 0;
                    for (int c = 0; c < 10; c++) {
                        fill(100,100,100);
                        delay(20);
                        fill(0,0,0);
                        delay(100);
                    }
                    spawnEnemies(enemies);
                }
            }
        }
    }
    delay(30);
}


// ------------------------------------- //
void drawPlr(Pixel player) {
    strip.setPixelColor(player.position, player.color_r, player.color_g, player.color_b);
}

// ------------------------------------- //
void drawBackground(Tile floorTiles[stripLength]) {
    for (int c = 0; c < stripLength; c ++) {
        strip.setPixelColor(c, floorTiles[c].r, floorTiles[c].g, floorTiles[c].b);
    }
}

// ------------------------------------- //
void moveEnemies(Pixel enemies[maxEnemies]) {
    for (int c = 0; c < maxEnemies; c ++) {
        if (((millis() - enemies[c].movementTimer) > enemies[c].movementPenalty) && (enemies[c].lives > 0)) {
            enemies[c].position ++;
            if (enemies[c].position > stripLength) {
                enemies[c].position = 0;
                Serial.println("ID:" + String(c) + " Lives:" + enemies[c].lives + " MovPen:" + enemies[c].movementPenalty);
            }
            enemies[c].movementTimer = millis();
        }
    }
}

// ------------------------------------- //
void drawEnemies(Pixel enemies[maxEnemies]) {
    for (int c = 0; c < maxEnemies; c++) {
        if (enemies[c].lives > 0) {
            strip.setPixelColor(enemies[c].position, enemies[c].color_r, enemies[c].color_g, enemies[c].color_b);
        }
    }
}

// -------------------------------------------------------------------------- //

Tile floorTiles[stripLength];

Pixel player;
Pixel enemies[maxEnemies];

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();
    pinMode(btn1, INPUT);
    pinMode(btn2, INPUT);
    digitalWrite(btn1, HIGH);
    digitalWrite(btn2, HIGH);
    spawnEnemies(enemies);
    createFloor(floorTiles);
}


void loop() {
    int btn1state = digitalRead(btn1);
    int btn2state = digitalRead(btn2);
    
    if ((btn1state == 0) && (btn2state == 0)) {
        if ((millis() - player.attackTimer) > player.attackChargeTime) {
            attack(3, player, enemies);
            player.attackTimer = millis();
        }
    } else if ((millis() - player.movementTimer) > player.movementPenalty) {
        if (btn1state == 0) {
            player.position --;
            player.movementTimer = millis();
            if (player.position < 0) {
                player.position = stripLength;
            }
        } 
        else if (btn2state == 0) {
            player.position ++;
            player.movementTimer = millis();
            if (player.position > stripLength) {
                player.position = 0;
            }
        }
    }

    moveEnemies(enemies);
    drawBackground(floorTiles);
    drawEnemies(enemies);
    drawPlr(player);
    strip.show();
}

