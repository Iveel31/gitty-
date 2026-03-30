#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>

// ============================================================================
// Constants and Enums
// ============================================================================

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 700;
const float GESTURE_MIN_DIST = 30.0f;

enum GameState {
    STATE_MENU,
    STATE_PLAYING_PVP,
    STATE_PLAYING_PVE,
    STATE_GAME_OVER
};

enum SkillType {
    SKILL_NONE,
    SKILL_PROJECTILE,
    SKILL_SHIELD,
    SKILL_DASH,
    SKILL_LIGHTNING,
    SKILL_WHIRLWIND
};

enum AnimState {
    ANIM_IDLE,
    ANIM_RUN,
    ANIM_ATTACK1,
    ANIM_ATTACK2,
    ANIM_ATTACK3,
    ANIM_HIT,
    ANIM_DEATH,
    ANIM_UP,
    ANIM_DOWN,
    ANIM_FLASH,
    ANIM_COUNT
};

enum PlayerSide {
    SIDE_LEFT,
    SIDE_RIGHT
};

// ============================================================================
// Structures
// ============================================================================

struct Particle {
    Vector2 pos;
    Vector2 vel;
    float life;
    Color color;
};

struct Projectile {
    Vector2 pos;
    Vector2 vel;
    float size;
    PlayerSide owner;
    float life;
};

struct Shield {
    Vector2 pos;
    float radius;
    float duration;
    PlayerSide owner;
};

struct AnimationClip {
    Texture2D texture;
    int frames;
    float frameTime;
    bool loop;
};

struct Player {
    Vector2 pos;
    Vector2 vel;
    float width;
    float height;
    float maxHP;
    float hp;
    float speed;
    float skillCooldown;
    PlayerSide side;
    SkillType lastSkill;
    bool isAI;
    std::array<AnimationClip, ANIM_COUNT> clips;
    AnimState animState;
    int animFrame;
    float animTimer;
    float attackAnimTimer;
    float hurtAnimTimer;
    float contactDamageCooldown;
};

struct Level {
    Player p1;
    Player p2;
    std::vector<Vector2> drawnPath;
    std::vector<Projectile> projectiles;
    std::vector<Shield> shields;
    std::vector<Particle> particles;
    float screenShakeAmount;
    float screenShakeDuration;
};

// ============================================================================
// Global Variables
// ============================================================================

GameState gameState = STATE_MENU;
Level level;
int winner = -1; // -1 = none, 0 = left, 1 = right
bool playerVsAI = false;
Texture2D menuPreview = {0};

const char* P1_IDLE_SPRITE = "Sprite/Idle.png";
const char* P1_RUN_SPRITE = "Sprite/Run.png";
const char* P1_ATTACK1_SPRITE = "Sprite/Attack1.png";
const char* P1_ATTACK2_SPRITE = "Sprite/Attack2.png";
const char* P1_ATTACK3_SPRITE = "Sprite/Attack3.png";
const char* P1_HIT_SPRITE = "Sprite/Take Hit.png";
const char* P1_DEATH_SPRITE = "Sprite/Death.png";
const char* P1_UP_SPRITE = "Sprite/Going Up.png";
const char* P1_DOWN_SPRITE = "Sprite/Going Down.png";
const char* P1_PREVIEW_SPRITE = "Sprite/Idle.png";

const char* P2_IDLE_SPRITE = "Sprites/Idle.png";
const char* P2_RUN_SPRITE = "Sprites/Run.png";
const char* P2_ATTACK1_SPRITE = "Sprites/Attack1.png";
const char* P2_ATTACK2_SPRITE = "Sprites/Attack2.png";
const char* P2_ATTACK3_SPRITE = "Sprites/Attack3.png";
const char* P2_HIT_SPRITE = "Sprites/Take Hit.png";
const char* P2_HIT_WHITE_SPRITE = "Sprites/Take Hit - white silhouette.png";
const char* P2_DEATH_SPRITE = "Sprites/Death.png";
const char* P2_UP_SPRITE = "Sprites/Jump.png";
const char* P2_DOWN_SPRITE = "Sprites/Fall.png";

void InitializeGame();

void ResetAnimationClip(AnimationClip& clip) {
    clip.texture = {0};
    clip.frames = 1;
    clip.frameTime = 0.12f;
    clip.loop = true;
}

void LoadAnimationClip(AnimationClip& clip, const char* path, float frameTime, bool loop, int frames = 0) {
    ResetAnimationClip(clip);
    clip.frameTime = frameTime;
    clip.loop = loop;

    if (!FileExists(path)) return;

    clip.texture = LoadTexture(path);
    if (clip.texture.id == 0 || clip.texture.height <= 0) return;
    SetTextureFilter(clip.texture, TEXTURE_FILTER_POINT);

    if (frames > 0) {
        clip.frames = std::max(1, frames);
    } else {
        int frameWidth = clip.texture.height;
        clip.frames = std::max(1, clip.texture.width / frameWidth);
    }
}

void UnloadAnimationClip(AnimationClip& clip) {
    if (clip.texture.id > 0) {
        UnloadTexture(clip.texture);
    }
    ResetAnimationClip(clip);
}

