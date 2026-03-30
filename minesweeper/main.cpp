#include "raylib.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

struct Cell {
    bool mine = false;
    bool open = false;
    bool flag = false;
    unsigned char n = 0;
};

struct Level {
    const char* name;
    int cols;
    int rows;
    int mines;
    int cell;
};

static constexpr Level kLevels[3] = {
    {"Easy", 9, 9, 10, 58},
    {"Medium", 16, 16, 40, 46},
    {"Hard", 30, 16, 99, 34},
};

static Color NumColor(int n) {
    switch (n) {
        case 1: return (Color){ 25, 118, 210, 255 };   // blue
        case 2: return (Color){ 56, 142, 60, 255 };    // green
        case 3: return (Color){ 211, 47, 47, 255 };    // red
        case 4: return (Color){ 13, 71, 161, 255 };
        case 5: return (Color){ 121, 85, 72, 255 };
        case 6: return (Color){ 0, 121, 107, 255 };
        case 7: return BLACK;
        case 8: return DARKGRAY;
        default: return BLACK;
    }
}

static Color RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    return (Color){r, g, b, a};
}

static constexpr Color kBg = {246, 248, 252, 255};
static constexpr Color kCard = {250, 251, 253, 255};
static constexpr Color kCard2 = {241, 244, 249, 255};
static constexpr Color kBorder = {0, 0, 0, 28};
static constexpr Color kText = {35, 45, 60, 255};
static constexpr Color kText2 = {90, 100, 115, 255};
static constexpr Color kAccent = {46, 125, 231, 255};
static constexpr Color kDanger = {211, 47, 47, 255};
static constexpr Color kSuccess = {56, 142, 60, 255};

static void DrawShadowRounded(Rectangle r, float roundness, int layers, int spread) {
    for (int i = 0; i < layers; i++) {
        int pad = spread + i;
        Rectangle s = {r.x - pad, r.y - pad, r.width + pad * 2.0f, r.height + pad * 2.0f};
        unsigned char a = (unsigned char)std::max(0, 55 - i * 12);
        DrawRectangleRounded(s, roundness, 10, RGBA(0, 0, 0, a));
    }
}

static void DrawCard(Rectangle r, Color fill, float roundness = 0.18f) {
    DrawShadowRounded(r, roundness, 3, 2);
    DrawRectangleRounded(r, roundness, 10, fill);
    DrawRectangleRoundedLines(r, roundness, 10, kBorder);
}

static void DrawPill(Rectangle r, Color fill, Color outline) {
    float roundness = 0.5f;
    DrawRectangleRounded(r, roundness, 10, fill);
    DrawRectangleRoundedLines(r, roundness, 10, outline);
}

static void DrawTile(Rectangle r, bool open) {
    float rr = 0.18f;
    // Soft drop shadow
    DrawRectangleRounded({r.x + 1, r.y + 2, r.width, r.height}, rr, 6, RGBA(0, 0, 0, 18));

    Color fill = open ? RGBA(255, 255, 255) : RGBA(231, 236, 245);
    DrawRectangleRounded(r, rr, 6, fill);

    if (!open) {
        // Gentle top highlight
        Rectangle hi = {r.x + 1, r.y + 1, r.width - 2, r.height * 0.48f};
        DrawRectangleRounded(hi, rr, 6, RGBA(255, 255, 255, 140));
        // Slight bottom shade
        Rectangle lo = {r.x + 1, r.y + r.height * 0.62f, r.width - 2, r.height * 0.36f};
        DrawRectangleRounded(lo, rr, 6, RGBA(0, 0, 0, 10));
    }

    DrawRectangleRoundedLines(r, rr, 6, kBorder);
}

struct Game {
    // Layout
    int margin = 28;
    int top = 104;

    // Level
    int levelIdx = 1;
    Level level = {"Medium", 16, 16, 40, 46};

    // Menu
    bool inMenu = true;

    // State
    std::vector<Cell> a;
    bool generated = false;
    bool over = false;
    bool win = false;
    int exploded = -1;

    int W() const { return level.cols * level.cell + margin * 2; }
    int H() const { return level.rows * level.cell + top + margin; }

    int idx(int x, int y) const { return y * level.cols + x; }
    bool in(int x, int y) const { return x >= 0 && x < level.cols && y >= 0 && y < level.rows; }
    Rectangle board() const {
        return {(float)margin, (float)top, (float)(level.cols * level.cell), (float)(level.rows * level.cell)};
    }

    Rectangle nextRect() const {
        float w = 76.0f;
        float h = 26.0f;
        return {(float)(W() - margin - w), 10.0f, w, h};
    }

