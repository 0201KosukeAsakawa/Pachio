// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayAnimationWidget.h"
#include "Animation/WidgetAnimation.h"

TArray<UWidgetAnimation*> GetAllWidgetAnimations(UUserWidget* Widget)
{
    TArray<UWidgetAnimation*> Result;

    if (!Widget) return Result;

    UClass* WidgetClass = Widget->GetClass();
    // クラスのプロパティを走査
    for (TFieldIterator<FObjectProperty> PropIt(WidgetClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
    {
        FObjectProperty* ObjProp = *PropIt;

        if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
        {
            UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(Widget);
            if (UWidgetAnimation* Anim = Cast<UWidgetAnimation>(Obj))
            {
                Result.Add(Anim);
            }
        }
    }

    return Result;
}

void UPlayAnimationWidget::PlayAnimationByName(FName AnimName)
{
    if (AnimName.IsNone())
        return;
    // FString に変換して文字列を追加
    FString AnimStr = AnimName.ToString();
    AnimStr += "_INST";

    // FName に戻す
    FName NewAnimName(*AnimStr);

    // 自分自身を UUserWidget として扱う
    UUserWidget* Widget = Cast<UUserWidget>(this);
    if (!Widget)
        return;

    // 全アニメーションを取得
    TArray<UWidgetAnimation*> Anims = GetAllWidgetAnimations(Widget);

    // 名前一致するものを探す
    for (UWidgetAnimation* Anim : Anims)
    {
        if (!Anim) continue;
        FName animName = Anim->GetFName();
        if (animName == NewAnimName) // ここで比較
        {
            Widget->PlayAnimation(Anim,0,0);
            UE_LOG(LogTemp, Log, TEXT("Play Animation: %s"), *Anim->GetName());
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Animation '%s' not found"), *NewAnimName.ToString());
}

void UPlayAnimationWidget::StopAnimationByName(FName AnimName)
{
    if (AnimName.IsNone()) return;

    TArray<UWidgetAnimation*> Anims = GetAllWidgetAnimations(this);
    for (UWidgetAnimation* Anim : Anims)
    {
        if (Anim && Anim->GetFName() == AnimName)
        {
            // 一度アニメーションを先頭フレームに適用
            PlayAnimation(Anim, 0.f, 1, EUMGSequencePlayMode::Forward, 0.f);

            // すぐ止めることで「初期状態」に戻る
            StopAnimation(Anim);

            UE_LOG(LogTemp, Log, TEXT("Stopped and Reset Animation: %s"), *Anim->GetName());
            return;
        }
    }
}

void UPlayAnimationWidget::StopAllAnimation()
{
    TArray<UWidgetAnimation*> Anims = GetAllWidgetAnimations(this);
    for (UWidgetAnimation* Anim : Anims)
    {
        if (Anim)
        {
            PlayAnimation(Anim, 0.f, 1, EUMGSequencePlayMode::Forward, 0.f);
            StopAnimation(Anim);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Stopped and Reset All Animations"));
}

