#include "HUD.h"
#include "../../Game.h"
#include "../../GameConstants.h"
#include "../../Actors/Characters/ShadowCat.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName, int maxHealth)
    :UIScreen(game, fontName),
    mMaxHealth(maxHealth),
    mHealth(maxHealth)
{   
    // Main health icons, top left
    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = mMaxHealth;

    InitHealthIcons();

    // Cursor
    mCursorImage = AddImage("../Assets/Icons/Cursor.png", Vector2(0.0f, 0.0f), 1.0f, 0.0f, 20000);

    // Enemy counter top right
    AddText("Enemies Left:", Vector2(500.0f, -300.0f), 0.7f);
    mEnemiesLeftCount = AddText("0", Vector2(600.0f, -300.0f), 0.7f);

    mAreaClearTxt = AddText("            Area clear!\nProceed to the red carpet!", Vector2(630.0f, -220.0f), 0.5f);

    for (auto &txt : mTexts) {
        txt->SetTextColor(Vector3::One); // White
        txt->SetBackgroundColor(Vector4::Zero); // Transparent
    }

    // Initialize boss health bar (initially hidden)
    InitBossHealthBar();
}

void HUD::Update(float deltaTime)
{
    // Update cursor pos  ------------------- //
    Vector2 mousePos = mGame->GetMouseAbsolutePosition();

    // Rotate cursor based on deviation from center
    Vector2 center(GameConstants::WINDOW_WIDTH / 2.0f, GameConstants::WINDOW_HEIGHT / 2.0f);
    Vector2 dir = mousePos - center;

    float angle = Math::Atan2(dir.y, dir.x) + Math::PiOver2;
    mCursorImage->SetAngle(angle);
    mCursorImage->SetAbsolutePos(mousePos);

    // Update enemies left  ------------------- //
    int enemiesLeft = mGame->CountAliveEnemies();
    mEnemiesLeftCount->SetText(std::to_string(enemiesLeft));
    if (enemiesLeft == 0)
        mAreaClearTxt->SetIsVisible(true);
    else
        mAreaClearTxt->SetIsVisible(false);

    // Update health ------------------- //
    if (!mGame->GetPlayer()) return;

    // If new max hp update otherwise just update health
    if (mGame->GetPlayer()->GetMaxHP() != mMaxHealth) {
        UpdateMaxHealth(mGame->GetPlayer()->GetMaxHP() / 10, true);
    }

    SetHealth(mGame->GetPlayer()->GetHP() / 10);

    // Update boss health bar
    UpdateBossHealth();
}

void HUD::InitHealthIcons() {
    if (mMaxHealth % 2 != 0) {
        mMaxHealth++; // Ensure max health is even for heart icons
        mHealth++;
    }

    // Delete any existing icons
    for (auto img : mFullHeartIcons) delete img;
    for (auto img : mHalfHeartIcons) delete img;
    for (auto img : mEmptyHeartIcons) delete img;

    mFullHeartIcons.clear();
    mHalfHeartIcons.clear();
    mEmptyHeartIcons.clear();

    // Create new icons based on mMaxHealth
    const float SCALE = 1.0f;
    const float SPACING = 35.0f * SCALE;
    const float VERTICAL_SPACING = 30.0f;

    // Draw two equal rows if more than 10 health
    int rows = (mMaxHealth > 10) ? 2 : 1;

    for (int i = 0; i < mMaxHealth / 2; ++i) {
        int row = i > mMaxHealth / 4 - 1 ? 1 : 0;
        Vector2 offset(-580.0f + (i - row * (mMaxHealth / 4)) * SPACING, +300.0f - row * VERTICAL_SPACING);

        UIImage* emptyHeart = AddImage("../Assets/Icons/LifeBar_2.png", offset, SCALE, 0.0f, 1);
        UIImage* halfHeart = AddImage("../Assets/Icons/LifeBar_1.png", offset, SCALE, 0.0f, 2);
        UIImage* fullHeart = AddImage("../Assets/Icons/LifeBar_0.png", offset, SCALE, 0.0f, 3);

        mEmptyHeartIcons.push_back(emptyHeart);
        mHalfHeartIcons.push_back(halfHeart);
        mFullHeartIcons.push_back(fullHeart);
    }

    SetHealth(mHealth);
}

