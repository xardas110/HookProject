#pragma once

#include "CoreMinimal.h"
#include "AISquidbird.h"
#include "AISquidbirdLeaper.generated.h"

UCLASS()
class HOOKGAME_API AAISquidbirdLeaper : public AAISquidbird
{
	GENERATED_BODY()
	
public:
	///			Variables			///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaper Params")
		float LeapDistance = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaper Params")
		float MaxZIncrease = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaper Params")
		FVector LeapForce{3000.f,3000.f,5000.f};
	
	FVector LeapTarget{ 0.f,0.f,0.f };

	///			Functions			///
	virtual void Attack() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndAttack() override;
	void SetGrabbedVelocity();
	void KeepGrounded();
	void BeginAttack();
	void Leap();
protected:
	bool bVelocityReduced = false;
	
	FRotator LeapRotation{ 90.f,0.f,0.f };
	FRotator RegularRotation{ 0.f,0.f,0.f };
};
