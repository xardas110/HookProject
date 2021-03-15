// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIBehaviorBase.h"
#include "GameFramework/PawnMovementComponent.h"

typedef APlayerBase THIS;

constexpr auto CharacterTopBone = TEXT("End");
constexpr auto CharacterBotBone = TEXT("End_002");
constexpr auto CharacterSpine2 = TEXT("Spine2");
constexpr auto CharacterHips = TEXT("Hips");
constexpr auto CharacterSwordSocket = TEXT("SwordSocket");

#ifdef _DEBUG_ALL
#define _DEBUG_VAULTING
#define _DEBUG_ANIM_NOTIFY
#define _DEBUG_GRAPPLE
#define _DEBUG_ANIM_STATES
#define _DEBUG_UPDATE_CAMERA
#endif
//#define _DEBUG_UPDATE_CAMERA
//#define _DEBUG_GRAPPLE
APlayerBase::APlayerBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Make Character move towards input direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	
	CreateComponents();
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	CharacterInit();
	WidgetInit();
	GrappleInit();
	SwordInit();

	CameraInitialPosition = SpringArm->TargetOffset;
	CameraInitialRotation = SpringArm->GetRelativeRotation();
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraUpdate(DeltaTime);
	HookUpdate(DeltaTime);
	VaultingUpdate(DeltaTime);
	DebugUpdate(DeltaTime);
}
void APlayerBase::VaultingUpdate(const float DeltaTime)
{	
	if (CustomMovementModes::Vaulting == _CustomMovementMode)
	{
		const FVector CharacterTopPoint = GetMesh()->GetBoneLocation(CharacterTopBone);
		const FVector CharacterLowPoint = GetMesh()->GetBoneLocation(CharacterBotBone);
		const float CharacterHeight = (CharacterTopPoint - CharacterLowPoint).Size();
		const float CharacterHalfHeight = CharacterHeight * 0.5f;
		GetCapsuleComponent()->SetCapsuleHalfHeight(CharacterHalfHeight);
	}

	if (CustomMovementModes::Default != _CustomMovementMode && CustomMovementModes::Grappling != _CustomMovementMode)
		return;
	
	const TArray<AActor*> FrontTraceIgnoredActors;
	FHitResult FrontTraceHitResult;
	const float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float ForwardReach = CapsuleRadius;
	const FVector ThisLocation = GetActorLocation();
	const FVector ThisForward = GetActorForwardVector();
	const FVector ForwardEndTrace = ThisLocation + ThisForward * ForwardReach;
	
#ifdef _DEBUG_VAULTING
	const bool bForwardTraceHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), ThisLocation, ForwardEndTrace, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery1, true, FrontTraceIgnoredActors, EDrawDebugTrace::ForOneFrame, FrontTraceHitResult, true);
#else
	const bool bForwardTraceHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), ThisLocation, ForwardEndTrace, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery1, true, FrontTraceIgnoredActors, EDrawDebugTrace::None, FrontTraceHitResult, true);
#endif
	if (!bForwardTraceHit)
		return;

	const TArray<AActor*> TopTraceIgnoredActors;
	FHitResult TopTraceHitResult;
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float MaxVaultHeight = CapsuleHalfHeight + GetCharacterMovement()->GetMaxJumpHeight();
	const float TotalHeightFromTopToBottom = MaxVaultHeight + CapsuleHalfHeight;
	const FVector TopTraceStartPoint = FrontTraceHitResult.ImpactPoint + FVector(0, 0.f, MaxVaultHeight) + ThisForward * CapsuleRadius;
	const FVector TopEndTrace = TopTraceStartPoint - FVector(0.f, 0.f, TotalHeightFromTopToBottom);
#ifdef _DEBUG_VAULTING
	const bool bTopTraceHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TopTraceStartPoint, TopEndTrace, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery1, true, TopTraceIgnoredActors, EDrawDebugTrace::ForOneFrame, TopTraceHitResult, true);
#else
	const bool bTopTraceHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TopTraceStartPoint, TopEndTrace, CapsuleRadius, ETraceTypeQuery::TraceTypeQuery1, true, TopTraceIgnoredActors, EDrawDebugTrace::None, TopTraceHitResult, true);
#endif
	if (!bTopTraceHit)
		return;
	
	const FVector EdgeLocation = FVector(FrontTraceHitResult.ImpactPoint.X, FrontTraceHitResult.ImpactPoint.Y, TopTraceHitResult.ImpactPoint.Z);