    Rectangle restartRect() const {
        float w = 34.0f;
        float h = 26.0f;
        return {(float)(W() - margin - w), 46.0f, w, h};
    }

    Rectangle minesRect() const {
        return {(float)margin, 46.0f, 132.0f, 26.0f};
    }

    Rectangle levelRect(int i) const {
        Rectangle nr = nextRect();
        float y = 10.0f;
        float h = 26.0f;
        float left = (float)margin;
        float right = nr.x - 10.0f;
        float available = std::max(0.0f, right - left);

        float w = (available - 2.0f * 10.0f) / 3.0f;
        w = std::clamp(w, 62.0f, 96.0f);
        float gap = (available - 3.0f * w) / 2.0f;
        gap = std::clamp(gap, 6.0f, 14.0f);

        float x = left + i * (w + gap);
        return {x, y, w, h};
    }

    void applyLevel(const Level& lv) {
        level = lv;
        a.assign(level.cols * level.rows, Cell{});
        generated = over = win = false;
        exploded = -1;
        inMenu = false;
        SetWindowSize(W(), H());
    }

    void reset() {
        for (auto& c : a) c = Cell{};
        generated = over = win = false;
        exploded = -1;
    }

    void goMenu() {
        inMenu = true;
        generated = over = win = false;
        exploded = -1;
        for (auto& c : a) c = Cell{};
    }

    int flags() const {
        int f = 0;
        for (auto& c : a) f += c.flag ? 1 : 0;
        return f;
    }

    int openedSafe() const {
        int r = 0;
        for (auto& c : a) r += (!c.mine && c.open) ? 1 : 0;
        return r;
    }

