#pragma once

#include "GameObject.h"
#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(MYMapTool)

class CBackGround final : public CGameObject
{
public:
	typedef struct BackGround_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		// 값이 필요하다면 채워서 활용
	}BACKGROUND_DESC;
private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	_float		m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
};

END