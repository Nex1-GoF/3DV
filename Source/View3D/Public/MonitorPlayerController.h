// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MonitorPlayerController.generated.h"

/**
 * 
 */
class UWBP_MissileMonitorBase;
class AMissileManager;

UCLASS()
class VIEW3D_API AMonitorPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	virtual void BeginPlay() override;

	/** UI À§Á¬ Å¬·¡½º */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MonitorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Missile")
	TSubclassOf<AMissileManager> MissileManagerClass;

	UPROPERTY()
	UUserWidget* MonitorWidgetInstance;

	UFUNCTION(BlueprintCallable)
	UWBP_MissileMonitorBase* GetMonitorWidget() const;
	
};