#ifdef _DEBUG_VAULTING
	DrawDebugPoint(GetWorld(), EdgeLocation, 20.f, FColor::Orange);
#endif
	const FVector DirFwdImpToEdge = EdgeLocation - FrontTraceHitResult.ImpactPoint;
	const float DistFwdImpactPointToEdge = DirFwdImpToEdge.Size();
	const float DistPlayerToFwdImpactPoint = (FrontTraceHitResult.ImpactPoint - ThisLocation).Size();

	const FVector CharacterNeckLocation = GetMesh()->GetBoneLocation(CharacterSpine2);
	const FVector CharacterNeckCapsuleEndLocation = CharacterNeckLocation - FrontTraceHitResult.ImpactNormal * CapsuleRadius;
#ifdef _DEBUG_VAULTING
	DrawDebugPoint(GetWorld(), CharacterNeckCapsuleEndLocation, 20.f, FColor::Blue);
#endif
	const bool bInsidePlayerRadius = DistPlayerToFwdImpactPoint <= CapsuleRadius+1.f;
	const bool bCanReachEdge = (EdgeLocation - CharacterNeckCapsuleEndLocation).Size() < 15.f;

	const FVector WallFwdNormal = FrontTraceHitResult.ImpactNormal;
	const FVector TargetLocation = FrontTraceHitResult.ImpactPoint + ThisForward * CapsuleRadius;

	if (bInsidePlayerRadius && bCanReachEdge && GetCharacterMovement()->IsFalling())
	{
		TryEnterVaultingTransition(TopTraceHitResult.ImpactPoint);
	}
}

void APlayerBase::TryEnterVaultingTransition(const FVector& EndPoint)
{
	if (_CustomMovementMode == CustomMovementModes::Default || _CustomMovementMode == CustomMovementModes::Grappling)
	{
		const FVector DirToVaultTarget = EndPoint - GetActorLocation();
		const FRotator RotationToTarget = (DirToVaultTarget / DirToVaultTarget.Size()).Rotation();
		SetActorRotation({ 0.f, RotationToTarget.Yaw, 0.f });
		_CustomMovementMode = CustomMovementModes::Vaulting;
	}
}

void APlayerBase::StartVaulting()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	_CustomMovementMode = CustomMovementModes::Vaulting;
	
	if (_HookCurrentState != HookCurrentState::GrappleDetached)
		HookDetach();
}

void APlayerBase::EndVaulting()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDefaultStandingHeight);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	if (_CustomMovementMode == CustomMovementModes::Vaulting)
	{
		_CustomMovementMode = CustomMovementModes::Default;
	}
}

void APlayerBase::SwordStartAttack()
{
	if (SwordPtr && _CustomMovementMode == CustomMovementModes::Attacking)
	{
		if (Controller)
		{
			const auto PlayerController = Cast<APlayerController>(Controller);
			if(PlayerController)
			{
				FHitResult FHit;
				PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, FHit);

				const auto ImpactPoint = FHit.ImpactPoint;
				const auto ThisLocation = GetActorLocation();
				const auto ThisLocToImpact = ImpactPoint - ThisLocation;
				const auto DirToImpact = (ThisLocToImpact / ThisLocToImpact.Size()).GetSafeNormal2D();
				const auto Rotation = DirToImpact.Rotation();
				SetActorRotation(Rotation);
			}		
		}
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopActiveMovement();
		SwordPtr->StartSwing();
		_AttackState = AttackState::Attack1;	
		HookDelayTimer = 0.f;
	}
}

void APlayerBase::SwordEndAttack()
{
	if (_CustomMovementMode == CustomMovementModes::Attacking)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		_CustomMovementMode = CustomMovementModes::Default;
	}
	if (SwordPtr)
	{
		SwordPtr->EndSwing();
	}
	AttackPressedWhileSwinging = 0;
	AttackCurrentComboNR = 0;
	bCanTransitionToAttack2 = false;
	bCanTransitionToAttack1 = false;
	HookDelayTimer = HookMaxDelay;
	_AttackState = AttackState::Default;
}

void APlayerBase::CameraUpdate(const float DeltaTime)
{
	CameraPositionUpdate(DeltaTime);
	CameraRotationUpdate(DeltaTime);
}