void UnloadPlayerSprites(Player& player) {
    for (auto& clip : player.clips) {
        UnloadAnimationClip(clip);
    }
}

void LoadPlayerSprites(Player& player) {
    for (auto& clip : player.clips) {
        ResetAnimationClip(clip);
    }

    if (player.side == SIDE_LEFT) {
        LoadAnimationClip(player.clips[ANIM_IDLE], P1_IDLE_SPRITE, 0.14f, true, 10);
        LoadAnimationClip(player.clips[ANIM_RUN], P1_RUN_SPRITE, 0.09f, true, 8);
        LoadAnimationClip(player.clips[ANIM_ATTACK1], P1_ATTACK1_SPRITE, 0.08f, false, 7);
        LoadAnimationClip(player.clips[ANIM_ATTACK2], P1_ATTACK2_SPRITE, 0.07f, false, 6);
        LoadAnimationClip(player.clips[ANIM_ATTACK3], P1_ATTACK3_SPRITE, 0.07f, false, 9);
        LoadAnimationClip(player.clips[ANIM_HIT], P1_HIT_SPRITE, 0.08f, false, 3);
        LoadAnimationClip(player.clips[ANIM_DEATH], P1_DEATH_SPRITE, 0.12f, false, 11);
        LoadAnimationClip(player.clips[ANIM_UP], P1_UP_SPRITE, 0.10f, true, 3);
        LoadAnimationClip(player.clips[ANIM_DOWN], P1_DOWN_SPRITE, 0.10f, true, 3);
    } else {
        LoadAnimationClip(player.clips[ANIM_IDLE], P2_IDLE_SPRITE, 0.14f, true, 8);
        LoadAnimationClip(player.clips[ANIM_RUN], P2_RUN_SPRITE, 0.09f, true, 8);
        LoadAnimationClip(player.clips[ANIM_ATTACK1], P2_ATTACK1_SPRITE, 0.08f, false, 4);
        LoadAnimationClip(player.clips[ANIM_ATTACK2], P2_ATTACK2_SPRITE, 0.07f, false, 4);
        LoadAnimationClip(player.clips[ANIM_ATTACK3], P2_ATTACK3_SPRITE, 0.07f, false, 4);
        LoadAnimationClip(player.clips[ANIM_HIT], P2_HIT_SPRITE, 0.08f, false, 4);
        LoadAnimationClip(player.clips[ANIM_DEATH], P2_DEATH_SPRITE, 0.12f, false, 6);
        LoadAnimationClip(player.clips[ANIM_UP], P2_UP_SPRITE, 0.10f, true, 2);
        LoadAnimationClip(player.clips[ANIM_DOWN], P2_DOWN_SPRITE, 0.10f, true, 2);
        LoadAnimationClip(player.clips[ANIM_FLASH], P2_HIT_WHITE_SPRITE, 0.06f, true, 4);
    }
}

AnimationClip& GetAnimClip(Player& player, AnimState state) {
    AnimationClip& requested = player.clips[state];
    if (requested.texture.id > 0) return requested;
    return player.clips[ANIM_IDLE];
}

SkillType GetAssignedSkill(PlayerSide side) {
    return (side == SIDE_LEFT) ? SKILL_DASH : SKILL_PROJECTILE;
}

// ============================================================================
// Gesture Recognition
// ============================================================================

float GetDistance(Vector2 a, Vector2 b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float GetPathDeviation(const std::vector<Vector2>& path) {
    if (path.size() < 3) return 0;

    Vector2 start = path.front();
    Vector2 end = path.back();
    Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));

    float totalDev = 0;
    for (const auto& p : path) {
        Vector2 toPoint = Vector2Subtract(p, start);
        float crossDist = std::fabs(dir.x * toPoint.y - dir.y * toPoint.x);
        totalDev += crossDist;
    }
    return totalDev / path.size();
}

bool IsCircle(const std::vector<Vector2>& path) {
    if (path.size() < 8) return false;

    Vector2 start = path.front();
    Vector2 end = path.back();
    float closeness = GetDistance(start, end);

    if (closeness > 50) return false; // Start and end should be close

    // Check if points are distributed in a circle
    float centerX = 0, centerY = 0;
    for (const auto& p : path) {
        centerX += p.x;
        centerY += p.y;
    }
    centerX /= path.size();
    centerY /= path.size();

    Vector2 center = {centerX, centerY};
    float avgRadius = 0;
    for (const auto& p : path) {
        avgRadius += GetDistance(p, center);
    }
    avgRadius /= path.size();

    for (const auto& p : path) {
        float r = GetDistance(p, center);
        if (std::fabs(r - avgRadius) > avgRadius * 0.3f) {
            return false;
        }
    }
    return true;
}

bool IsZigzag(const std::vector<Vector2>& path) {
    if (path.size() < 4) return false;

    int directionChanges = 0;
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        Vector2 v1 = Vector2Subtract(path[i], path[i - 1]);
        Vector2 v2 = Vector2Subtract(path[i + 1], path[i]);

        float dot = v1.x * v2.x + v1.y * v2.y;
        if (dot < 0) {
            directionChanges++;
        }
    }

    return directionChanges >= 3;
}

