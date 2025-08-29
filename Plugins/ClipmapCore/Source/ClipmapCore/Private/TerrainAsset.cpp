#include "TerrainAsset.h"

void UTerrainAsset::PostEditChangeProperty(FPropertyChangedEvent& propertyChanged)
{
	Super::PostEditChangeProperty(propertyChanged);

}

void UTerrainAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	int64 numHeights = Heights.Num();
	Ar << numHeights;
	Heights.SetNum(numHeights);
	for (int64 i = 0; i < numHeights; i++)
	{
		Ar << Heights[i];
	}
}