void APlayerBase::CameraPositionUpdate(const float DeltaTime)
{
	if (!Controller)
		return;

	const auto PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
		return;

	float x, y;
	if (!PlayerController->GetMousePosition(x, y))
		return;

#ifdef _DEBUG_UPDATE_CAMERA
	//UE_LOG(LogTemp, Warning, TEXT("Mouse X: %f\nMouse Y: %f"), x, y);
#endif
	int32 Width, Height;
	PlayerController->GetViewportSize(Width, Height);
	const float FWidth = static_cast<float>(Width);
	const float FHeight = static_cast<float>(Height);
	const float XScale = x / FWidth;
	const float YScale = y / FHeight;
	const float AspectRatio = FWidth / FHeight;
	const float ScreenRatio = EdgeScale;
	
	const auto XMin = ScreenRatio;
	const auto XMax = 1.f - XMin;
	const auto YMin = ScreenRatio * AspectRatio;
	const auto YMax = 1.f - YMin;

	struct Bounds
	{
		Bounds() :XMin(0.f), XMax(0.f), YMin(0.f), YMax(0.f) {};
		Bounds(const float XMin, const float XMax, const float YMin, const float YMax)
			:XMin(XMin), XMax(XMax), YMin(YMin), YMax(YMax) {};
		float XMin, XMax, YMin, YMax;
		bool Intersect(const float MouseX, const float MouseY) const
		{
			return MouseX >= XMin && MouseX <= XMax && MouseY >= YMin && MouseY <= YMax;
		}
	}

	const MidScreen(XMin, XMax, YMin, YMax);
	if (MidScreen.Intersect(XScale, YScale))
	{
		CameraMoveToPlayer(DeltaTime);
		return;
	}

	const float YInverted = 1.f - YScale;
	const float YH = YInverted * 2.f - 1.f;
	const float XH = (XScale * 2.f) - 1.f;
	const FVector2D MousePos(XH, YH);
	const FVector2D MouseDir = MousePos / MousePos.Size();
	CameraMoveTo2DDirection(MouseDir, DeltaTime);

#ifdef _DEBUG_UPDATE_CAMERA
	UE_LOG(LogTemp, Warning, TEXT("Mouse DIR: %s"), *MouseDir.ToString());
#endif
}

void APlayerBase::CameraMoveTo2DDirection(const FVector2D& Dir, const float DeltaTime)
{
#ifdef _DEBUG_UPDATE_CAMERA
	UE_LOG(LogTemp, Warning, TEXT("target offset: %s"), *SpringArm->TargetOffset.ToString());
#endif
	const FRotator SpringArmYawRotation = {0.f, SpringArm->GetRelativeRotation().Yaw, 0.f};
	const FQuat SpringArmYawQuaternion = SpringArmYawRotation.Quaternion();
	
	const FVector Dir3D = FVector(Dir.Y, Dir.X, 0.f);
	const FVector Dir3DVelocity = (Dir3D * CameraLerpToSpeed * DeltaTime);
	const FVector Dir3DVelocityTransformed = SpringArmYawQuaternion.RotateVector(Dir3DVelocity);

	FVector NewLocation = SpringArm->TargetOffset + Dir3DVelocityTransformed;
	NewLocation = FMath::VInterpConstantTo(SpringArm->TargetOffset, NewLocation, DeltaTime, CameraLerpToSpeed);
	NewLocation.Z = 0.f;

	//** This feels better than just setting it back to springarm->targetoffset**//
	if (FMath::Abs(NewLocation.X) > MaxCameraExtent.X)
	{
		const float Direction = NewLocation.X < 0.f ? -1.f : 1.f;
		NewLocation.X = FMath::FInterpTo(NewLocation.X, MaxCameraExtent.X * Direction, DeltaTime, (CameraLerpToSpeed*0.05f));
	}
	if (FMath::Abs(NewLocation.Y) > MaxCameraExtent.Y)
	{
		const float Direction = NewLocation.Y < 0.f ? -1.f : 1.f;
		NewLocation.Y = FMath::FInterpTo(NewLocation.Y, MaxCameraExtent.Y * Direction, DeltaTime, (CameraLerpToSpeed * 0.05f));
	}
	
	SpringArm->TargetOffset = NewLocation;
}

void APlayerBase::CameraMoveToPlayer(const float DeltaTime)
{
	const FVector NewLocation = FVector(CameraInitialPosition);
	SpringArm->TargetOffset = FMath::Lerp(SpringArm->TargetOffset, NewLocation, CameraLerpBackSpeed* DeltaTime);
}