bool IsVShape(const std::vector<Vector2>& path) {
    if (path.size() < 5) return false;

    Vector2 start = path.front();
    Vector2 end = path.back();

    // V should be moderately straight overall but have a sharp turn
    float startToEnd = GetDistance(start, end);

    int sharpAngles = 0;
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        Vector2 v1 = Vector2Normalize(Vector2Subtract(path[i], path[i - 1]));
        Vector2 v2 = Vector2Normalize(Vector2Subtract(path[i + 1], path[i]));

        float dot = v1.x * v2.x + v1.y * v2.y;
        if (dot < -0.5f) {
            sharpAngles++;
        }
    }

    return sharpAngles >= 1;
}

bool IsLine(const std::vector<Vector2>& path) {
    if (path.size() < 3) return false;
    float deviation = GetPathDeviation(path);
    return deviation < 15.0f;
}

bool IsCrossSlash(const std::vector<Vector2>& path) {
    if (path.size() < 8) return false;

    float minX = path[0].x;
    float maxX = path[0].x;
    float minY = path[0].y;
    float maxY = path[0].y;

    int positiveSlope = 0;
    int negativeSlope = 0;

    for (size_t i = 1; i < path.size(); ++i) {
        minX = std::min(minX, path[i].x);
        maxX = std::max(maxX, path[i].x);
        minY = std::min(minY, path[i].y);
        maxY = std::max(maxY, path[i].y);

        float dx = path[i].x - path[i - 1].x;
        float dy = path[i].y - path[i - 1].y;
        if (std::fabs(dx) < 0.01f || std::fabs(dy) < 0.01f) continue;

        if (dx * dy > 0) positiveSlope++;
        if (dx * dy < 0) negativeSlope++;
    }

    float width = maxX - minX;
    float height = maxY - minY;
    return width > 80 && height > 80 && positiveSlope >= 2 && negativeSlope >= 2;
}

SkillType RecognizeGesture(const std::vector<Vector2>& path) {
    if (path.size() < 3) return SKILL_NONE;

    float totalDist = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        totalDist += GetDistance(path[i], path[i + 1]);
    }

    if (totalDist < GESTURE_MIN_DIST) return SKILL_NONE;

    if (IsCrossSlash(path)) return SKILL_WHIRLWIND;
    if (IsCircle(path)) return SKILL_SHIELD;
    if (IsZigzag(path)) return SKILL_LIGHTNING;
    if (IsVShape(path)) return SKILL_DASH;
    if (IsLine(path)) return SKILL_PROJECTILE;

    return SKILL_NONE;
}

// ============================================================================
// Combat and Effects
// ============================================================================

void AddParticles(Vector2 pos, Color color, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos = pos;
        p.vel = {(float)(std::rand() % 200 - 100) / 100.0f * 200,
                 (float)(std::rand() % 200 - 100) / 100.0f * 200};
        p.life = 0.5f;
        p.color = color;
        level.particles.push_back(p);
    }
}

void ScreenShake(float amount) {
    level.screenShakeAmount = amount;
    level.screenShakeDuration = 0.1f;
}

void ApplyDamage(Player& target, float damage, Color hitColor) {
    target.hp -= damage;
    if (target.hp < 0) target.hp = 0;
    target.hurtAnimTimer = 0.28f;
    AddParticles(target.pos, hitColor, 16);
}

void ExecuteSkill(Player& attacker, SkillType skill) {
    if (attacker.skillCooldown > 0) return;

    Vector2 direction = (attacker.side == SIDE_LEFT) ? (Vector2){1, 0} : (Vector2){-1, 0};
    Vector2 skillPos = {attacker.pos.x + direction.x * 40, attacker.pos.y};

    switch (skill) {
        case SKILL_PROJECTILE: {
            Projectile proj;
            proj.pos = skillPos;
            proj.vel = Vector2Scale(direction, 400);
            proj.size = 8;
            proj.owner = attacker.side;
            proj.life = 5.0f;
            level.projectiles.push_back(proj);
            attacker.skillCooldown = 0.5f;
            AddParticles(skillPos, ORANGE, 10);
            ScreenShake(3.0f);
            break;
        }
        case SKILL_SHIELD: {
            Shield shield;
            shield.pos = attacker.pos;
            shield.radius = 40;
            shield.duration = 1.5f;
            shield.owner = attacker.side;
            level.shields.push_back(shield);
            attacker.skillCooldown = 2.0f;
            AddParticles(shield.pos, BLUE, 15);
            break;
        }
        case SKILL_DASH: {
            attacker.vel = Vector2Scale(direction, 500);
            attacker.skillCooldown = 1.0f;
            ScreenShake(5.0f);

            // Dash damage
            Player& opponent = (attacker.side == SIDE_LEFT) ? level.p2 : level.p1;
            if (GetDistance(attacker.pos, opponent.pos) < 60) {
                ApplyDamage(opponent, 15, RED);
                ScreenShake(8.0f);
            }
            break;
        }
        case SKILL_LIGHTNING: {
            Player& opponent = (attacker.side == SIDE_LEFT) ? level.p2 : level.p1;
            ApplyDamage(opponent, 25, YELLOW);
            attacker.skillCooldown = 1.5f;
            ScreenShake(10.0f);
            AddParticles(opponent.pos, YELLOW, 25);
            break;
        }
        case SKILL_WHIRLWIND: {
            Player& opponent = (attacker.side == SIDE_LEFT) ? level.p2 : level.p1;
            attacker.skillCooldown = 2.8f;
            attacker.hp += 8;
            if (attacker.hp > attacker.maxHP) attacker.hp = attacker.maxHP;

            if (GetDistance(attacker.pos, opponent.pos) < 220) {
                ApplyDamage(opponent, 18, ORANGE);
                float push = (attacker.side == SIDE_LEFT) ? 240.0f : -240.0f;
                opponent.vel.x += push;
            }

            AddParticles(attacker.pos, ORANGE, 24);
            ScreenShake(9.0f);
            break;
        }
        default:
            break;
    }

    attacker.lastSkill = skill;
    attacker.attackAnimTimer = 0.30f;
}

