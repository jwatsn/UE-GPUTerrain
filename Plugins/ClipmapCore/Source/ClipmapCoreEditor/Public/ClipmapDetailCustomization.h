#pragma once

#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class UTerrainAsset;
//////////////////////////////////////////////////////////////////////////
// FClipmapActorDetailsCustomization

class FClipmapActorDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	// End of IDetailCustomization interface

	void ImportPNGHeightmap(UTerrainAsset* Asset,const FString& path);
};