void APlayerBase::CameraRotationUpdate(const float DeltaTime)
{
	if (!AssociatedTriggerBox)
		return;

	if (AssociatedTriggerBox->Reverse)
	{	
		const auto CameraRotation = SpringArm->GetRelativeRotation();
		const auto NewRotation = FMath::Lerp(CameraRotation, AssociatedTriggerBox->EndOverlapRotation, AssociatedTriggerBox->RotationSpeed * DeltaTime);
		SpringArm->SetRelativeRotation(NewRotation);
		if (NewRotation.Equals(CameraInitialRotation, 1.f))
		{
			AssociatedTriggerBox->Reverse = false;
			AssociatedTriggerBox = nullptr;		
		}
	}
	else if (AssociatedTriggerBox->bOnBeginOverlapRotation)
	{
		const auto CameraRotation = SpringArm->GetRelativeRotation();
		const auto NewRotation = FMath::Lerp(CameraRotation, AssociatedTriggerBox->BeginOverlapRotation, AssociatedTriggerBox->RotationSpeed * DeltaTime);
		SpringArm->SetRelativeRotation(NewRotation);
		if (NewRotation.Equals(AssociatedTriggerBox->BeginOverlapRotation, 1.f) && !AssociatedTriggerBox->bOnEndOverlapRotation)
		{
			AssociatedTriggerBox = nullptr;
		}
	}
}

void APlayerBase::OnBeginCameraTrigger(ACameraRotateTrigger* Trigger)
{
	if (AssociatedTriggerBox)
		AssociatedTriggerBox->ResetToDefault();
	AssociatedTriggerBox = Trigger;
}

void APlayerBase::OnEndCameraTrigger(ACameraRotateTrigger* Trigger)
{
	if (AssociatedTriggerBox == Trigger)
		AssociatedTriggerBox->Reverse = true;
	else
		Trigger->Reverse = false;
}

void APlayerBase::OnVaultingBegin_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionStart"));
#endif
	StartVaulting();
}

void APlayerBase::OnVaultingBegin_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionInterrupted"));
#endif
	EndVaulting();
}

void APlayerBase::OnVaultingBegin_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionEnd"));
#endif
}

void APlayerBase::OnVaultingEnd_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionStart"));
#endif
	EndVaulting();
}

void APlayerBase::OnVaultingEnd_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionInterrupted"));
#endif

}

void APlayerBase::OnVaultingEnd_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionEnd"));
#endif

}

void APlayerBase::TryEnterSwordAttackTransition()
{
	if (_CustomMovementMode == CustomMovementModes::Attacking)
	{		
		HandleAttackCombo();
		AttackPressedWhileSwinging++;
	}
	if (_CustomMovementMode == CustomMovementModes::Default && !GetCharacterMovement()->IsFalling())
	{
		_CustomMovementMode = CustomMovementModes::Attacking;
	}	
}

void APlayerBase::OnSwordAttackBegin_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack Begin Transition Start"));
#endif
	SwordStartAttack();
}

void APlayerBase::OnSwordAttackBegin_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack Begin Transition Interrupted"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackBegin_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnSwordAttackBegin_TransitionEnd"));
#endif
}

void APlayerBase::OnSwordAttackEnd_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition Start"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackEnd_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition Interrupted"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackEnd_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition End"));
#endif
	SwordEndAttack();
}

void APlayerBase::HandleAttackCombo()
{
	if (AttackPressedWhileSwinging == AttackCurrentComboNR)
	{
		if (_AttackState == AttackState::Attack1)
		{
			if (bAttack1WindowOpen)
			{
				AttackCurrentComboNR++;
				_AttackState = AttackState::Attack2;
				bCanTransitionToAttack2 = true;
				bCanTransitionToAttack1 = false;
				HookDelayTimer = 0.f;
				if (SwordPtr)
					SwordPtr->ComboSwing();
			}
		}else if (_AttackState == AttackState::Attack2)
		{
			if (bAttack2WindowOpen)
			{
				AttackCurrentComboNR++;
				_AttackState = AttackState::Attack1;
				bCanTransitionToAttack1 = true;
				bCanTransitionToAttack2 = false;
				HookDelayTimer = 0.f;
				if (SwordPtr)
					SwordPtr->ComboSwing();
			}
		}
	}
}

void APlayerBase::OnAttack1StartWindow()
{
	bAttack1WindowOpen = true;
}

void APlayerBase::OnAttack1EndWindow()
{
	bAttack1WindowOpen = false;
}