// ============================================================================
// AI Logic
// ============================================================================

void UpdateAI(Player& aiPlayer, Player& opponent) {
    if (aiPlayer.skillCooldown > 0) return;

    float dist = GetDistance(aiPlayer.pos, opponent.pos);

    // AI has one fixed skill depending on side.
    if (dist < 260 && std::rand() % 100 < 35) {
        ExecuteSkill(aiPlayer, GetAssignedSkill(aiPlayer.side));
    }
}

// ============================================================================
// Update Logic
// ============================================================================

void UpdatePlayer(Player& player, float dt) {
    // Velocity decay
    player.vel = Vector2Scale(player.vel, 0.95f);

    // Movement clamp
    float maxSpeed = 300;
    float speed = std::sqrt(player.vel.x * player.vel.x + player.vel.y * player.vel.y);
    if (speed > maxSpeed) {
        player.vel = Vector2Scale(Vector2Normalize(player.vel), maxSpeed);
    }

    // Position update
    player.pos.x += player.vel.x * dt;
    player.pos.y += player.vel.y * dt;

    // Boundary check
    if (player.pos.y - player.height / 2 < 50) {
        player.pos.y = 50 + player.height / 2;
        player.vel.y = 0;
    }
    if (player.pos.y + player.height / 2 > WINDOW_HEIGHT - 50) {
        player.pos.y = WINDOW_HEIGHT - 50 - player.height / 2;
        player.vel.y = 0;
    }

    // Skill cooldown
    if (player.skillCooldown > 0) {
        player.skillCooldown -= dt;
    }

    if (player.attackAnimTimer > 0) {
        player.attackAnimTimer -= dt;
    }

    if (player.hurtAnimTimer > 0) {
        player.hurtAnimTimer -= dt;
    }

    if (player.contactDamageCooldown > 0) {
        player.contactDamageCooldown -= dt;
    }

    // HP clamp
    if (player.hp < 0) player.hp = 0;
    if (player.hp > player.maxHP) player.hp = player.maxHP;

    AnimState nextState = ANIM_IDLE;
    if (player.hp <= 0) {
        nextState = ANIM_DEATH;
    } else if (player.hurtAnimTimer > 0) {
        nextState = ANIM_HIT;
    } else if (player.attackAnimTimer > 0) {
        if (player.lastSkill == SKILL_DASH) {
            nextState = ANIM_ATTACK2;
        } else if (player.lastSkill == SKILL_LIGHTNING || player.lastSkill == SKILL_WHIRLWIND) {
            nextState = ANIM_ATTACK3;
        } else {
            nextState = ANIM_ATTACK1;
        }
    } else if (std::fabs(player.vel.y) > 120) {
        nextState = (player.vel.y < 0) ? ANIM_UP : ANIM_DOWN;
    } else if (std::fabs(player.vel.x) > 35 || std::fabs(player.vel.y) > 35) {
        nextState = ANIM_RUN;
    }

    if (player.animState != nextState) {
        player.animState = nextState;
        player.animFrame = 0;
        player.animTimer = 0;
    }

    AnimationClip& clip = GetAnimClip(player, player.animState);
    player.animTimer += dt;
    while (player.animTimer >= clip.frameTime) {
        player.animTimer -= clip.frameTime;
        if (clip.loop) {
            player.animFrame = (player.animFrame + 1) % clip.frames;
        } else if (player.animFrame + 1 < clip.frames) {
            player.animFrame++;
        }
    }
}

void UpdateProjectiles(float dt) {
    for (auto& proj : level.projectiles) {
        proj.pos.x += proj.vel.x * dt;
        proj.pos.y += proj.vel.y * dt;
        proj.life -= dt;

        // Check collision with opponent
        Player& opponent = (proj.owner == SIDE_LEFT) ? level.p2 : level.p1;

        // Check shields first
        bool hitShield = false;
        for (auto& shield : level.shields) {
            if (shield.owner != proj.owner) {
                if (GetDistance(proj.pos, shield.pos) < shield.radius) {
                    hitShield = true;
                    AddParticles(proj.pos, BLUE, 8);
                    break;
                }
            }
        }

        if (!hitShield && GetDistance(proj.pos, opponent.pos) < 20) {
            ApplyDamage(opponent, 10, RED);
            proj.life = 0;
            AddParticles(proj.pos, RED, 8);
            ScreenShake(5.0f);
        }

        // Out of bounds
        if (proj.pos.x < -100 || proj.pos.x > WINDOW_WIDTH + 100 ||
            proj.pos.y < -100 || proj.pos.y > WINDOW_HEIGHT + 100) {
            proj.life = 0;
        }
    }

    // Remove dead projectiles
    level.projectiles.erase(
        std::remove_if(level.projectiles.begin(), level.projectiles.end(),
                       [](const Projectile& p) { return p.life <= 0; }),
        level.projectiles.end());
}

