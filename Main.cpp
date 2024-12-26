#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>

struct Bullet {
    int x;
    int y;
};

struct Enemy {
    int x;
    int y;
    int xSpeed;
    int ySpeed;
};

//Deletes a bullet from the array of bullets
static void deleteBullet(Bullet* bullets, int position, int& length) {
    for (int i = position; i < length - 1; i++) {
        bullets[i] = bullets[i + 1];
    }
    length--;
}

//Delets an enemy from the array of enemies
static void deleteEnemy(Enemy* enemies, int position, int& length) {
    for (int i = position; i < length - 1; i++) {
        enemies[i] = enemies[i + 1];
    }
    length--;
}

static void resetgame(double& enemySpeedScale, int& level, int& score, int&bulletCount, int& currentEnemiesCount) {
    enemySpeedScale = 1.0;
    level = 1;
    score = 0;
    bulletCount = 0;
    currentEnemiesCount = 0;
}

static void displayMessage(sf::RenderWindow& window, const std::string& message) {
     sf::Font font;
     if (!font.loadFromFile("Assets/bake_soda/Bake Soda.otf")) {
         std::cerr << "Error loading font! \n";
         return;
     }

     sf::Text Text;
     Text.setFont(font);
     Text.setString(message);
     Text.setCharacterSize(20);
     Text.setFillColor(sf::Color::Black);
     Text.setStyle(sf::Text::Bold);
     Text.setPosition(200, 200);

     window.clear(sf::Color::White);
     window.draw(Text);
     window.display();

     while (true) {
         sf::Event event;
         while (window.pollEvent(event)) {
             if (event.type == sf::Event::KeyPressed) {
                 if (event.key.code == sf::Keyboard::Escape) {
                     window.close();
                     return;
                 }
                 if (event.key.code == sf::Keyboard::P) {
                     return;
                 }
             }
         }
     }
 }
 
