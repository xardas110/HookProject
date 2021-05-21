#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitableMesh.generated.h"

UCLASS()
class HOOKGAME_API AHitableMesh : public AActor
{
	GENERATED_BODY()
	
public:	

	AHitableMesh();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float Health{ 10.f };

	void HandleDestruction();
	
protected:

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
