// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

/**
 * @brief 汎用的なユーティリティ関数を提供する関数ライブラリ
 * BlueprintやC++から簡単に呼び出せる静的関数群
 */
UCLASS()
class PACHIO_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * @brief 指定されたActorから名前でComponentを探して返す
     * @tparam T 取得したいComponentの型
     * @param TargetActor 検索対象のActor
     * @param ComponentName 取得したいComponentの名前
     * @return 指定型にキャスト可能なComponentが見つかればそのポインタ、なければnullptr
     */
    template <typename T>
    static T* FindComponentByName(AActor* TargetActor, FName ComponentName);

    /**
     * @brief 指定Enum型の全ての値を取得する
     * @tparam EnumType 取得したいEnum型
     * @return Enumの全値を格納したTArray
     */
    template <typename EnumType>
    static TArray<EnumType> GetAllEnumValues();
};

template <typename T>
T* UFunctionLibrary::FindComponentByName(AActor* TargetActor, FName ComponentName)
{
    if (!TargetActor)
        return nullptr;

    // Actorが持つ全てのComponentを取得
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);

    // Componentの数だけループ
    for (UActorComponent* Comp : Components)
    {
        // Componentがnullでないかつ名前が一致
        if (Comp && Comp->GetFName() == ComponentName)
        {
            // 指定型にキャスト可能か確認
            T* targetComp = Cast<T>(Comp);
            if (targetComp)
            {
                // キャスト成功したComponentを返す
                return targetComp;
            }
        }
    }

    // 見つからなかった場合
    return nullptr;
}

template<typename EnumType>
TArray<EnumType> UFunctionLibrary::GetAllEnumValues()
{
    TArray<EnumType> EnumValues;

    // Enum型に対応するUEnumを取得
    UEnum* EnumPtr = StaticEnum<EnumType>();
    if (!EnumPtr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enum not found!"));
        return EnumValues;
    }

    // Enumの全値を列挙（最後のNoneなどを除外）
    for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
    {
        EnumType EnumValue = static_cast<EnumType>(EnumPtr->GetValueByIndex(i));
        EnumValues.Add(EnumValue);
    }

    return EnumValues;
}