void UpdateShields(float dt) {
    for (auto& shield : level.shields) {
        shield.duration -= dt;
    }

    level.shields.erase(
        std::remove_if(level.shields.begin(), level.shields.end(),
                       [](const Shield& s) { return s.duration <= 0; }),
        level.shields.end());
}

void UpdateParticles(float dt) {
    for (auto& p : level.particles) {
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.vel = Vector2Scale(p.vel, 0.95f);
        p.life -= dt;
    }

    level.particles.erase(
        std::remove_if(level.particles.begin(), level.particles.end(),
                       [](const Particle& p) { return p.life <= 0; }),
        level.particles.end());
}

void Update(float dt) {
    if (gameState != STATE_PLAYING_PVP && gameState != STATE_PLAYING_PVE) return;

    UpdatePlayer(level.p1, dt);
    UpdatePlayer(level.p2, dt);
    UpdateProjectiles(dt);
    UpdateShields(dt);
    UpdateParticles(dt);

    if (level.screenShakeDuration > 0) {
        level.screenShakeDuration -= dt;
    }

    // Contact damage: fighters must clash to trade HP.
    if (level.p1.hp > 0 && level.p2.hp > 0) {
        float collisionDist = (level.p1.width + level.p2.width) * 0.28f;
        float dist = GetDistance(level.p1.pos, level.p2.pos);
        bool touching = dist <= collisionDist;
        if (touching && level.p1.contactDamageCooldown <= 0 && level.p2.contactDamageCooldown <= 0) {
            ApplyDamage(level.p1, 6.0f, ORANGE);
            ApplyDamage(level.p2, 6.0f, ORANGE);

            float pushDir = (level.p1.pos.x < level.p2.pos.x) ? -1.0f : 1.0f;
            level.p1.vel.x += 170.0f * pushDir;
            level.p2.vel.x -= 170.0f * pushDir;

            level.p1.contactDamageCooldown = 0.35f;
            level.p2.contactDamageCooldown = 0.35f;
            ScreenShake(4.0f);
        }
    }

    // Check win condition
    if (level.p1.hp <= 0) {
        gameState = STATE_GAME_OVER;
        winner = 1;
    }
    if (level.p2.hp <= 0) {
        gameState = STATE_GAME_OVER;
        winner = 0;
    }
}

// ============================================================================
// Input Handling
// ============================================================================

void StartMatch(bool vsAI) {
    playerVsAI = vsAI;
    InitializeGame();
    gameState = vsAI ? STATE_PLAYING_PVE : STATE_PLAYING_PVP;
}

void HandleInput() {
    if (gameState == STATE_MENU) {
        if (IsKeyPressed(KEY_ONE)) {
            StartMatch(false);
        }
        if (IsKeyPressed(KEY_TWO)) {
            StartMatch(true);
        }
    } else if (gameState == STATE_PLAYING_PVP || gameState == STATE_PLAYING_PVE) {
        // Player 1 (left) - mouse drawing
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (level.drawnPath.empty() || GetDistance(level.drawnPath.back(), mousePos) > 5) {
                level.drawnPath.push_back(mousePos);
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (!level.drawnPath.empty()) {
                SkillType skill = RecognizeGesture(level.drawnPath);
                if (skill != SKILL_NONE) {
                    ExecuteSkill(level.p1, GetAssignedSkill(level.p1.side));
                }
                level.drawnPath.clear();
            }
        }

        // Player 2 (right) - keyboard or AI
        if (!playerVsAI) {
            if (IsKeyDown(KEY_UP)) level.p2.vel.y = -level.p2.speed;
            if (IsKeyDown(KEY_DOWN)) level.p2.vel.y = level.p2.speed;
            if (IsKeyDown(KEY_LEFT)) level.p2.vel.x = -level.p2.speed;
            if (IsKeyDown(KEY_RIGHT)) level.p2.vel.x = level.p2.speed;

            if (IsKeyPressed(KEY_Q)) ExecuteSkill(level.p2, GetAssignedSkill(level.p2.side));
        } else {
            // AI movement
            if (level.p2.skillCooldown <= 0 || std::rand() % 100 < 20) {
                UpdateAI(level.p2, level.p1);
            }

            // Simple AI movement toward/away from opponent
            float dist = GetDistance(level.p2.pos, level.p1.pos);
            if (dist > 200) {
                level.p2.vel.x = 150;
            } else if (dist < 100) {
                level.p2.vel.x = -150;
            }
        }

        // Player 1 movement with keys as alternative
        if (IsKeyDown(KEY_W)) level.p1.vel.y = -level.p1.speed;
        if (IsKeyDown(KEY_S)) level.p1.vel.y = level.p1.speed;
        if (IsKeyDown(KEY_A)) level.p1.vel.x = -level.p1.speed;
        if (IsKeyDown(KEY_D)) level.p1.vel.x = level.p1.speed;

        if (IsKeyPressed(KEY_R)) {
            gameState = STATE_MENU;
        }
    } else if (gameState == STATE_GAME_OVER) {
        if (IsKeyPressed(KEY_SPACE)) {
            StartMatch(playerVsAI);
        }
        if (IsKeyPressed(KEY_M)) {
            gameState = STATE_MENU;
        }
    }
}

