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

		void NosignalRun(uint8 id);
		void UpdateTelemetry(uint8 Id, uint8 TelemetryByte);
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

		UPROPERTY(meta = (BindWidget))
		UImage* NoSignal_1;
		UPROPERTY(meta = (BindWidget))
		UImage* NoSignal_2;
		UPROPERTY(meta = (BindWidget))
		UImage* NoSignal_3;
		UPROPERTY(meta = (BindWidget))
		UImage* NoSignal_4;



		virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

		float signalRunTime = 0.3f;


		bool signal1 = false;
		bool signal2 = false;
		bool signal3 = false;
		bool signal4 = false;
		float signal1time = 0;
		float signal2time = 0;
		float signal3time = 0;
		float signal4time = 0;


		UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Block_1;

		UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Block_2;

		UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Block_3;

		UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Block_4;

		float TimeUpdateAccumulator = 0.f;

		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_1_1;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_1_2;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_1_3;

		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_2_1;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_2_2;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_2_3;

		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_3_1;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_3_2;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_3_3;

		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_4_1;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_4_2;
		UPROPERTY(meta = (BindWidget))
		UImage* Telemetry_4_3;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
		UTexture2D* GreenTexture;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
		UTexture2D* RedTexture;

		/*
		
		
		
		
		
		
		*/




	};
