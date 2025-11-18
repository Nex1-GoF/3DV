// Fill out your copyright notice in the Description page of Project Settings.


#include "WBP_MissileMonitorBase.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateBrush.h"

void UWBP_MissileMonitorBase::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[UMG] NativeConstruct Ω««‡µ "));

	/*ApplyRenderTargetToImage(Image_Cam1, RT_Cam1);
	ApplyRenderTargetToImage(Image_Cam2, RT_Cam2);
	ApplyRenderTargetToImage(Image_Cam3, RT_Cam3);
	ApplyRenderTargetToImage(Image_Cam4, RT_Cam4);*/
}

//void UWBP_MissileMonitorBase::ApplyRenderTargetToImage(UImage* Image, UTextureRenderTarget2D* RT)
//{
//	if (!Image || !RT) return;
//
//	FSlateBrush Brush;
//	Brush.SetResourceObject(RT);
//	Brush.ImageSize = FVector2D(RT->SizeX, RT->SizeY);
//	Image->SetBrush(Brush);
//}

//void UWBP_MissileMonitorBase::SetRenderTargets(
//	UTextureRenderTarget2D* InRT_Cam1,
//	UTextureRenderTarget2D* InRT_Cam2,
//	UTextureRenderTarget2D* InRT_Cam3,
//	UTextureRenderTarget2D* InRT_Cam4)
//{
//	RT_Cam1 = InRT_Cam1;
//	RT_Cam2 = InRT_Cam2;
//	RT_Cam3 = InRT_Cam3;
//	RT_Cam4 = InRT_Cam4;
//
//	UE_LOG(LogTemp, Warning,
//		TEXT("[UMG] SetRenderTargets »£√‚µ  RT:%p %p %p %p"),
//		RT_Cam1, RT_Cam2, RT_Cam3, RT_Cam4);
//
//	ApplyRenderTargetToImage(Image_Cam1, RT_Cam1);
//	ApplyRenderTargetToImage(Image_Cam2, RT_Cam2);
//	ApplyRenderTargetToImage(Image_Cam3, RT_Cam3);
//	ApplyRenderTargetToImage(Image_Cam4, RT_Cam4);
//}

//void UWBP_MissileMonitorBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//	static float Accum = 0.f;
//	Accum += InDeltaTime;
//	if (Accum > 1.0f) {
//		Accum = 0.f;
//		UE_LOG(LogTemp, Warning,
//			TEXT("[UMG] ApplyRenderTargetToImage »£√‚µ  RT:%p %p %p %p"),
//			RT_Cam1, RT_Cam2, RT_Cam3, RT_Cam4);
//	}
//	ApplyRenderTargetToImage(Image_Cam1, RT_Cam1);
//	ApplyRenderTargetToImage(Image_Cam2, RT_Cam2);
//	ApplyRenderTargetToImage(Image_Cam3, RT_Cam3);
//	ApplyRenderTargetToImage(Image_Cam4, RT_Cam4);
//}