// ============================================================================
// Drawing
// ============================================================================

void DrawHealthBar(Vector2 pos, float maxHP, float currentHP, float width) {
    float barHeight = 15;
    DrawRectangle((int)(pos.x - width / 2), (int)(pos.y - 40), (int)width, (int)barHeight, DARKGRAY);

    float healthPercent = currentHP / maxHP;
    Color healthColor = (healthPercent > 0.5f) ? GREEN : (healthPercent > 0.25f) ? ORANGE : RED;
    DrawRectangle((int)(pos.x - width / 2), (int)(pos.y - 40), (int)(width * healthPercent), (int)barHeight,
                  healthColor);
    DrawRectangleLines((int)(pos.x - width / 2), (int)(pos.y - 40), (int)width, (int)barHeight, WHITE);
}

void DrawPlayer(const Player& player) {
    const AnimationClip& clip = player.clips[player.animState];
    if (clip.texture.id > 0) {
        float frameWidth = (float)clip.texture.width / clip.frames;
        int frame = player.animFrame;
        if (frame >= clip.frames) frame = clip.frames - 1;

        Rectangle src = {frame * frameWidth, 0.0f, frameWidth, (float)clip.texture.height};
        if (player.side == SIDE_RIGHT) {
            src.x += src.width;
            src.width = -src.width;
        }

        Rectangle dst = {player.pos.x, player.pos.y, player.width, player.height};
        Vector2 origin = {player.width / 2.0f, player.height / 2.0f};
        DrawTexturePro(clip.texture, src, dst, origin, 0.0f, WHITE);

        const AnimationClip& flashClip = player.clips[ANIM_FLASH];
        if (player.hurtAnimTimer > 0 && flashClip.texture.id > 0) {
            float flashFrameWidth = (float)flashClip.texture.width / flashClip.frames;
            int flashFrame = ((int)(GetTime() * 18)) % flashClip.frames;
            Rectangle flashSrc = {flashFrame * flashFrameWidth, 0.0f, flashFrameWidth, (float)flashClip.texture.height};
            if (player.side == SIDE_RIGHT) {
                flashSrc.x += flashSrc.width;
                flashSrc.width = -flashSrc.width;
            }
            Color glow = {255, 255, 255, 120};
            DrawTexturePro(flashClip.texture, flashSrc, dst, origin, 0.0f, glow);
        }
    } else {
        Color playerColor = (player.side == SIDE_LEFT) ? BLUE : RED;
        DrawRectangle((int)(player.pos.x - player.width / 2), (int)(player.pos.y - player.height / 2),
                      (int)player.width, (int)player.height, playerColor);

        // Fallback face if the texture is missing.
        float eyeY = player.pos.y - player.height / 4;
        DrawCircle((int)(player.pos.x - 7), (int)eyeY, 3, WHITE);
        DrawCircle((int)(player.pos.x + 7), (int)eyeY, 3, WHITE);
    }

    DrawHealthBar(player.pos, player.maxHP, player.hp, 60);

    // Draw cooldown indicator
    if (player.skillCooldown > 0) {
        float cooldownCircle = (1 - player.skillCooldown / 2.0f) * 360;
        DrawRing(player.pos, 45, 48, 0, cooldownCircle, 16, YELLOW);
    }
}

void DrawProjectile(const Projectile& proj) {
    Color color = (proj.owner == SIDE_LEFT) ? ORANGE : ORANGE;
    DrawCircle((int)proj.pos.x, (int)proj.pos.y, proj.size, color);
    DrawCircleLines((int)proj.pos.x, (int)proj.pos.y, proj.size + 2, YELLOW);
}

void DrawShield(const Shield& shield) {
    float alpha = (shield.duration / 1.5f) * 150;
    Color shieldColor = {100, 150, 255, (unsigned char)alpha};
    DrawCircleLines((int)shield.pos.x, (int)shield.pos.y, (int)shield.radius, {100, 200, 255, 255});
    DrawRing(shield.pos, shield.radius - 2, shield.radius + 2, 0, 360, 32, shieldColor);
}

void DrawParticle(const Particle& p) {
    Color c = p.color;
    c.a = (unsigned char)(p.life * 255);
    DrawCircle((int)p.pos.x, (int)p.pos.y, 2, c);
}

