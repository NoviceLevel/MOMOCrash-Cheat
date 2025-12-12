#pragma once
#include <iostream>
#include <vector>
#include <string>

// MOMOCrash (MomottoCrash) SDK
// Based on dump from Assembly-CSharp.dll

namespace MomottoCrash
{
    // Character Type
    enum class CharacterType : int
    {
        Daia = 0,
        Hikaru = 1,
        Seika = 2
    };

    // Game Mode
    enum class GameMode : int
    {
        None = 0,
        Story = 1,
        FreePlay = 2
    };

    // Difficulty Type
    enum class DifficultyType : int
    {
        Easy = 0,
        Normal = 1,
        Hard = 2,
        VeryHard = 3
    };

    // Note Type
    enum class NoteType : int
    {
        PressNote = 0,
        KickNote = 1,
        Cat = 2
    };

    // Music Clear Type
    enum class MusicClearType : int
    {
        NoPlay = 0,
        Failed = 1,
        Clear = 2,
        FullCombo = 3,
        Perfect = 4
    };

    // Achievement Type
    enum class AchievementType : int
    {
        None = 0,
        Steam_momo_000 = 1,
        Steam_momo_001 = 2,
        Steam_momo_002 = 3,
        Steam_momo_003 = 4
    };

    // Result Type (from MomottoCrash.Result.ResultType)
    enum class ResultType : int
    {
        Perfect = 0,
        Good = 1,
        Bad = 2,
        Miss = 3,
        MaxCombo = 4,
        Score = 5,
        Rate = 6
    };

    // Map Step Type
    enum class MapStepType : int
    {
        Seika = 0,
        Hikaru = 1,
        Daia = 2,
        Other = 3
    };
}

// Character Info Structure
struct Character
{
    std::string name;
    MomottoCrash::CharacterType type;
};

// Character List
inline std::vector<Character> Characters
{
    {"Daia", MomottoCrash::CharacterType::Daia},
    {"Hikaru", MomottoCrash::CharacterType::Hikaru},
    {"Seika", MomottoCrash::CharacterType::Seika}
};

// Difficulty Info Structure
struct Difficulty
{
    std::string name;
    MomottoCrash::DifficultyType type;
};

// Difficulty List
inline std::vector<Difficulty> Difficulties
{
    {"Easy", MomottoCrash::DifficultyType::Easy},
    {"Normal", MomottoCrash::DifficultyType::Normal},
    {"Hard", MomottoCrash::DifficultyType::Hard},
    {"VeryHard", MomottoCrash::DifficultyType::VeryHard}
};

// Game Constants (from GameConstants class)
namespace GameConstants
{
    constexpr int PERFECT_SCORE = 100;
    constexpr int GOOD_SCORE = 50;
    constexpr int BAD_SCORE = 0;
    constexpr float PERFECT_TIMING_WINDOW = 0.05f;
    constexpr float GOOD_TIMING_WINDOW = 0.1f;
    constexpr float BAD_TIMING_WINDOW = 0.15f;
    constexpr int TARGET_FRAME_RATE = 60;
    constexpr int MAX_REMAIN_SOULS = 10;
}

// ============================================
// Class Offsets (for memory modification)
// ============================================

// GameLogic Class Offsets
namespace GameLogicOffsets
{
    constexpr int _scoreRp = 0x0A8;           // UniRx.ReactiveProperty<Int32>
    constexpr int _comboRp = 0x0B0;           // UniRx.ReactiveProperty<Int32>
    constexpr int _gaugeRp = 0x0B8;           // UniRx.ReactiveProperty<Int32>
    constexpr int _maxCombo = 0x0E4;          // Int32
    constexpr int DebugAutoPlayMode = 0x0E8;  // Boolean - Auto Play Mode!
    constexpr int GaugeMaxValue = 0x0EC;      // Int32
    constexpr int _gaugeClearBorder = 0x0F0;  // Int32
    constexpr int _gaugePerfectValue = 0x0F4; // Int32
    constexpr int _gaugeGoodValue = 0x0F8;    // Int32
    constexpr int _gaugeBadValue = 0x0FC;     // Int32
    constexpr int _timingOffset = 0x100;      // Single
}

// SaveData Class Offsets
namespace SaveDataOffsets
{
    constexpr int SaveVer = 0x010;
    constexpr int _highSpeedRate = 0x0C4;     // Single - High Speed Rate
    constexpr int _timingOffset = 0x0C8;      // Single - Timing Offset
    constexpr int ScoreList = 0x060;          // List<SaveMusicResult>
    constexpr int CharacterAffinityList = 0x068; // List<SaveCharacterAffinity>
    constexpr int UnlockMusicList = 0x078;    // List<String>
}

// SaveCharacterAffinity Class Offsets
namespace SaveCharacterAffinityOffsets
{
    constexpr int CharacterName = 0x010;      // String
    constexpr int Level = 0x018;              // Int32 - Affinity Level
    constexpr int MidLevel = 0x01C;           // Int32
    constexpr int Point = 0x020;              // Int32 - Affinity Points
    constexpr int HavePeaceCount = 0x024;     // Int32 - Puzzle pieces count
    constexpr int isCheckNewPeace = 0x028;    // Boolean - New piece notification flag
}

// SaveMusicResult Class Offsets
namespace SaveMusicResultOffsets
{
    constexpr int MusicName = 0x010;          // String
    constexpr int Easy = 0x018;               // ResultStatus
    constexpr int Normal = 0x020;             // ResultStatus
    constexpr int Hard = 0x028;               // ResultStatus
    constexpr int VeryHard = 0x030;           // ResultStatus
}

// ResultHolder Class Offsets
namespace ResultHolderOffsets
{
    constexpr int _resultDictionary = 0x010;  // Dictionary<ResultType, Int32>
}

// OnePlaySaveHolder Class Offsets (current play session)
namespace OnePlaySaveHolderOffsets
{
    constexpr int LatestMusic = 0x010;           // MusicMemo
    constexpr int DebugForceResultSkip = 0x030;  // Boolean - Skip result screen
    constexpr int StoryCharacter = 0x034;        // CharacterType
    constexpr int SelectedDifficulty = 0x038;    // DifficultyType
    constexpr int LatestMusicRank = 0x03C;       // MusicRank
    constexpr int LatestMusicClearType = 0x040;  // MusicClearType
    constexpr int LatestGaugePercent = 0x044;    // Single
    constexpr int CurrentGameMode = 0x048;       // GameMode
    constexpr int IsAutoPlay = 0x04C;            // Boolean
    constexpr int LatestAddCharacterAffinityPoint = 0x050; // Int32
    constexpr int UpdateRecordScore = 0x05C;     // Int32
    constexpr int CurrentMapStep = 0x060;        // MapStepType
    constexpr int CurrentMapIndex = 0x064;       // Int32
    constexpr int MapClearFlag = 0x068;          // Boolean
}

// SaveManager Class Offsets
namespace SaveManagerOffsets
{
    constexpr int _current = 0x020;              // SaveData
    constexpr int _isInitialized = 0x058;        // Boolean
}
