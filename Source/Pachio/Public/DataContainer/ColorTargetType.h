#pragma once
#include "CoreMinimal.h"


// 色モードの対象を示す列挙型
UENUM(BlueprintType)
enum class EColorTargetType : uint8
{
    Layer      UMETA(DisplayName = "Layer"),       // レイヤー単位での色変更
    Object     UMETA(DisplayName = "Object"),      // オブジェクト単位での色変更
    Background UMETA(DisplayName = "Background"),  // 背景色の変更
    Responders UMETA(DisplayName = "Responders")   // 複数の反応対象（通知など）の色変更
};