void HUD::SetHealth(int health)
{
    mHealth = std::clamp(health, 0, mMaxHealth);

    // Update heart icons based on current health
    for (int i = 0; i < mMaxHealth / 2; ++i) {
        int heartHealth = (i + 1) * 2;

        if (mHealth >= heartHealth) {
            mFullHeartIcons[i]->SetIsVisible(true);
            mHalfHeartIcons[i]->SetIsVisible(false);
            mEmptyHeartIcons[i]->SetIsVisible(false);
        } else if (mHealth == heartHealth - 1) {
            mFullHeartIcons[i]->SetIsVisible(false);
            mHalfHeartIcons[i]->SetIsVisible(true);
            mEmptyHeartIcons[i]->SetIsVisible(false);
        } else {
            mFullHeartIcons[i]->SetIsVisible(false);
            mHalfHeartIcons[i]->SetIsVisible(false);
            mEmptyHeartIcons[i]->SetIsVisible(true);
        }
    }
}

void HUD::UpdateMaxHealth(int maxHealth, bool fill)
{
    mMaxHealth = maxHealth;

    mMaxHealth = std::max(2, mMaxHealth); // Ensure at least 2
    mHealth = fill ? mMaxHealth : std::min(mHealth, mMaxHealth);

    InitHealthIcons();
}

void HUD::InitBossHealthBar()
{
    // Boss health bar at top center of screen
    // Total: 40 bars (12 left + 16 middle + 12 right), each bar = 2 HP
    // Initial HP: 80
    
    float scale = 5.0f;
    float yPos = -330.0f; // Top center Y position
    
    // All images are 64x64 pixels
    float imageWidth = 64.0f * scale;
    
    // Calculate total width and starting position (left edge)
    float totalWidth = imageWidth * 3.0f; // 3 images of 64 pixels each
    float leftEdge = -totalWidth / 2.0f; // Left edge of entire bar
    
    // Position each image so it starts exactly where the previous one ends
    // Since images are positioned by their center, we calculate:
    // Image center = leftEdge + (imageIndex * imageWidth) + (imageWidth / 2)
    
    // Left image (index 0): center at -96 + 0*64 + 32 = -64 (spans from -96 to -32)
    Vector2 leftPos(leftEdge + imageWidth / 2.0f, yPos);
    
    // Middle image (index 1): center at -96 + 1*64 + 32 = -32 (spans from -32 to 0)
    Vector2 middlePos(leftEdge + imageWidth + imageWidth / 2.0f, yPos);
    
    // Right image (index 2): center at -96 + 2*64 + 32 = 0 (spans from 0 to 32)
    Vector2 rightPos(leftEdge + imageWidth * 2.0f + imageWidth / 2.0f, yPos);
    
    // Create individual BossLife_3.png segments FIRST (40 total, each represents 2 HP)
    // These appear behind the main bars (drawOrder 5, lower than main bars at 10)
    // Start 20px to the right from the beginning of BossLife_0.png, then every 4px
    // Apply scale to all spacing and positioning calculations
    float segmentSpacing = 4.0f * scale; // Spacing between each segment (4px scaled)
    
    // BossLife_0.png's left edge is at leftEdge (e.g., -96 for a centered bar)
    // The center of BossLife_0.png is at leftPos.x = leftEdge + imageWidth/2 = -96 + 32 = -64
    // To start 20px to the RIGHT of the left edge, we use: leftEdge + (20 * scale)
    // Since leftEdge is negative (-96), leftEdge + (20 * scale) moves to the right
    float bossLife0LeftEdge = leftEdge; // Left edge of BossLife_0.png
    float firstSegmentLeftEdge = bossLife0LeftEdge + (50.0f * scale); // 20px to the RIGHT of BossLife_0 left edge (scaled)
    
    // We'll need to get the actual texture width, but for now assume it's 4px
    // The center of the first segment should be at: leftEdge + (20 * scale) + (segmentWidth/2)
    // Since spacing is 4px, we assume segment width is also 4px (scaled)
    float segmentWidth = 4.0f * scale; // Width of each BossLife_3 segment (scaled)
    float firstSegmentCenterX = firstSegmentLeftEdge + segmentWidth / 2.0f;
    
    mBossHealthSegments.clear();
    for (int i = 0; i < 38; ++i)
    {
        // Each segment center is positioned at: firstSegmentCenterX + (i * segmentSpacing)
        float segmentCenterX = firstSegmentCenterX + (i * segmentSpacing);
        Vector2 segmentPos(segmentCenterX, yPos);
        UIImage* segment = AddImage("../Assets/HUD/BossFight/BossLife_3.png", segmentPos, scale, 0.0f, 5);
        segment->SetIsVisible(false); // Initially hidden
        mBossHealthSegments.push_back(segment);
    }
    
    // Create main bar segments AFTER BossLife_3 segments (so they appear on top)
    // Left segment (BossLife_0.png) - 12 bars
    mBossHealthLeft = AddImage("../Assets/HUD/BossFight/BossLife_0.png", leftPos, scale, 0.0f, 10);
    
    // Middle segment (BossLife_1.png) - 16 bars
    mBossHealthMiddle = AddImage("../Assets/HUD/BossFight/BossLife_1.png", middlePos, scale, 0.0f, 10);
    
    // Right segment (BossLife_2.png) - 12 bars
    mBossHealthRight = AddImage("../Assets/HUD/BossFight/BossLife_2.png", rightPos, scale, 0.0f, 10);
    
    // Initially hide all boss health elements
    if (mBossHealthLeft) mBossHealthLeft->SetIsVisible(false);
    if (mBossHealthMiddle) mBossHealthMiddle->SetIsVisible(false);
    if (mBossHealthRight) mBossHealthRight->SetIsVisible(false);
}