bool APlayerBase::IsAttack1() const
{
	return _AttackState == AttackState::Attack1;
}

bool APlayerBase::CanTransitionToAttack1() const
{
	return bCanTransitionToAttack1;
}

void APlayerBase::OnAttack2StartWindow()
{
	bAttack2WindowOpen = true;
}

void APlayerBase::OnAttack2EndWindow()
{
	bAttack2WindowOpen = false;
}

bool APlayerBase::IsAttack2() const
{
	return _AttackState == AttackState::Attack2;
}


void APlayerBase::HookUpdate(const float DeltaTime)
{
	HookSpawnLocation = GetActorLocation() + GetActorForwardVector() * 20.f;
	
	if (HookCurrentState::GrappleDetached == _HookCurrentState)
	{
		HookDelayTimer += DeltaTime;
		return;
	}

	if (!Hook)
		return;

	//Out of range scenario
	if (HookMaxRange < GetDistanceToHook())
	{
		_HookCurrentState = HookCurrentState::GrappleReturn;
	}

	if (HookDetachTimer < 0.f)
	{
		HookDelete();
		return;
	}

	HookDetachTimer -= DeltaTime;
	
	auto DirectionToHook = GetDirectionToHook();
	DirectionToHook /= DirectionToHook.Size();
	const FRotator Rotation = { 0.f, DirectionToHook.Rotation().Yaw, 0.f };

	switch (_HookCurrentState)
	{
	case HookCurrentState::GrappleTo:
	{
		SetActorRotation(Rotation);	
		GetCharacterMovement()->Velocity += CalculatePlayerVelocityWhileRetracting(DeltaTime);
		if (GetDistanceToHook() < HookInRange)
		{
			HookDelete();
		}
	}
	break;
	case HookCurrentState::GrappleReturn:
	{
		Hook->GetCollisionComponent()->SetWorldRotation(DirectionToHook.Rotation());
		Hook->GetProjectileMovementComponent()->Velocity = -DirectionToHook * HookReturnSpeed;

		if (GetDistanceToHook() < HookInRange)
		{
			if (AttachedComponent)
			{
				AttachedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				AttachedComponent->SetWorldLocation(GetActorLocation() + GetActorForwardVector() * 120.f);
				AttachedComponent->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
			}
			HookDelete();
		}
	}
	break;
	default:
		break;
	}
}

void APlayerBase::HookFire()
{
	if (!HookCanFire())
		return;
	if (CustomMovementModes::Default != _CustomMovementMode)
		return;

	if (HookDelayTimer <= HookMaxDelay)
		return;

	if (!Hook)
	{
		HookDelayTimer = 0.f;
		const auto PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

			const FVector ImpactPoint = HitResult.ImpactPoint;
			const FVector ThisLocation = GetActorLocation();
			const FVector DirectionToImpactPoint = ImpactPoint - ThisLocation;
			const FVector Direction = (DirectionToImpactPoint / DirectionToImpactPoint.Size()).GetSafeNormal2D();
			const FRotator Rotation = Direction.Rotation();

			const FVector HookSpawn = GetMesh()->GetBoneLocation(CharacterHips) + (Direction * 200.f);
			//const FRotator HookRotation = GetActorForwardVector().Rotation();

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = this;

			Hook = GetWorld()->SpawnActor<AHookProjectile>(HookProjectileClass, HookSpawn, Rotation, Params);
			Hook->GetCollisionComponent()->SetupAttachment(RootComponent);
			Hook->GetProjectileMovementComponent()->InitialSpeed = HookFireSpeed;
			Hook->GetProjectileMovementComponent()->MaxSpeed = HookFireSpeed;
			Hook->GetProjectileMovementComponent()->Velocity = Direction * HookFireSpeed;
			GrappleCable->SetAttachEndToComponent(Hook->GetCableAttachmentPoint());
			GrappleCable->SetVisibility(true);

			//Make sure this is before ShootGrabbedComponent
			_HookCurrentState = HookCurrentState::GrappleFired;
			_CustomMovementMode = CustomMovementModes::Grappling;
			if (AttachedComponent)
			{
#ifdef _DEBUG_GRAPPLE
				UE_LOG(LogTemp, Error, TEXT("Shooting attached component"));
#endif
				AttachedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				AttachedComponent->AttachToComponent(Hook->GetCollisionComponent(), FAttachmentTransformRules::KeepWorldTransform);

				_HookCurrentState = HookCurrentState::GrappleFiredWithObject;

			}
		}

	}
}