int main() {

    const int     screenWidth = 800;
    const int     screenHeight = 600;
    const int     playerSize = 60;
    const int     enemySize = 60;
    const int     bulletSize = 5;
    const int     bulletSpeed = 10;
    const int     maxBullets = 40;
    const int     scoreThreshold = 1000;
    const int     maxEnemies = 50;
    const int     hitScore = 10;
    const int     deathScore = 0;
    const int     missScore = -10;
    const double  maxEnemySpeedScale = 3.5f;
    double        enemySpeedScale = 0.5f;
    int           nextlevelscore = scoreThreshold;
    int playerX = (screenWidth - playerSize) / 2;
    int playerY = screenHeight - playerSize - 40;
    int level   = 1;
    int score   = 0;
    int lives   = 3;
   
    //Array that stores bullets
    Bullet bullets[maxBullets]{};
    int bulletCount = 0;

    //Array that stores enemies
    Enemy enemies[maxEnemies]{};
    int currentEnemiesCount = 0;

     sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Space Shooter Clone", sf::Style::Default);
     window.setVerticalSyncEnabled(true);

     //Draw Player Texture
     sf::Texture playerTexture;
     if (!playerTexture.loadFromFile("Assets/Main Ship - Base - Full health.png")) {
         std::cerr << "Error loading player texture! \n";
         return -1;
     }

     //Draw background texture
     sf::Texture backgroundTexture;
     if (!backgroundTexture.loadFromFile("Assets/Stars-B.png")) {
         std::cerr << "Error loading background texture!\n";
         return -1;
     }

     //Draw Enemy Texture
     sf::Texture enemyTexture;
     if (!enemyTexture.loadFromFile("Assets/Kla'ed - Bomber - Base.png")) {
         std::cerr << "Error loading enemy texture! \n";
         return -1;
     }

     //Create sound for explosion
     sf::SoundBuffer explosionbuffer;
     if (!explosionbuffer.loadFromFile("Assets/mixkit-short-laser-gun-shot-1670.wav")) {
         std::cerr << "Error loading Explosion Sound!\n";
         return -1;
     }
     sf::Sound explosionsound;
     explosionsound.setBuffer(explosionbuffer);

     //Create sound for Game over
     sf::SoundBuffer gameoverbuffer;
     if (!gameoverbuffer.loadFromFile("Assets/game-over-arcade-6435.mp3")) {
         std::cerr << "Error loading Game over Sound!\n";
         return -1;
     }
     sf::Sound gameoversound;
     gameoversound.setBuffer(gameoverbuffer);

     //Create sound for game completion
     sf::SoundBuffer congratsbuffer;
     if (!congratsbuffer.loadFromFile("Assets/New Slaves (mp3cut.net).wav")) {
         std::cerr << "Error loading Game completion Sound!\n";
         return -1;
     }
     sf::Sound congratssound;
     congratssound.setBuffer(congratsbuffer);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);

        // Draw Score
        if (score < 0) score = 0;
        sf::Font font;
        if (!font.loadFromFile("Assets/bake_soda/Bake Soda.otf")) {
            std::cerr << "Error loading font!\n";
            return -1;
        }
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::Green);
        scoreText.setPosition(20, 20);
        window.draw(scoreText);

        //Draw level
        sf::Text levelText;
        levelText.setFont(font);
        levelText.setString("Level: " + std::to_string(level));
        levelText.setCharacterSize(20);
        levelText.setFillColor(sf::Color::Green);
        levelText.setPosition(700, 20);
        window.draw(levelText);

        // Draw Player
        sf::Sprite playerSprite(playerTexture);
        playerSprite.setPosition(playerX, playerY);
        window.draw(playerSprite);

        //Draw Background
        sf::Sprite backgroundSprite(backgroundTexture);
        backgroundSprite.setPosition(0, 0);
        window.draw(backgroundSprite);

        //Draw Enemy
        sf::Sprite enemySprite(enemyTexture);

        //Control keys for player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && playerX > 0) {
            playerX -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && playerX < screenWidth - playerSize) {
            playerX += 10;
        }

        // Fire bullets
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (bulletCount < maxBullets) {
                bullets[bulletCount++] = { playerX + playerSize / 2, playerY};
            }
        }

        // Spawn enemies
        if (currentEnemiesCount < 4) {
            int direction = (std::rand() % 3) - 1;
            if (direction == 0) direction = 1;
            enemies[currentEnemiesCount++] = { std::rand() % (screenWidth - enemySize), 0, direction * 2, 2 };
        }

        // Level progression
        if (score >= nextlevelscore) {
            level++;                  // Increment level
            nextlevelscore += scoreThreshold; // Set the score required for the next level
        }

        // Increase enemy speed gradually based on score
        if (score / static_cast<double>((scoreThreshold)) > enemySpeedScale - 1) {
            if (enemySpeedScale < maxEnemySpeedScale) {
                enemySpeedScale += 0.3f;
            } 
            
        }

        // Check win condition
        if (level>10) {
            congratssound.play();
            displayMessage(window, "Congratulations!!! \n You Won! \n Press ESC to close and P to play again.");
            resetgame(enemySpeedScale, level, score, bulletCount, currentEnemiesCount);
            continue;
        }

        // Move enemies and check collisions
        for (int i = 0; i < currentEnemiesCount; i++) {

            // Update enemy speed scaling
            enemies[i].x += enemies[i].xSpeed * enemySpeedScale;
            enemies[i].y += enemies[i].ySpeed * enemySpeedScale;

            // Enemies bounce off screen edges
            if (enemies[i].x <= 0 || enemies[i].x >= screenWidth - enemySize) {
                enemies[i].xSpeed *= -1;
            }

            // Check if enemy moves out of screen bounds (bottom)
            if (enemies[i].y >= screenHeight) {
                deleteEnemy(enemies, i, currentEnemiesCount);
                score += missScore;
                continue;
            }

            // Draw enemy sprite
            enemySprite.setPosition(enemies[i].x, enemies[i].y);
            window.draw(enemySprite);

            //Collision detection with player
            if (sf::IntRect(playerX, playerY, playerSize, playerSize).intersects(
                sf::IntRect(enemies[i].x, enemies[i].y, enemySize, enemySize))) {
                lives--;
                if (lives <= 0) {
                    gameoversound.play();
                    displayMessage(window, "Oops!!! \n Game Over! \n Collision Detected! \n Press ESC to close or P to play again.");
                }
                
                break;
            }
        }
        // Move bullets and check collisions
        for (int i = 0; i < bulletCount; i++) {
            bullets[i].y -= bulletSpeed;

            if (bullets[i].y < 0) {
                deleteBullet(bullets, i, bulletCount);
            }
            else {
                sf::CircleShape bulletShape(4);
                bulletShape.setFillColor(sf::Color::Red);
                bulletShape.setPosition(bullets[i].x, bullets[i].y);
                window.draw(bulletShape);

                for (int j = 0; j < currentEnemiesCount; j++) {
                    if (sf::IntRect(bullets[i].x, bullets[i].y, bulletSize, bulletSize).intersects(
                        sf::IntRect(enemies[j].x, enemies[j].y, enemySize, enemySize))) {

                        deleteEnemy(enemies, j, currentEnemiesCount);
                        deleteBullet(bullets, i, bulletCount);
                        explosionsound.play();
                        score += hitScore;
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}