void DrawGame() {
    BeginDrawing();
    ClearBackground({18, 24, 36, 255});

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {21, 34, 58, 255}, {10, 16, 28, 255});
    DrawCircleGradient(180, 120, 180, {60, 110, 170, 80}, {0, 0, 0, 0});
    DrawCircleGradient(WINDOW_WIDTH - 160, 140, 200, {180, 90, 70, 70}, {0, 0, 0, 0});

    // Apply screen shake
    if (level.screenShakeDuration > 0) {
        float shake = level.screenShakeAmount * level.screenShakeDuration / 0.1f;
        Camera2D camera = {0};
        camera.target = {0.0f, 0.0f};
        camera.offset = {(float)GetRandomValue((int)-shake, (int)shake),
                         (float)GetRandomValue((int)-shake, (int)shake)};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
        BeginMode2D(camera);
    }

    // Draw background elements
    DrawRectangle(WINDOW_WIDTH / 2 - 2, 0, 4, WINDOW_HEIGHT, {255, 255, 255, 45});
    DrawRectangleLines(WINDOW_WIDTH / 2 - 8, 0, 16, WINDOW_HEIGHT, {255, 255, 255, 25});

    // Draw game objects
    for (const auto& shield : level.shields) {
        DrawShield(shield);
    }

    for (const auto& proj : level.projectiles) {
        DrawProjectile(proj);
    }

    for (const auto& p : level.particles) {
        DrawParticle(p);
    }

    // Draw drawn path
    for (size_t i = 0; i + 1 < level.drawnPath.size(); ++i) {
        DrawLineEx(level.drawnPath[i], level.drawnPath[i + 1], 3, BLUE);
    }

    DrawPlayer(level.p1);
    DrawPlayer(level.p2);

    if (level.screenShakeDuration > 0) {
        EndMode2D();
    }

    // UI
    DrawRectangleRounded({16, 14, 420, 70}, 0.25f, 8, {0, 0, 0, 90});
    DrawText("P1 - Mouse Gestures", 28, 24, 20, SKYBLUE);
    DrawText(playerVsAI ? "P2 - AI DUELIST" : "P2 - Arrow + Q (Projectile)", 28, 48, 18, {255, 180, 160, 255});

    std::string skill1 = "Recognized: ";
    switch (level.p1.lastSkill) {
        case SKILL_PROJECTILE:
            skill1 += "Projectile";
            break;
        case SKILL_SHIELD:
            skill1 += "Shield";
            break;
        case SKILL_DASH:
            skill1 += "Dash";
            break;
        case SKILL_LIGHTNING:
            skill1 += "Lightning";
            break;
        case SKILL_WHIRLWIND:
            skill1 += "Whirlwind";
            break;
        default:
            skill1 += "None";
    }
    DrawRectangleRounded({14, WINDOW_HEIGHT - 82, 640, 66}, 0.22f, 8, {0, 0, 0, 95});
    DrawText(skill1.c_str(), 24, WINDOW_HEIGHT - 68, 18, SKYBLUE);
    DrawText("R - Menu | SPACE (Game Over) Restart | Contact = HP damage", 24, WINDOW_HEIGHT - 42, 16,
             RAYWHITE);

    EndDrawing();
}

void DrawMenu() {
    BeginDrawing();
    ClearBackground({11, 16, 30, 255});

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {34, 62, 96, 255}, {8, 12, 24, 255});
    float pulse = 0.5f + 0.5f * std::sin((float)GetTime() * 1.6f);
    DrawCircleGradient(150, 140, 260, {120, 180, 255, (unsigned char)(42 + pulse * 36)}, {0, 0, 0, 0});
    DrawCircleGradient(WINDOW_WIDTH - 140, 140, 240, {255, 130, 120, (unsigned char)(34 + pulse * 40)},
                       {0, 0, 0, 0});

    DrawRectangleRounded({60, 44, WINDOW_WIDTH - 120, 94}, 0.28f, 12, {6, 10, 20, 165});
    DrawText("DUEL SKETCH FIGHTERS", 86, 68, 54, {245, 236, 214, 255});
    DrawText("Gesture magic + sword combat", 88, 120, 24, {170, 208, 255, 255});

    DrawRectangleRounded({60, 170, 500, 340}, 0.18f, 10, {8, 15, 32, 170});
    DrawText("Gesture Skills", 92, 194, 34, {253, 219, 126, 255});
    DrawText("P1 Gesture Skill -> Dash", 92, 246, 26, RAYWHITE);
    DrawText("Draw any valid gesture to dash", 92, 286, 22, {210, 230, 255, 255});
    DrawText("P2 Skill -> Projectile (Q key)", 92, 330, 24, RAYWHITE);
    DrawText("Touching each other causes HP loss", 92, 372, 22, {255, 190, 140, 255});
    DrawText("Get in, hit, get out", 92, 412, 22, {255, 160, 150, 255});

    DrawRectangleRounded({590, 170, 550, 340}, 0.18f, 10, {8, 15, 32, 170});
    DrawText("Modes & Controls", 622, 194, 34, {170, 223, 255, 255});
    DrawText("Press 1  -> PvP", 622, 246, 26, RAYWHITE);
    DrawText("Press 2  -> Versus AI", 622, 284, 26, RAYWHITE);
    DrawText("P2 Move  -> Arrow Keys", 622, 328, 22, RAYWHITE);
    DrawText("P2 Skill -> Q (Projectile)", 622, 360, 22, RAYWHITE);
    DrawText("R        -> Back to menu", 622, 392, 22, RAYWHITE);

    if (menuPreview.id > 0) {
        Rectangle src = {0, 0, (float)(menuPreview.width / 10), (float)menuPreview.height};
        Rectangle dst = {915, 376, 200, 120};
        DrawTexturePro(menuPreview, src, dst, {0, 0}, 0.0f, {255, 255, 255, 220});
        DrawRectangleLinesEx({911, 372, 208, 128}, 2.0f, {255, 240, 200, 170});
    }

    DrawRectangleRounded({250, 548, 300, 76}, 0.25f, 8, {45, 104, 192, 255});
    DrawRectangleLinesEx({250, 548, 300, 76}, 3.0f, {180, 220, 255, 220});
    DrawText("START PVP (1)", 298, 574, 28, WHITE);

    DrawRectangleRounded({650, 548, 300, 76}, 0.25f, 8, {185, 74, 66, 255});
    DrawRectangleLinesEx({650, 548, 300, 76}, 3.0f, {255, 196, 188, 220});
    DrawText("START VS AI (2)", 680, 574, 28, WHITE);

    EndDrawing();
}

