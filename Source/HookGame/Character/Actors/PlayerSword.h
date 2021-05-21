#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSword.generated.h"

UCLASS()
class HOOKGAME_API APlayerSword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSword();
private:
	//** Making sure each actor hit is only hit once in current swing**//
	TArray<AActor*> HitRegisteredComponentsInCurrentSwing;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Setup")
	UStaticMeshComponent* SwordMesh{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setup")
	class UBoxComponent* SwordHitbox{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setup")
	USceneComponent* SwordHandleLocation{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setup")
	USceneComponent* SwordParticleSpawnLocation{nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	UParticleSystemComponent* SwordParticleTrail{nullptr};

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* AttackSound{nullptr};

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* SliceSound{nullptr};
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	UPROPERTY(EditAnywhere, Category = "Setup")
	float Damage = 5.f;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float KnockBackValue = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float SwordLength = 100.f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class UBoxComponent* GetCollisionComponent() const;
	void StartSwing();
	void EndSwing();
	void ComboSwing();
};