bool APlayerBase::HookCanFire()
{
	/*
	if (!Controller)
		return false;

	const auto PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
		return false;
	
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	const FVector ImpactPoint = HitResult.ImpactPoint;
	const FVector ThisLocation = GetActorLocation();
	const FVector DirectionToImpactPoint = ImpactPoint - ThisLocation;
	FVector Direction = DirectionToImpactPoint / DirectionToImpactPoint.Size();
	Direction.Y = 0.f;
	FHitResult FHitRay;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(FHitRay, GetActorLocation(), GetActorLocation() + (Direction*HookMaxRange), ECollisionChannel::ECC_Visibility, Params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ray running"));
		const auto RayImpact = FHitRay.ImpactPoint;
		const auto Distance = FVector::Dist(RayImpact, ThisLocation);
		if (Distance < 100.f)
			return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ray failed"));
	}
	
	*/
	return true;
}

void APlayerBase::GrappleInit()
{
	HookDelayTimer = 0.f;
	HookDetachTimer = HookDetachMaxTimer;
	//Cable Component Related
	GrappleCable->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	GrappleCable->SetVisibility(false);
	GrappleCable->bEnableStiffness = true;
	GrappleCable->NumSegments = 2.f;
	GrappleCable->SetEnableGravity(false);
}

void APlayerBase::WidgetInit()
{
	const auto PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		WidgetCursorNotLocked = CreateWidget(PlayerController, WidgetClassCursorNoneLock);
		PlayerController->SetMouseCursorWidget(EMouseCursor::Crosshairs, WidgetCursorNotLocked);
	}
}

void APlayerBase::CharacterInit()
{
	CurrentHealth = MaxHealth;
	GetCharacterMovement()->AirControl = DefaultAirControl;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDefaultStandingHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleDefaultRadius);

	
}

void APlayerBase::SwordInit()
{
	SwordPtr = GetWorld()->SpawnActor<APlayerSword>(SwordClass);
	if (SwordPtr)
	{
		SwordPtr->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterSwordSocket);
		SwordPtr->SetActorHiddenInGame(false);
		SwordPtr->SetOwner(this);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn sword"));
	}
}

void APlayerBase::HandleDeath()
{
	//GetCharacterMovement()->DisableMovement();
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetMesh()->AddImpulse(GetActorForwardVector() * -10000.f, NAME_None, false);
}

void APlayerBase::HookDetach()
{
	if (_CustomMovementMode == CustomMovementModes::Grappling)
		_CustomMovementMode = CustomMovementModes::Default;
	
	if (HookCurrentState::GrappleDetached == _HookCurrentState)
		return;

	if (Hook)
	{
		Hook->GetProjectileMovementComponent()->UpdateComponentVelocity();
		Hook->GetProjectileMovementComponent()->SetUpdatedComponent(Hook->GetCollisionComponent());
		Hook->GetProjectileMovementComponent()->bRotationFollowsVelocity = false;

		if (AttachedComponent)
		{
			AttachedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			AttachedComponent->SetSimulatePhysics(true);
			AttachedComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			const FVector UnitDirectionToHook = GetDirectionToHook() / GetDirectionToHook().Size();
			AttachedComponent->SetWorldLocation(Hook->GetCollisionComponent()->GetComponentLocation() + UnitDirectionToHook * 100.f);
			AttachedComponent->SetPhysicsLinearVelocity(Hook->GetProjectileMovementComponent()->Velocity);
			GrappleDetachPhysicsComponent();		
		}
	}
	_HookCurrentState = HookCurrentState::GrappleReturn;
}

void APlayerBase::GrappleDetachPhysicsComponent()
{
	if (AttachedComponent)
	{
		Hook->GetCollisionComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		const auto Actor = AttachedComponent->GetOwner();
		if (Actor)
		{
			if (Actor->IsA(AAIBehaviorBase::StaticClass()))
			{
				const auto AIBB = Cast<AAIBehaviorBase>(Actor);
				if (AIBB)
					AIBB->OnDetach();
			}
		}
		AttachedComponent = nullptr;		
	}
}

void APlayerBase::OnFirePressed()
{
	TryEnterSwordAttackTransition();
}

FVector APlayerBase::GetDirectionToHook() const
{
	const FVector HookLocation = Hook->GetCollisionComponent()->GetComponentLocation();
	const FVector ThisLocation = GetActorLocation();
	return HookLocation - ThisLocation;
}