    void computeNumbers() {
        for (int y = 0; y < level.rows; y++) {
            for (int x = 0; x < level.cols; x++) {
                Cell& c = a[idx(x, y)];
                if (c.mine) { c.n = 0; continue; }
                int cnt = 0;
                for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++) {
                    if (!dx && !dy) continue;
                    int nx = x + dx, ny = y + dy;
                    if (in(nx, ny) && a[idx(nx, ny)].mine) cnt++;
                }
                c.n = (unsigned char)cnt;
            }
        }
    }

    void generate(int sx, int sy) {
        // Clear
        for (auto& c : a) c = Cell{};

        std::vector<int> cand;
        cand.reserve(level.cols * level.rows);
        for (int y = 0; y < level.rows; y++) {
            for (int x = 0; x < level.cols; x++) {
                // Keep a 3x3 safe zone around first click for nicer starts.
                if (std::abs(x - sx) <= 1 && std::abs(y - sy) <= 1) continue;
                cand.push_back(idx(x, y));
            }
        }
        for (int i = (int)cand.size() - 1; i > 0; i--) {
            int j = GetRandomValue(0, i);
            std::swap(cand[i], cand[j]);
        }
        int m = std::min(level.mines, (int)cand.size());
        for (int i = 0; i < m; i++) a[cand[i]].mine = true;
        computeNumbers();
        generated = true;
    }

    void floodOpen(int sx, int sy) {
        std::vector<int> st;
        st.reserve(level.cols * level.rows);
        st.push_back(idx(sx, sy));

        while (!st.empty()) {
            int id = st.back();
            st.pop_back();
            int x = id % level.cols;
            int y = id / level.cols;
            Cell& c = a[id];
            if (c.open || c.flag) continue;
            c.open = true;
            if (c.n != 0) continue;

            for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++) {
                if (!dx && !dy) continue;
                int nx = x + dx, ny = y + dy;
                if (!in(nx, ny)) continue;
                Cell& nc = a[idx(nx, ny)];
                if (nc.open || nc.flag || nc.mine) continue;
                st.push_back(idx(nx, ny));
            }
        }
    }

    void click(int x, int y, int btn) {
        if (!in(x, y) || over || win) return;
        Cell& c = a[idx(x, y)];

        if (btn == MOUSE_BUTTON_RIGHT) {
            if (!c.open) c.flag = !c.flag;
            return;
        }

        if (btn != MOUSE_BUTTON_LEFT) return;
        if (c.flag) return;

        if (!generated) generate(x, y);

        if (c.mine) {
            over = true;
            exploded = idx(x, y);
            for (auto& t : a) if (t.mine) t.open = true;
            return;
        }

        floodOpen(x, y);
        if (openedSafe() == level.cols * level.rows - level.mines) {
            win = true;
            for (auto& t : a) if (t.mine) t.flag = true;
        }
    }

    void draw(Vector2 mouse) {
        ClearBackground(kBg);

        Rectangle topBar = {0, 0, (float)W(), (float)top};
        DrawRectangleGradientV(0, 0, W(), top, kCard2, kBg);
        DrawLine(0, top - 1, W(), top - 1, kBorder);

        // Left: mines left
        int minesLeft = level.mines - flags();
        Rectangle minesPill = minesRect();
        DrawPill(minesPill, RGBA(255, 255, 255, 220), kBorder);
        std::string left = "Mines  " + std::to_string(minesLeft);
        DrawText(left.c_str(), (int)minesPill.x + 12, (int)minesPill.y + 5, 18, kText);

        // Middle: status
        const char* status = (!generated ? "Click to start" : (over ? "BOOM" : (win ? "YOU WIN" : "")));
        Color sc = over ? kDanger : (win ? kSuccess : kText2);
        int sw = MeasureText(status, 22);
        DrawText(status, (W() - sw) / 2, 48, 22, sc);

        // Right: Next button + restart hint
           Rectangle nr = nextRect();
           bool nextHot = CheckCollisionPointRec(mouse, nr);
           DrawPill(nr, nextHot ? RGBA(255, 255, 255, 245) : RGBA(255, 255, 255, 220), kBorder);
           const char* nextLabel = "Next";
           DrawText(nextLabel,
                  (int)(nr.x + (nr.width - MeasureText(nextLabel, 18)) / 2),
                  (int)(nr.y + 5),
                  18,
                  kText);

        Rectangle rPill = restartRect();
        DrawPill(rPill, RGBA(255, 255, 255, 220), kBorder);
        DrawText("R", (int)rPill.x + 11, (int)rPill.y + 5, 18, kText2);

        // Level buttons
        for (int i = 0; i < 3; i++) {
            Rectangle br = levelRect(i);
            bool hot = CheckCollisionPointRec(mouse, br);
            bool active = (i == levelIdx);
            Color base = active ? RGBA(kAccent.r, kAccent.g, kAccent.b, 35) : (hot ? RGBA(255, 255, 255, 245) : RGBA(255, 255, 255, 210));
            DrawPill(br, base, kBorder);
            std::string label = std::string((i == 0) ? "1 " : (i == 1) ? "2 " : "3 ") + kLevels[i].name;
            DrawText(label.c_str(), (int)br.x + 10, (int)br.y + 5, 18, active ? kAccent : kText2);
        }

        Rectangle b = board();
        DrawCard({b.x - 12, b.y - 12, b.width + 24, b.height + 24}, kCard2, 0.14f);

        int hx = -1, hy = -1;
        if (CheckCollisionPointRec(mouse, b)) {
            hx = (int)((mouse.x - b.x) / level.cell);
            hy = (int)((mouse.y - b.y) / level.cell);
            if (!in(hx, hy)) { hx = -1; hy = -1; }
        }

        for (int y = 0; y < level.rows; y++) {
            for (int x = 0; x < level.cols; x++) {
                Cell& c = a[idx(x, y)];
                Rectangle r = {b.x + x * level.cell, b.y + y * level.cell, (float)level.cell, (float)level.cell};
                Rectangle tr = {r.x + 3, r.y + 3, r.width - 6, r.height - 6};
                DrawTile(tr, c.open);

                if (c.open) {
                    if (c.mine) {
                        Vector2 cc = {tr.x + tr.width * 0.5f, tr.y + tr.height * 0.5f};
                        DrawCircleV(cc, tr.width * 0.22f, RGBA(15, 18, 25));
                        DrawCircleLines((int)cc.x, (int)cc.y, tr.width * 0.22f, RGBA(255, 255, 255, 90));
                        if (idx(x, y) == exploded) {
                            DrawRectangleRoundedLines(tr, 0.15f, 6, kDanger);
                        }
                    } else if (c.n) {
                        std::string t(1, char('0' + c.n));
                        int fs = (int)(tr.width * 0.68f);
                        int tw = MeasureText(t.c_str(), fs);
                        DrawText(t.c_str(), (int)(tr.x + (tr.width - tw) / 2), (int)(tr.y + (tr.height - fs) / 2 - 1), fs, NumColor(c.n));
                    }
                } else if (c.flag) {
                    // Flag
                    int px = (int)tr.x;
                    int py = (int)tr.y;
                    int cs = (int)tr.width;
                    DrawLine(px + cs / 2 - 6, py + cs / 2 + 10, px + cs / 2 - 6, py + cs / 2 - 10, RGBA(90, 100, 115));
                    DrawTriangle(
                        (Vector2){(float)(px + cs / 2 - 6), (float)(py + cs / 2 - 10)},
                        (Vector2){(float)(px + cs / 2 - 6), (float)(py + cs / 2 + 2)},
                        (Vector2){(float)(px + cs / 2 + 10), (float)(py + cs / 2 - 4)},
                        kDanger);
                }

                if (!over && !win && x == hx && y == hy) {
                    DrawRectangleRoundedLines(tr, 0.15f, 6, RGBA(0, 0, 0, 35));
                }
            }
        }

        if (inMenu) {
            Rectangle overlay = {0, 0, (float)W(), (float)H()};
            DrawRectangleRec(overlay, RGBA(10, 15, 25, 110));

            Rectangle card = {(float)(W() / 2 - 170), (float)(H() / 2 - 120), 340.0f, 240.0f};
            DrawCard(card, kCard, 0.16f);

            const char* title = "Select Level";
            int tw = MeasureText(title, 28);
            DrawText(title, (int)(card.x + (card.width - tw) / 2), (int)card.y + 18, 28, kText);

            const char* sub = "Press 1/2/3 or click";
            int sw2 = MeasureText(sub, 18);
            DrawText(sub, (int)(card.x + (card.width - sw2) / 2), (int)card.y + 54, 18, kText2);

            int btnW = 220;
            int btnH = 40;
            int cx = (int)(card.x + (card.width - btnW) / 2);
            int cy = (int)card.y + 90;
            for (int i = 0; i < 3; i++) {
                Rectangle br = {(float)cx, (float)(cy + i * (btnH + 12)), (float)btnW, (float)btnH};
                bool hot = CheckCollisionPointRec(mouse, br);
                Color base = hot ? RGBA(255, 255, 255, 245) : RGBA(255, 255, 255, 210);
                DrawCard(br, base, 0.25f);
                std::string label = std::string((i == 0) ? "1 - " : (i == 1) ? "2 - " : "3 - ") + kLevels[i].name;
                int lw = MeasureText(label.c_str(), 20);
                DrawText(label.c_str(), (int)(br.x + (br.width - lw) / 2), (int)(br.y + 10), 20, kText);
            }

            const char* tip = "ESC: back to menu";
            DrawText(tip, (int)card.x + 18, (int)(card.y + card.height - 26), 16, kText2);
        }
    }
};

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1000, 750, "Minesweeper");
    SetTargetFPS(60);

    Game g;
    g.levelIdx = 1;
    g.applyLevel(kLevels[g.levelIdx]);
    g.goMenu();

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (IsKeyPressed(KEY_ESCAPE)) g.goMenu();
        if (IsKeyPressed(KEY_R) && !g.inMenu) g.reset();

        if (IsKeyPressed(KEY_ONE)) { g.levelIdx = 0; g.applyLevel(kLevels[0]); }
        if (IsKeyPressed(KEY_TWO)) { g.levelIdx = 1; g.applyLevel(kLevels[1]); }
        if (IsKeyPressed(KEY_THREE)) { g.levelIdx = 2; g.applyLevel(kLevels[2]); }

        // Click level buttons (top bar)
        {
            for (int i = 0; i < 3; i++) {
                Rectangle br = g.levelRect(i);
                if (CheckCollisionPointRec(mouse, br) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    g.levelIdx = i;
                    g.applyLevel(kLevels[i]);
                }
            }
        }

        // Next button (cycle levels)
        if (!g.inMenu && CheckCollisionPointRec(mouse, g.nextRect()) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            g.levelIdx = (g.levelIdx + 1) % 3;
            g.applyLevel(kLevels[g.levelIdx]);
        }

        // Menu clicks (center card)
        if (g.inMenu && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Rectangle card = {(float)(g.W() / 2 - 170), (float)(g.H() / 2 - 120), 340.0f, 240.0f};
            int btnW = 220;
            int btnH = 40;
            int cx = (int)(card.x + (card.width - btnW) / 2);
            int cy = (int)card.y + 90;
            for (int i = 0; i < 3; i++) {
                Rectangle br = {(float)cx, (float)(cy + i * (btnH + 12)), (float)btnW, (float)btnH};
                if (CheckCollisionPointRec(mouse, br)) {
                    g.levelIdx = i;
                    g.applyLevel(kLevels[i]);
                }
            }
        }

        if (!g.inMenu) {
            Rectangle b = g.board();
            if (CheckCollisionPointRec(mouse, b)) {
                int x = (int)((mouse.x - b.x) / g.level.cell);
                int y = (int)((mouse.y - b.y) / g.level.cell);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) g.click(x, y, MOUSE_BUTTON_LEFT);
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) g.click(x, y, MOUSE_BUTTON_RIGHT);
            }
        }

        BeginDrawing();
        g.draw(mouse);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