void HUD::UpdateBossHealth()
{
    auto boss = mGame->GetCurrentBoss();
    
    if (!boss || boss->IsDead())
    {
        // Hide boss health bar if no boss or boss is dead
        if (mBossHealthLeft) mBossHealthLeft->SetIsVisible(false);
        if (mBossHealthMiddle) mBossHealthMiddle->SetIsVisible(false);
        if (mBossHealthRight) mBossHealthRight->SetIsVisible(false);
        for (auto segment : mBossHealthSegments)
        {
            if (segment) segment->SetIsVisible(false);
        }
        return;
    }
    
    // Show boss health bar
    int currentHP = boss->GetHP();
    int maxHP = boss->GetMaxHP();
    
    // Each segment represents 2 HP
    // Total segments: 40 (12 left + 16 middle + 12 right)
    int totalSegments = 40;
    int visibleSegments = currentHP / 2; // Each segment = 2 HP
    visibleSegments = std::clamp(visibleSegments, 0, totalSegments);
    
    // Update individual BossLife_3 segments visibility
    for (int i = 0; i < mBossHealthSegments.size(); ++i)
    {
        if (mBossHealthSegments[i])
        {
            mBossHealthSegments[i]->SetIsVisible(i < visibleSegments);
        }
    }
    
    // Calculate which main bar segments should be visible
    // Left: segments 1-12
    // Middle: segments 13-28 (12+1 to 12+16)
    // Right: segments 29-40 (28+1 to 40)
    
    bool showLeft = visibleSegments > 0;
    bool showMiddle = visibleSegments > 12;
    bool showRight = visibleSegments > 28;
    
    if (mBossHealthLeft)
    {
        mBossHealthLeft->SetIsVisible(showLeft);
    }
    
    if (mBossHealthMiddle)
    {
        mBossHealthMiddle->SetIsVisible(showMiddle);
    }
    
    if (mBossHealthRight)
    {
        mBossHealthRight->SetIsVisible(showRight);
    }
}
