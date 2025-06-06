#pragma once
#include "CoreMinimal.h"


// 色付けモードの列挙型
UENUM(BlueprintType)
enum class EColorTargetType : uint8
{
    Layer      UMETA(DisplayName = "Layer"),      // レイヤー単位で色付け
    Object     UMETA(DisplayName = "Object"),     // オブジェクト単位で色付け
    Background UMETA(DisplayName = "Background"),  // 背景に対して色付け
    Responders    UMETA(DisplayName = "Responders") // 反応者全般（通知対象）
};