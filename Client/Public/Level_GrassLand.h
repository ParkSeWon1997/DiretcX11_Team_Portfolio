#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "LandObject.h"
BEGIN(Client)
class CUI_Manager;


class CLevel_GrassLand final : public CLevel
{
private:
	CLevel_GrassLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GrassLand() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;


private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);	
	HRESULT Ready_Fog();
	HRESULT Ready_Layer_Trigger();
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc);

	HRESULT Ready_Grass(const wstring& strLayerTag);


	HRESULT Load_LevelData(const _tchar* pFilePath);
	HRESULT Load_Data_Effects();
	HRESULT Load_Data_Decals();

	void Load_Lights(const wstring& strLightFile);
	
private:
	CUI_Manager* m_pUI_Manager = nullptr;

//#ifdef _DEBUG
	_uint m_iMainCameraIdx = 0;
	_uint m_iCamSize = 0;
//#endif

public:
	static CLevel_GrassLand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END