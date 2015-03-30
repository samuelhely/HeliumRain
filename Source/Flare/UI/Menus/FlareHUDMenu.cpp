
#include "../../Flare.h"
#include "FlareHUDMenu.h"
#include "../../Player/FlarePlayerController.h"


#define LOCTEXT_NAMESPACE "FlareHUDMenu"


/*----------------------------------------------------
	Construct
----------------------------------------------------*/

void SFlareHUDMenu::Construct(const FArguments& InArgs)
{
	// Data
	OwnerHUD = InArgs._OwnerHUD;
	TargetShip = NULL;
	Overheating = false;
	PowerOutage = false;
	PresentationFlashTime = 1.0f;
	TimeSinceOverheatChanged = PresentationFlashTime;
	TimeSinceOutageChanged = PresentationFlashTime;
	AFlarePlayerController* PC = Cast<AFlarePlayerController>(OwnerHUD->GetOwner());

	// Style
	const FFlareContainerStyle* ContainerStyle = &FFlareStyleSet::Get().GetWidgetStyle<FFlareContainerStyle>("/Style/DefaultContainerStyle");
	const FFlareContainerStyle* InvertedContainerStyle = &FFlareStyleSet::Get().GetWidgetStyle<FFlareContainerStyle>("/Style/InvertedContainerStyle");
	
	// Structure
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.Padding(FMargin(0))
	[
		SNew(SVerticalBox)

		// Text notification box
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SVerticalBox)

			// Speed
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SVerticalBox)

					// Status string
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0))
					[
						// Background
						SNew(SBorder)
						.BorderImage(&ContainerStyle->BackgroundBrush)
						[
							SNew(STextBlock)
							.Text(this, &SFlareHUDMenu::GetSpeedText)
							.TextStyle(FFlareStyleSet::Get(), "Flare.Text")
							.Justification(ETextJustify::Center)
						]
					]

					// Speed text
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0))
					.HAlign(HAlign_Center)
					[
						// Background
						SNew(SBorder)
						.Padding(FMargin(10, 0))
						.BorderImage(&InvertedContainerStyle->BackgroundBrush)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Speed", "CURRENT SPEED"))
							.TextStyle(FFlareStyleSet::Get(), "Flare.VerySmallTextInverted")
							.Justification(ETextJustify::Center)
						]
					]
				]
			]

			// Overheating box
			+ SVerticalBox::Slot()
			.Padding(FMargin(0, 100))
			.AutoHeight()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.BorderImage(&ContainerStyle->BackgroundBrush)
				.BorderBackgroundColor(this, &SFlareHUDMenu::GetOverheatBackgroundColor)
				[
					SNew(SHorizontalBox)

					// Overheating icon
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FFlareStyleSet::GetIcon("HUD_Temperature"))
						.ColorAndOpacity(this, &SFlareHUDMenu::GetOverheatColor, false)
					]

					// Overheating text
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FFlareStyleSet::Get(), "Flare.Title1")
						.Text(LOCTEXT("Overheating", "OVERHEATING"))
						.ColorAndOpacity(this, &SFlareHUDMenu::GetOverheatColor, true)
					]
				]
			]

			// Outage box
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.BorderImage(&ContainerStyle->BackgroundBrush)
				.BorderBackgroundColor(this, &SFlareHUDMenu::GetOutageBackgroundColor)
				[
					SNew(SHorizontalBox)

					// Outage icon
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FFlareStyleSet::GetIcon("HUD_Power"))
						.ColorAndOpacity(this, &SFlareHUDMenu::GetOutageColor, false)
					]

					// Outage text
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FFlareStyleSet::Get(), "Flare.Title1")
						.Text(LOCTEXT("PowerOutage", "POWER OUTAGE"))
						.ColorAndOpacity(this, &SFlareHUDMenu::GetOutageColor, true)
					]
				]
			]
		]

		// Main (bottom) panel
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		[
			SNew(SHorizontalBox)

			// Static container
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(TemperatureStatus, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_Temperature)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(PowerStatus, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_Power)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(PropulsionStatus, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_Propulsion)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(RCSStatus, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_RCS)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(LifeSupportStatus, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_LifeSupport)
				]
			]

			// Weapon container
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(WeaponContainer, SHorizontalBox)
			]
		]
	];
}