float APlayerBase::GetDistanceToHook() const
{
	return GetDirectionToHook().Size();
}

void APlayerBase::HookDelete()
{
	GrappleCable->SetAttachEndToComponent(GetCapsuleComponent());
	
	Hook->Destroy();
	Hook = nullptr;
	HookDetachTimer = HookDetachMaxTimer;
	GrappleCable->SetVisibility(false);

	GetCharacterMovement()->AirControl = DefaultAirControl;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;

	_HookCurrentState = HookCurrentState::GrappleDetached;
	if (CustomMovementModes::Grappling == _CustomMovementMode)
	{
		_CustomMovementMode = CustomMovementModes::Default;
	}
}

const FVector APlayerBase::CalculatePlayerVelocityWhileRetracting(const float DeltaTime)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacterMovement()->AirControl = 0.0f;
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingFriction = 0.f;
		GetCharacterMovement()->FallingLateralFriction = 0.f;
	}
	
	const FVector PlayerVelocity = GetCharacterMovement()->Velocity;
	const FVector DirectionToHook = GetDirectionToHook();
	const FVector DirToHookUnit = DirectionToHook / DirectionToHook.Size();
	const FVector PlayerVelocityDir = PlayerVelocity / PlayerVelocity.Size();
	
	if (PlayerVelocity.IsNearlyZero())
	{
#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("PlayerVelocity:nearly zero"));
#endif
		LaunchCharacter(GetActorUpVector() * HookOnAttachedLaunchForce, false, false);
		return DirToHookUnit * HookRetractStartSpeed;
	}
#ifdef _DEBUG_GRAPPLE
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (PlayerVelocityDir * 200.f), FColor::Blue, false, -1, 0, 2.f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (DirToHookUnit * 200.f), FColor::Red, false, -1, 0, 2.f);
#endif
	const float CurrentSpeed = PlayerVelocity.Size();
	const float DotProj = Dot3(PlayerVelocityDir, DirToHookUnit);
	const float ProjectedSpeed = DotProj * CurrentSpeed;
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("Currentspeed: %f"), CurrentSpeed);
	UE_LOG(LogTemp, Warning, TEXT("ProjectedSpeed: %f"), ProjectedSpeed);
#endif
	if (ProjectedSpeed > HookRetractMaxSpeed)
	{
		GetCharacterMovement()->Velocity += GetCharacterMovement()->Velocity * -0.1f;
	}else if (ProjectedSpeed < HookRetractMaxSpeed)
	{
		GetCharacterMovement()->Velocity += DirToHookUnit * HookConstantAddSpeed;
	}
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("DotProj: %f"), DotProj);
#endif
	const float DistanceToHook = DirectionToHook.Size();
	const float x = DistanceToHook / InitialDistanceToHook;
	const float a = 60.f;
	const float b = 200.f;
	const float c = 0.f;
	const float WishSpeed = QuadraticFormula(x, a, b, c);

#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("WishSpeed: %f"), WishSpeed);
	UE_LOG(LogTemp, Warning, TEXT("DistanceToHook: %f"), DistanceToHook);
#endif
	return DirToHookUnit * WishSpeed;
}

float APlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	const auto IncomingDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= FMath::Min(CurrentHealth, IncomingDamage);
	if (CurrentHealth <= 0.f)
		HandleDeath();
	return IncomingDamage;
}

void APlayerBase::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (HookCurrentState::GrappleReturn == _HookCurrentState)
		return;

	if (HookCurrentState::GrappleFiredWithObject == _HookCurrentState)
	{
		_HookCurrentState = HookCurrentState::GrappleReturn;
		return;
	}
	
	if (HitComponent)
	{	
		AttachedComponentType = OtherComp->GetCollisionObjectType();
		if (Hook)
		{
			switch (AttachedComponentType)
			{
				case ECollisionChannel::ECC_WorldStatic:
				{
					Hook->GetProjectileMovementComponent()->StopSimulating(Hit);
					GetCharacterMovement()->AirControl = 0.0f;
					InitialDistanceToHook = GetDistanceToHook();
					LaunchCharacter(GetActorUpVector() * HookOnAttachedLaunchForce, false, false);
					_HookCurrentState = HookCurrentState::GrappleTo;
					
				}
				break;
				case ECollisionChannel::ECC_PhysicsBody:
				{
					AttachedComponent = OtherComp;
					//Hook->GetProjectileMovementComponent()->StopSimulating(Hit);
					//Hook->GetCollisionComponent()->AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
					AttachedComponent->SetSimulatePhysics(false);
					AttachedComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					AttachedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					AttachedComponent->AttachToComponent(Hook->GetCollisionComponent(), FAttachmentTransformRules::KeepWorldTransform);				
					_HookCurrentState = HookCurrentState::GrappleReturn;
					if (AttachedComponent->GetOwner()->IsA(AAIBehaviorBase::StaticClass()))
					{
						const auto AIBB = Cast<AAIBehaviorBase>(AttachedComponent->GetOwner());
						if (AIBB)
						{
							AIBB->OnAttach();
						}
					}
				}
				break;
				default:
				{
					_HookCurrentState = HookCurrentState::GrappleReturn;				
				}
				break;
			}
		
		}
#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("Collision type %i"), AttachedComponentType);
#endif
	}
}

