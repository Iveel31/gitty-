#include<iostream>
#include<raylib.h>
#include<vector>
#include<cmath>

using namespace std;
int cpu_score=0;
int player_score=0;
enum class GameMode{
    Singleplayer,
    MultiplayerLocal
};
enum class GameState{
    Menu,
    LevelSelect,
    Playing
};
enum class Difficulty{
    Easy,
    Medium,
    Hard
};
class Ball{
public:
float x,y;
int speed_x, speed_y;
int radius;
void Draw(){
    DrawCircle(x,y,radius,WHITE);
}
void Update(){
    x+=speed_x;
    y+=speed_y;

    if(y+radius>=GetScreenHeight() || y-radius<=0){
        speed_y*=-1;
    }
    if(x+radius>=GetScreenWidth()){
        cpu_score++;
        ResetBall();
    }
    if(x-radius<=0){
        player_score++;
        ResetBall();
    }
}
void ResetBall(){
    x=GetScreenWidth()/2;
    y=GetScreenHeight()/2;
    int speed_choices[2] = {-1,1};
    speed_x *= speed_choices[GetRandomValue(0,1)];
    speed_y *= speed_choices[GetRandomValue(0,1)];
}
};
Ball ball;
class Paddle
{
protected:
void LimitMovement(){
     if(y<=0){
            y=0;
        }
     if(y + height >= GetScreenHeight()){
            y = GetScreenHeight() - height;
     }
}
public:
    float x ,y;
    float width, height;
    int speed;
    void UpdateWithKeys(int upKey, int downKey){
    if (IsKeyDown(upKey)) y -= speed;
    if (IsKeyDown(downKey)) y += speed;
    LimitMovement();
    }
    void draw(){
        DrawRectangle(x,y,width,height,WHITE);
    }
    void Update(){
        if(IsKeyDown(KEY_UP)) y -= speed;
        if(IsKeyDown(KEY_DOWN)) y += speed;
        LimitMovement();
    }
};
Paddle player;
class CpuPaddle : public Paddle{
public:
    void Update(int ball_y){
        if(y+height/2 >ball_y){
            y -= speed;
        }
        if(y + height/2 <= ball_y){
            y += speed;
        }
        LimitMovement();
    }
};
CpuPaddle cpu;

static Sound CreateHitSound(){
    const int sampleRate = 44100;
    const float durationSec = 0.07f;
    const int frameCount = (int)(sampleRate * durationSec);
    const float frequency = 900.0f;

    std::vector<short> samples(frameCount);
    for(int i = 0; i < frameCount; i++){
        float t = (float)i / (float)sampleRate;
        float env = 1.0f - ((float)i / (float)frameCount);
        float s = sinf(2.0f * PI * frequency * t) * env;
        samples[i] = (short)(s * 12000.0f);
    }

    Wave wave{};
    wave.frameCount = (unsigned int)frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples.data();
    return LoadSoundFromWave(wave);
}