/*----------------------------------------------------
	Interaction
----------------------------------------------------*/

void SFlareHUDMenu::SetTargetShip(IFlareShipInterface* Target)
{
	// Set targets
	TargetShip = Target;
	TemperatureStatus->SetTargetShip(Target);
	PowerStatus->SetTargetShip(Target);
	PropulsionStatus->SetTargetShip(Target);
	RCSStatus->SetTargetShip(Target);
	LifeSupportStatus->SetTargetShip(Target);
	AFlareShip* PlayerShip = Cast<AFlareShip>(Target);

	if (PlayerShip)
	{
		// Update weapon list
		TArray<UFlareWeapon*> WeaponList = PlayerShip->GetWeaponList();
		TSharedPtr<SFlareSubsystemStatus> Temp;
		WeaponContainer->ClearChildren();

		// Add weapon indicators
		for (int32 i = 0; i < WeaponList.Num(); i++)
		{
			WeaponContainer->AddSlot()
				.AutoWidth()
				[
					SAssignNew(Temp, SFlareSubsystemStatus)
					.Subsystem(EFlareSubsystem::SYS_Weapon)
				];
			Temp->SetTargetShip(PlayerShip);
			Temp->SetTargetComponent(WeaponList[i]);
		}
		WeaponContainer->SetVisibility(EVisibility::Visible);
	}
}


/*----------------------------------------------------
	Callbacks
----------------------------------------------------*/

void SFlareHUDMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (TargetShip)
	{
		// Overheating status
		TimeSinceOverheatChanged += InDeltaTime;
		bool NewOverheating = (TargetShip->GetTemperature() > TargetShip->GetMaxTemperature());
		if (NewOverheating != Overheating)
		{
			TimeSinceOverheatChanged = 0;
		}
		Overheating = NewOverheating;

		// Outage status
		TimeSinceOutageChanged += InDeltaTime;
		bool NewPowerOutage = TargetShip->HasPowerOutage();
		if (NewPowerOutage != PowerOutage)
		{
			TimeSinceOutageChanged = 0;
		}
		PowerOutage = NewPowerOutage;
	}
}

FText SFlareHUDMenu::GetSpeedText() const
{
	if (TargetShip)
	{
		AFlareShip* PlayerShip = Cast<AFlareShip>(TargetShip);
		if (PlayerShip)
		{
			return FText::FromString("\n" + FString::FromInt(PlayerShip->GetLinearVelocity().Size()) + " m/s");
		}
	}

	return FText::FromString("");
}

FSlateColor SFlareHUDMenu::GetOverheatColor(bool Text) const
{
	FLinearColor Color = FFlareStyleSet::GetHeatColor();
	float Ratio = FMath::Clamp(TimeSinceOverheatChanged / PresentationFlashTime, 0.0f, 1.0f);
	Color.A *= (Overheating ? Ratio : (1 - Ratio));

	if (Text)
	{
		Color.A *= 0.7;
	}

	return Color;
}

FSlateColor SFlareHUDMenu::GetOverheatBackgroundColor() const
{
	FLinearColor Color = FLinearColor::White;
	Color.A = GetOverheatColor(true).GetSpecifiedColor().A;
	return Color;
}

FSlateColor SFlareHUDMenu::GetOutageColor(bool Text) const
{
	FLinearColor Color = FLinearColor::White;
	float Ratio = FMath::Clamp(TimeSinceOutageChanged / PresentationFlashTime, 0.0f, 1.0f);
	Color.A *= (PowerOutage ? Ratio : (1 - Ratio));

	if (Text)
	{
		Color.A *= 0.7;
	}

	return Color;
}

FSlateColor SFlareHUDMenu::GetOutageBackgroundColor() const
{
	FLinearColor Color = FLinearColor::White;
	Color.A = GetOutageColor(true).GetSpecifiedColor().A;
	return Color;
}


#undef LOCTEXT_NAMESPACE
