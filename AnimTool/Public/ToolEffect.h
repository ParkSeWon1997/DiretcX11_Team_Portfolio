#pragma once

#include "AnimTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(AnimTool)

class CToolEffect final : public CGameObject
{
	typedef struct ToolEffect_Desc : public GAMEOBJECT_DESC
	{
		_float4			vStartPos;
	}TOOLEFFECT_DESC;

private:
	CToolEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolEffect(const CToolEffect& rhs);
	virtual ~CToolEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Instance_Point*	m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CToolEffect*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END