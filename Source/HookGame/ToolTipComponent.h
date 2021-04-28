// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ToolTipComponent.generated.h"

UCLASS()
class HOOKGAME_API AToolTipComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolTipComponent();

	friend class APlayerBase;

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	bool bHasPlayerOverlapped = false;

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	UBoxComponent* ToolTipBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	FString ToolTipText = "Remember to set the text in here";

	UUserWidget* ToolTipUI;
	UPROPERTY(EditAnywhere, Category = "ToolTipWidget")
	TSubclassOf<class UUserwidget> ToolTipWidget;
	

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