void DrawGameOver() {
    BeginDrawing();
    ClearBackground({8, 12, 22, 255});

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {40, 22, 36, 255}, {8, 12, 22, 255});
    DrawRectangleRounded({170, 110, WINDOW_WIDTH - 340, WINDOW_HEIGHT - 220}, 0.2f, 12, {0, 0, 0, 170});
    DrawRectangleLinesEx({170, 110, WINDOW_WIDTH - 340, WINDOW_HEIGHT - 220}, 3.0f, {255, 255, 255, 40});

    std::string text = (winner == 0) ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";
    Color winColor = (winner == 0) ? SKYBLUE : Color{255, 120, 110, 255};

    DrawText("BATTLE RESULT", WINDOW_WIDTH / 2 - 170, 180, 46, {244, 225, 178, 255});
    DrawText(text.c_str(), WINDOW_WIDTH / 2 - 210, WINDOW_HEIGHT / 2 - 20, 64, winColor);
    DrawText("SPACE - Instant Rematch", WINDOW_WIDTH / 2 - 170, WINDOW_HEIGHT / 2 + 90, 30, RAYWHITE);
    DrawText("M - Return to Main Menu", WINDOW_WIDTH / 2 - 165, WINDOW_HEIGHT / 2 + 132, 26, {200, 212, 230, 255});

    EndDrawing();
}

// ============================================================================
// Game Loop
// ============================================================================

void InitializeGame() {
    UnloadPlayerSprites(level.p1);
    UnloadPlayerSprites(level.p2);

    level.p1.pos = {200, WINDOW_HEIGHT / 2};
    level.p1.vel = {0, 0};
    level.p1.width = 140;
    level.p1.height = 140;
    level.p1.maxHP = 100;
    level.p1.hp = 100;
    level.p1.speed = 250;
    level.p1.skillCooldown = 0;
    level.p1.side = SIDE_LEFT;
    level.p1.lastSkill = SKILL_NONE;
    level.p1.isAI = false;
    level.p1.animState = ANIM_IDLE;
    level.p1.animFrame = 0;
    level.p1.animTimer = 0;
    level.p1.attackAnimTimer = 0;
    level.p1.hurtAnimTimer = 0;
    level.p1.contactDamageCooldown = 0;
    LoadPlayerSprites(level.p1);

    level.p2.pos = {WINDOW_WIDTH - 200, WINDOW_HEIGHT / 2};
    level.p2.vel = {0, 0};
    level.p2.width = 140;
    level.p2.height = 140;
    level.p2.maxHP = 100;
    level.p2.hp = 100;
    level.p2.speed = 250;
    level.p2.skillCooldown = 0;
    level.p2.side = SIDE_RIGHT;
    level.p2.lastSkill = SKILL_NONE;
    level.p2.isAI = playerVsAI;
    level.p2.animState = ANIM_IDLE;
    level.p2.animFrame = 0;
    level.p2.animTimer = 0;
    level.p2.attackAnimTimer = 0;
    level.p2.hurtAnimTimer = 0;
    level.p2.contactDamageCooldown = 0;
    LoadPlayerSprites(level.p2);

    level.projectiles.clear();
    level.shields.clear();
    level.particles.clear();
    level.drawnPath.clear();
    level.screenShakeAmount = 0;
    level.screenShakeDuration = 0;

    winner = -1;
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Duel Sketch Fighters");
    SetTargetFPS(60);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (FileExists(P1_PREVIEW_SPRITE)) {
        menuPreview = LoadTexture(P1_PREVIEW_SPRITE);
    }

    InitializeGame();
    gameState = STATE_MENU;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.016f) dt = 0.016f;

        HandleInput();
        Update(dt);

        if (gameState == STATE_MENU) {
            DrawMenu();
        } else if (gameState == STATE_PLAYING_PVP || gameState == STATE_PLAYING_PVE) {
            DrawGame();
        } else if (gameState == STATE_GAME_OVER) {
            DrawGameOver();
        }
    }

    UnloadPlayerSprites(level.p1);
    UnloadPlayerSprites(level.p2);
    if (menuPreview.id > 0) {
        UnloadTexture(menuPreview);
        menuPreview = {0};
    }

    CloseWindow();
    return 0;
}