void APlayerBase::OnFireReleased()
{
	
}

void APlayerBase::OnUsePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("On use pressed"));
	CurrentHealth -= 10.f;
	SetActorLocation(GetActorLocation() - FVector(0.f, 0.f, 10.f));
}

void APlayerBase::OnUseReleased()
{
}

void APlayerBase::OnJumpPressed()
{
	Jump();
}

void APlayerBase::OnJumpReleased()
{
	StopJumping();
}

void APlayerBase::OnRightClickPressed()
{
	HookFire();
}

void APlayerBase::OnRightClickReleased()
{
	HookDetach();
}

bool APlayerBase::CanTransitionToAttack2() const
{
	return bCanTransitionToAttack2;
}

bool APlayerBase::IsSliding() const
{
	return GetCharacterMovement()->CustomMovementMode == CustomMovementModes::Sliding;
}

bool APlayerBase::IsAttacking() const
{
	return _CustomMovementMode == CustomMovementModes::Attacking;
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &THIS::OnMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &THIS::OnMoveRight);

	PlayerInputComponent->BindAction(TEXT("Use"), IE_Pressed, this, &THIS::OnUsePressed);
	PlayerInputComponent->BindAction(TEXT("Use"), IE_Released, this, &THIS::OnUseReleased);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &THIS::OnFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &THIS::OnFireReleased);

	PlayerInputComponent->BindAction(TEXT("RightClick"), IE_Pressed, this, &THIS::OnRightClickPressed);
	PlayerInputComponent->BindAction(TEXT("RightClick"), IE_Released, this, &THIS::OnRightClickReleased);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &THIS::OnJumpPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &THIS::OnJumpReleased);
}

bool APlayerBase::CanVault() const
{
	return _CustomMovementMode == CustomMovementModes::Vaulting;
}

bool APlayerBase::IsDead() const
{
	return CurrentHealth <= 0.f;
}

float APlayerBase::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

float APlayerBase::GetHookDelayPercent() const
{
	return HookDelayTimer / HookMaxDelay;
}

void APlayerBase::OnMoveForward(const float Value)
{
	const FRotator YawRotation(0, SpringArm->GetRelativeRotation().Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void APlayerBase::OnMoveRight(const float Value)
{
	const FRotator YawRotation(0, SpringArm->GetRelativeRotation().Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void APlayerBase::DebugUpdate(const float DeltaTime) const
{
#ifdef _DEBUG_VAULTING
	DrawDebugCapsule(GetWorld(), GetActorLocation(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), GetActorRotation().Quaternion(), FColor::Red, false, -1, 0, 2.f);
#endif
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("Current state: %i"), _HookCurrentState);
#endif
#ifdef _DEBUG_ANIM_STATES
	const int LocomotionIndex = GetMesh()->GetAnimInstance()->GetStateMachineIndex(TEXT("Locomotion"));
	const auto StateName = GetMesh()->GetAnimInstance()->GetCurrentStateName(LocomotionIndex);
	UE_LOG(LogTemp, Warning, TEXT("state name: %s"), *StateName.ToString());
#endif
}

void APlayerBase::CreateComponents()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	SpringArm->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArm);

	GrappleCable = CreateDefaultSubobject<UCableComponent>(TEXT("Grapple Cable"));

	ConstructorHelpers::FClassFinder<AActor> OBJ(TEXT("/Game/Blueprints/FastTest"));
	if (OBJ.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("Obj found"));
		SpawnThisClass = OBJ.Class;
}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Obj not found"));
	}
}
