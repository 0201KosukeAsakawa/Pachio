// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//名前で関数を見つけて返す
	template <typename T>
	static T* FindComponentByName(AActor*, FName);
};

template <typename T>
static T* UFunctionLibrary::FindComponentByName(AActor* TargetActor, FName ComponentName)
{
    if (!TargetActor)
        return nullptr;

    //全てのComponentを格納する配列
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);

    //Componentの数分
    for (UActorComponent* Comp : Components)
    {
        // Componentがnull出ない　かつ　名前が同じである
        if (Comp && Comp->GetFName() == ComponentName)
        {
            // 指定された型でキャストできる
            T* targetComp = Cast<T>(Comp);
            if (targetComp)
            {
                //キャストしたものを返す
                return targetComp;
            }
        }
    }
    //見つからなかった
    return nullptr;
}