int main(){
    cout <<"starting the game" << endl;
    const int screen_width = 1400;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My pong game!");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    Sound hitSound = CreateHitSound();
    SetSoundVolume(hitSound, 0.45f);
    ball.radius=20;
    ball.x=screen_width/2;
    ball.y=screen_height/2;
    ball.speed_x=7;
    ball.speed_y=7;
    player.width=25;
    player.height=120;
    player.x=screen_width-player.width-10;
    player.y=screen_height/2-player.height/2;
    player.speed=6;
    cpu.width=25;
    cpu.height=120;
    cpu.x=10;
    cpu.y=screen_height/2-cpu.height/2;
    cpu.speed = 6;

    GameMode currentMode = GameMode::Singleplayer;
    GameState gameState = GameState::Menu;
    Difficulty currentDifficulty = Difficulty::Medium;

    int base_ball_speed = 7;
    int base_cpu_speed = 6;

    auto ApplyDifficulty = [&]() {
        if (currentDifficulty == Difficulty::Easy) {
            base_ball_speed = 8;
            base_cpu_speed = 6;
        } else if (currentDifficulty == Difficulty::Medium) {
            base_ball_speed = 10;
            base_cpu_speed = 8;
        } else {
            base_ball_speed = 11;
            base_cpu_speed = 9;
        }

        ball.speed_x = (ball.speed_x < 0) ? -base_ball_speed : base_ball_speed;
        ball.speed_y = (ball.speed_y < 0) ? -base_ball_speed : base_ball_speed;
        cpu.speed = base_cpu_speed;
    };

    auto GetDifficultyText = [&]() {
        if (currentDifficulty == Difficulty::Easy) return "Easy";
        if (currentDifficulty == Difficulty::Medium) return "Medium";
        return "Hard";
    };

    auto ApplyMultiplayerSpeedScaling = [&]() {
        if (currentMode != GameMode::MultiplayerLocal) return;

        int totalScore = cpu_score + player_score;
        int speedBonus = totalScore / 2; // Every 2 total points, add +1 speed.
        if (speedBonus > 6) speedBonus = 6;

        int targetSpeed = base_ball_speed + speedBonus;
        ball.speed_x = (ball.speed_x < 0) ? -targetSpeed : targetSpeed;
        ball.speed_y = (ball.speed_y < 0) ? -targetSpeed : targetSpeed;
    };

    auto ResetMatch = [&]() {
        if (currentMode == GameMode::Singleplayer) {
            ApplyDifficulty();
        } else {
            base_ball_speed = 7;
            ball.speed_x = (ball.speed_x < 0) ? -base_ball_speed : base_ball_speed;
            ball.speed_y = (ball.speed_y < 0) ? -base_ball_speed : base_ball_speed;
            cpu.speed = 6;
        }
        cpu_score = 0;
        player_score = 0;
        ball.x = screen_width / 2;
        ball.y = screen_height / 2;
        ball.speed_x = base_ball_speed;
        ball.speed_y = base_ball_speed;
        player.y = screen_height / 2 - player.height / 2;
        cpu.y = screen_height / 2 - cpu.height / 2;
    };

    while (!WindowShouldClose()){
        BeginDrawing();

        if (gameState == GameState::Menu) {
            ClearBackground(BLACK);
            DrawRectangleGradientV(0, 0, screen_width, screen_height, Color{8, 16, 34, 255}, Color{18, 43, 74, 255});
            DrawCircle(screen_width / 2, 130, 220, Color{35, 80, 130, 90});

            DrawText("PONG", screen_width / 2 - 130, 120, 96, WHITE);
            DrawText("Choose game mode", screen_width / 2 - 145, 230, 34, Color{210, 225, 245, 255});

            DrawRectangleRounded(Rectangle{(float)screen_width / 2 - 260, 300, 520, 82}, 0.25f, 12, Color{255, 255, 255, 24});
            DrawRectangleRounded(Rectangle{(float)screen_width / 2 - 260, 400, 520, 82}, 0.25f, 12, Color{255, 255, 255, 24});
            DrawText("1  Singleplayer", screen_width / 2 - 215, 322, 42, WHITE);
            DrawText("2  Multiplayer Local", screen_width / 2 - 215, 422, 42, WHITE);
            DrawText("ESC  Quit", screen_width / 2 - 75, 540, 28, LIGHTGRAY);

            if (IsKeyPressed(KEY_ONE)) {
                currentMode = GameMode::Singleplayer;
                gameState = GameState::LevelSelect;
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentMode = GameMode::MultiplayerLocal;
                ResetMatch();
                gameState = GameState::Playing;
            }
        } else if (gameState == GameState::LevelSelect) {
            ClearBackground(BLACK);
            DrawRectangleGradientV(0, 0, screen_width, screen_height, Color{10, 22, 40, 255}, Color{30, 56, 84, 255});

            DrawText("Singleplayer", screen_width / 2 - 150, 120, 60, WHITE);
            DrawText("Select difficulty", screen_width / 2 - 135, 200, 36, Color{210, 225, 245, 255});

            DrawRectangleRounded(Rectangle{(float)screen_width / 2 - 280, 290, 600, 75}, 0.22f, 12, Color{255, 255, 255, 22});
            DrawRectangleRounded(Rectangle{(float)screen_width / 2 - 280, 385, 600, 75}, 0.22f, 12, Color{255, 255, 255, 22});
            DrawRectangleRounded(Rectangle{(float)screen_width / 2 - 280, 480, 600, 75}, 0.22f, 12, Color{255, 255, 255, 22});

            DrawText("1  Easy    (Slow ball, slower bot)", screen_width / 2 - 240, 312, 34, WHITE);
            DrawText("2  Medium  (Balanced)", screen_width / 2 - 240, 407, 34, WHITE);
            DrawText("3  Hard    (Fast ball, faster bot)", screen_width / 2 - 240, 502, 34, WHITE);
            DrawText("B  Back", screen_width / 2 - 60, 620, 30, LIGHTGRAY);

            if (IsKeyPressed(KEY_ONE)) {
                currentDifficulty = Difficulty::Easy;
                ResetMatch();
                gameState = GameState::Playing;
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentDifficulty = Difficulty::Medium;
                ResetMatch();
                gameState = GameState::Playing;
            }
            if (IsKeyPressed(KEY_THREE)) {
                currentDifficulty = Difficulty::Hard;
                ResetMatch();
                gameState = GameState::Playing;
            }
            if (IsKeyPressed(KEY_B)) {
                gameState = GameState::Menu;
            }
        } else if (gameState == GameState::Playing) {
            // update
            ball.Update();
            ApplyMultiplayerSpeedScaling();
            player.UpdateWithKeys(KEY_UP, KEY_DOWN);

            if (currentMode == GameMode::Singleplayer) {
                cpu.Update(ball.y);
            } else {
                cpu.UpdateWithKeys(KEY_W, KEY_S);
            }

            // checking for collisions
            if(CheckCollisionCircleRec(Vector2{ball.x , ball.y}, ball.radius, Rectangle{player.x,player.y,player.width,player.height})){
                ball.speed_x *= -1;
                ball.x = player.x - ball.radius;
                PlaySound(hitSound);
            }
            if(CheckCollisionCircleRec(Vector2{ball.x , ball.y}, ball.radius, Rectangle{cpu.x,cpu.y,cpu.width,cpu.height})){
                ball.speed_x *= -1;
                ball.x = cpu.x + cpu.width + ball.radius;
                PlaySound(hitSound);
            }

            if (IsKeyPressed(KEY_TAB)) {
                gameState = GameState::Menu;
            }

            // drawing
            ClearBackground(BLACK);
            DrawLine(screen_width/2,screen_height,screen_width/2,0,WHITE);
            ball.Draw();
            cpu.draw();
            player.draw();

            const char* modeText = (currentMode == GameMode::Singleplayer)
                ? TextFormat("Mode: Singleplayer | Level: %s", GetDifficultyText())
                : "Mode: Multiplayer Local";

            int modeTextWidth = MeasureText(modeText, 30);
            int modeTextX = screen_width / 2 - modeTextWidth / 2;

            DrawText(modeText, modeTextX, 20, 30, WHITE);
            if (currentMode == GameMode::Singleplayer) {
                DrawText("Difficulty selected before match", screen_width / 2 - 180, 58, 24, LIGHTGRAY);
            } else {
                int totalScore = cpu_score + player_score;
                int speedBonus = totalScore / 2;
                if (speedBonus > 6) speedBonus = 6;
                int currentSpeed = base_ball_speed + speedBonus;
                DrawText(TextFormat("Ball Speed: %i", currentSpeed), screen_width / 2 - 85, 58, 24, LIGHTGRAY);
            }
            DrawText("Menu: TAB", screen_width - 210, 20, 30, WHITE);
            DrawText(TextFormat("%i",cpu_score),screen_width/4-20,20,80,WHITE);
            DrawText(TextFormat("%i",player_score),(screen_width/4)*3-20,20,80,WHITE);
        }

        EndDrawing();
    }

    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}