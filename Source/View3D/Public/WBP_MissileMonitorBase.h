// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_MissileMonitorBase.generated.h"

class UImage;
class UTextureRenderTarget2D;


/**
 * 
 */
UCLASS()
class VIEW3D_API UWBP_MissileMonitorBase : public UUserWidget
{
	GENERATED_BODY()
	
public:   // ← 반드시 public으로!


protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Cam1;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Cam2;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Cam3;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Cam4;

	UPROPERTY()
	UTextureRenderTarget2D* RT_Cam1;
	UPROPERTY()
	UTextureRenderTarget2D* RT_Cam2;
	UPROPERTY()
	UTextureRenderTarget2D* RT_Cam3;
	UPROPERTY()
	UTextureRenderTarget2D* RT_Cam4;


};
